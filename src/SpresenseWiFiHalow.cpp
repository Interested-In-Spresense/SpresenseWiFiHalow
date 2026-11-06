/*
 *  SpresenseWiFiHalow.cpp - Spresense WiFi Halo wrapper library implementation.
 *  Author Interested-In-Spresense
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "SpresenseWiFiHalow.h"

/**************************************************************************
 * WiFiClient Implementation
 **************************************************************************/

WiFiClient::~WiFiClient() {
  if (_sockfd >= 0) {
    close(_sockfd);
  }
}

int WiFiClient::connect(const char* host, uint16_t port) {
  IPAddress ip;
  
  if (SpresenseWiFiHalow.hostByName(host, ip) < 0) {
    return 0;
  }
  
  return connect(ip, port);
}

int WiFiClient::connect(const IPAddress& ip, uint16_t port) {
  struct sockaddr_in addr;
  
  _sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (_sockfd < 0) {
    return 0;
  }
  
  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = *(uint32_t*)ip.b;
  
  if (::connect(_sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    close(_sockfd);
    _sockfd = -1;
    return 0;
  }
  
  return 1;
}

size_t WiFiClient::write(uint8_t b) {
  return write(&b, 1);
}

size_t WiFiClient::write(const uint8_t *buf, size_t size) {
  if (_sockfd < 0) {
    return 0;
  }
  
  int n = send(_sockfd, (const void*)buf, size, 0);
  return (n > 0) ? n : 0;
}

int WiFiClient::read() {
  uint8_t b;
  int n = read(&b, 1);
  return (n > 0) ? b : -1;
}

int WiFiClient::read(uint8_t *buf, size_t size) {
  if (_sockfd < 0) {
    return 0;
  }
  
  int n = recv(_sockfd, (void*)buf, size, 0);
  return (n >= 0) ? n : 0;
}

int WiFiClient::available() {
  if (_sockfd < 0) {
    return 0;
  }
  
  int n = 0;
  if (ioctl(_sockfd, FIONREAD, &n) < 0) {
    return 0;
  }
  
  return n;
}

void WiFiClient::flush() {
  // No-op for now
}

void WiFiClient::stop() {
  if (_sockfd >= 0) {
    close(_sockfd);
    _sockfd = -1;
  }
}

uint8_t WiFiClient::connected() {
  return (_sockfd >= 0) ? 1 : 0;
}

/**************************************************************************
 * WiFiServer Implementation
 **************************************************************************/

WiFiServer::~WiFiServer() {
  if (_sockfd >= 0) {
    close(_sockfd);
  }
}

int WiFiServer::begin() {
  struct sockaddr_in addr;
  
  _sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (_sockfd < 0) {
    return 0;
  }
  
  int reuse = 1;
  if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
    close(_sockfd);
    _sockfd = -1;
    return 0;
  }
  
  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(_port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  
  if (bind(_sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    close(_sockfd);
    _sockfd = -1;
    return 0;
  }
  
  if (listen(_sockfd, 1) < 0) {
    close(_sockfd);
    _sockfd = -1;
    return 0;
  }
  
  return 1;
}

WiFiClient WiFiServer::available() {
  WiFiClient client;
  struct sockaddr_in addr;
  socklen_t addrlen = sizeof(addr);
  
  if (_sockfd < 0) {
    return client;
  }
  
  int newsockfd = accept(_sockfd, (struct sockaddr*)&addr, &addrlen);
  if (newsockfd >= 0) {
    client._sockfd = newsockfd;
  }
  
  return client;
}

void WiFiServer::stop() {
  if (_sockfd >= 0) {
    close(_sockfd);
    _sockfd = -1;
  }
}

/**************************************************************************
 * WiFiUDP Implementation
 **************************************************************************/

WiFiUDP::~WiFiUDP() {
  if (_sockfd >= 0) {
    close(_sockfd);
  }
}

int WiFiUDP::begin(uint16_t port) {
  struct sockaddr_in addr;
  
  _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (_sockfd < 0) {
    return 0;
  }
  
  int reuse = 1;
  if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
    close(_sockfd);
    _sockfd = -1;
    return 0;
  }
  
  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  
  if (bind(_sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    close(_sockfd);
    _sockfd = -1;
    return 0;
  }
  
  return 1;
}

int WiFiUDP::beginMulticast(const IPAddress& ip, uint16_t port) {
  if (begin(port) < 0) {
    return 0;
  }
  
  struct ip_mreq imreq;
  imreq.imr_multiaddr.s_addr = *(uint32_t*)ip.b;
  imreq.imr_interface.s_addr = htonl(INADDR_ANY);
  
  if (setsockopt(_sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imreq, sizeof(imreq)) < 0) {
    close(_sockfd);
    _sockfd = -1;
    return 0;
  }
  
  return 1;
}

int WiFiUDP::write(uint8_t b) {
  return write(&b, 1);
}

int WiFiUDP::write(const uint8_t *buf, size_t size) {
  if (_sockfd < 0) {
    return 0;
  }
  
  int n = send(_sockfd, (const void*)buf, size, 0);
  return (n > 0) ? n : 0;
}

int WiFiUDP::parsePacket() {
  struct sockaddr_in addr;
  socklen_t addrlen = sizeof(addr);
  uint8_t buf[1500];
  
  if (_sockfd < 0) {
    return 0;
  }
  
  int n = recvfrom(_sockfd, (void*)buf, sizeof(buf), MSG_PEEK, (struct sockaddr*)&addr, &addrlen);
  if (n < 0) {
    return 0;
  }
  
  _remoteIP = IPAddress(addr.sin_addr.s_addr);
  _remotePort = ntohs(addr.sin_port);
  
  return n;
}

int WiFiUDP::read(uint8_t *buf, size_t size) {
  struct sockaddr_in addr;
  socklen_t addrlen = sizeof(addr);
  
  if (_sockfd < 0) {
    return 0;
  }
  
  int n = recvfrom(_sockfd, (void*)buf, size, 0, (struct sockaddr*)&addr, &addrlen);
  return (n >= 0) ? n : 0;
}

int WiFiUDP::read() {
  uint8_t b;
  int n = read(&b, 1);
  return (n > 0) ? b : -1;
}

IPAddress WiFiUDP::remoteIP() {
  return _remoteIP;
}

uint16_t WiFiUDP::remotePort() {
  return _remotePort;
}

int WiFiUDP::endPacket() {
  return 1;
}

void WiFiUDP::stop() {
  if (_sockfd >= 0) {
    close(_sockfd);
    _sockfd = -1;
  }
}

/**************************************************************************
 * SpresenseWiFiHalowClass Implementation
 **************************************************************************/

SpresenseWiFiHalowClass::SpresenseWiFiHalowClass()
  : _status(WL_IDLE_STATUS), _scanResultsCount(0)
{
  memset(_ssid, 0, sizeof(_ssid));
  memset(_scanResults, 0, sizeof(_scanResults));
  memset(_macaddr, 0, sizeof(_macaddr));
}

int SpresenseWiFiHalowClass::begin() {
  // TODO: Initialize WiFi Halo hardware via Spresense SDK
  // This should call the appropriate Spresense WiFi driver initialization
  
  _status = WL_IDLE_STATUS;
  return _initializeModem();
}

int SpresenseWiFiHalowClass::end() {
  // TODO: Shutdown WiFi Halo hardware
  disconnect();
  _status = WL_IDLE_STATUS;
  return 0;
}

int SpresenseWiFiHalowClass::connect(const char* ssid, const char* password) {
  return connect(ssid, password, 0);
}

int SpresenseWiFiHalowClass::connect(const char* ssid, const char* password, uint8_t keyIndex) {
  strncpy(_ssid, ssid, WL_SSID_MAX_LENGTH);
  _ssid[WL_SSID_MAX_LENGTH] = '\0';
  
  return _connectNetwork(ssid, password);
}

int SpresenseWiFiHalowClass::disconnect() {
  // TODO: Disconnect from current WiFi network
  _status = WL_DISCONNECTED;
  memset(_ssid, 0, sizeof(_ssid));
  return 0;
}

wl_status_t SpresenseWiFiHalowClass::status() {
  return _status;
}

const char* SpresenseWiFiHalowClass::SSID() {
  return _ssid;
}

const char* SpresenseWiFiHalowClass::SSID(uint8_t networkItem) {
  if (networkItem >= _scanResultsCount) {
    return "";
  }
  return _scanResults[networkItem].ssid;
}

int8_t SpresenseWiFiHalowClass::RSSI() {
  if (_status != WL_CONNECTED) {
    return 0;
  }
  
  // TODO: Get current RSSI from driver
  return 0;
}

int8_t SpresenseWiFiHalowClass::RSSI(uint8_t networkItem) {
  if (networkItem >= _scanResultsCount) {
    return 0;
  }
  return _scanResults[networkItem].rssi;
}

wl_enc_type SpresenseWiFiHalowClass::encryptionType(uint8_t networkItem) {
  if (networkItem >= _scanResultsCount) {
    return ENC_TYPE_OPEN;
  }
  return _scanResults[networkItem].enc_type;
}

IPAddress SpresenseWiFiHalowClass::localIP() {
  return _ip;
}

IPAddress SpresenseWiFiHalowClass::subnetMask() {
  return _subnet;
}

IPAddress SpresenseWiFiHalowClass::gatewayIP() {
  return _gateway;
}

IPAddress SpresenseWiFiHalowClass::dnsIP(uint8_t dnsnum) {
  // TODO: Return DNS IP address
  return IPAddress(0, 0, 0, 0);
}

uint8_t* SpresenseWiFiHalowClass::macAddress(uint8_t* mac) {
  memcpy(mac, _macaddr, WL_MAC_ADDR_LENGTH);
  return mac;
}

int8_t SpresenseWiFiHalowClass::scanNetworks() {
  // TODO: Perform WiFi network scan via Spresense SDK
  return _scanResultsCount;
}

void SpresenseWiFiHalowClass::scanDelete() {
  memset(_scanResults, 0, sizeof(_scanResults));
  _scanResultsCount = 0;
}

int SpresenseWiFiHalowClass::hostByName(const char* hostname, IPAddress& aResult) {
  // TODO: Perform DNS lookup
  struct hostent *host = gethostbyname(hostname);
  
  if (host == NULL) {
    return -1;
  }
  
  if (host->h_addr_list[0] != NULL) {
    aResult = IPAddress(*(uint32_t*)host->h_addr_list[0]);
    return 0;
  }
  
  return -1;
}

uint8_t SpresenseWiFiHalowClass::getSocket() {
  // Deprecated - return dummy value
  return 0;
}

void SpresenseWiFiHalowClass::setPins(uint8_t ss, uint8_t irq, uint8_t rst, uint8_t en) {
  // Deprecated - WiFi Halo doesn't need pin configuration
}

int SpresenseWiFiHalowClass::_initializeModem() {
  // TODO: Call actual Spresense SDK initialization functions
  // Example placeholder:
  //
  // int ret = wifi_halo_initialize();
  // if (ret < 0) {
  //   return -1;
  // }
  //
  // ret = wifi_halo_get_mac(_macaddr);
  // if (ret >= 0) {
  //   _status = WL_IDLE_STATUS;
  //   return 0;
  // }
  
  _status = WL_IDLE_STATUS;
  return 0;
}

int SpresenseWiFiHalowClass::_connectNetwork(const char* ssid, const char* password) {
  // TODO: Call actual Spresense SDK connect functions
  // Example placeholder:
  //
  // int ret = wifi_halo_connect(ssid, password);
  // if (ret < 0) {
  //   _status = WL_CONNECT_FAILED;
  //   return -1;
  // }
  //
  // // Wait for connection
  // int retry = 30;
  // while (retry-- > 0) {
  //   delay(100);
  //   ret = wifi_halo_status();
  //   if (ret == WIFI_STATUS_CONNECTED) {
  //     break;
  //   }
  // }
  //
  // if (ret != WIFI_STATUS_CONNECTED) {
  //   _status = WL_CONNECTION_LOST;
  //   return -1;
  // }
  //
  // uint32_t ip, gateway, subnet, dns;
  // wifi_halo_get_ipconfig(&ip, &gateway, &subnet, &dns);
  //
  // _ip = IPAddress(ip);
  // _gateway = IPAddress(gateway);
  // _subnet = IPAddress(subnet);
  // _status = WL_CONNECTED;
  
  _status = WL_CONNECTED;
  return 0;
}

// Create singleton instance
SpresenseWiFiHalowClass SpresenseWiFiHalow;
