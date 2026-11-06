/*
 *  SpresenseWiFiHalow.h - Spresense WiFi Halo wrapper library header.
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

#ifndef _SPRESENSE_WIFI_HALOW_H_
#define _SPRESENSE_WIFI_HALOW_H

#include <Arduino.h>
#include <stdint.h>
#include <string.h>

/**************************************************************************
 * Constants
 **************************************************************************/

#define WL_SSID_MAX_LENGTH         32
#define WL_MAX_SCAN_RESULTS        15
#define WL_MAC_ADDR_LENGTH         6
#define WL_IPV4_LENGTH             4

// WiFi status constants
typedef enum {
  WL_IDLE_STATUS       = 0,
  WL_NO_SSID_AVAIL     = 1,
  WL_SCAN_COMPLETED    = 2,
  WL_CONNECTED         = 3,
  WL_CONNECT_FAILED    = 4,
  WL_CONNECTION_LOST   = 5,
  WL_DISCONNECTED      = 6,
  WL_AP_LISTENING      = 7,
  WL_AP_CONNECTED      = 8,
  WL_AP_FAILED         = 9,
} wl_status_t;

// Encryption types
typedef enum {
  ENC_TYPE_OPEN   = 0,
  ENC_TYPE_WEP    = 1,
  ENC_TYPE_TKIP   = 2,
  ENC_TYPE_CCMP   = 3,
  ENC_TYPE_AUTO   = 4,
} wl_enc_type;

/**************************************************************************
 * Structures
 **************************************************************************/

struct IPAddress {
  uint8_t b[4];

  IPAddress() {}
  IPAddress(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth) {
    b[0] = first;
    b[1] = second;
    b[2] = third;
    b[3] = fourth;
  }
  
  IPAddress(uint32_t address) {
    memcpy(b, &address, 4);
  }

  bool operator==(const IPAddress& other) const {
    return memcmp(b, other.b, 4) == 0;
  }

  bool operator!=(const IPAddress& other) const {
    return !(*this == other);
  }

  void print() const {
    printf("%d.%d.%d.%d\n", b[0], b[1], b[2], b[3]);
  }

  void println() const {
    print();
  }

  String toString() const {
    char buf[16];
    snprintf(buf, sizeof(buf), "%d.%d.%d.%d", b[0], b[1], b[2], b[3]);
    return String(buf);
  }
};

/**************************************************************************
 * WiFi Scan Result Structure
 **************************************************************************/

struct wl_scan_result {
  char ssid[WL_SSID_MAX_LENGTH + 1];
  uint8_t bssid[WL_MAC_ADDR_LENGTH];
  int8_t rssi;
  wl_enc_type enc_type;
  uint8_t channel;
};

/**************************************************************************
 * WiFi Client/Server Classes
 **************************************************************************/

class WiFiClient {
public:
  WiFiClient() : _sockfd(-1) {}
  virtual ~WiFiClient();

  int connect(const char* host, uint16_t port);
  int connect(const IPAddress& ip, uint16_t port);
  
  size_t write(uint8_t b);
  size_t write(const uint8_t *buf, size_t size);
  int read();
  int read(uint8_t *buf, size_t size);
  
  int available();
  void flush();
  void stop();
  
  uint8_t connected();

private:
  int _sockfd;
};

class WiFiServer {
public:
  WiFiServer(uint16_t port) : _port(port), _sockfd(-1) {}
  virtual ~WiFiServer();

  int begin();
  WiFiClient available();
  void stop();

private:
  uint16_t _port;
  int _sockfd;
};

/**************************************************************************
 * WiFiUDP Class
 **************************************************************************/

class WiFiUDP {
public:
  WiFiUDP() : _sockfd(-1) {}
  virtual ~WiFiUDP();

  int begin(uint16_t port);
  int beginMulticast(const IPAddress& ip, uint16_t port);
  
  int write(uint8_t b);
  int write(const uint8_t *buf, size_t size);
  
  int parsePacket();
  int read(uint8_t *buf, size_t size);
  int read();
  
  IPAddress remoteIP();
  uint16_t remotePort();
  
  int endPacket();
  void stop();

private:
  int _sockfd;
  IPAddress _remoteIP;
  uint16_t _remotePort;
};

/**************************************************************************
 * Main WiFi Class
 **************************************************************************/

class SpresenseWiFiHalowClass {
public:
  SpresenseWiFiHalowClass();
  virtual ~SpresenseWiFiHalowClass() {}

  // Initialization
  int begin();
  int end();
  
  // Connection Management
  int connect(const char* ssid, const char* password);
  int connect(const char* ssid, const char* password, uint8_t keyIndex);
  int disconnect();
  
  // Status and Information
  wl_status_t status();
  const char* SSID();
  const char* SSID(uint8_t networkItem);
  int8_t RSSI();
  int8_t RSSI(uint8_t networkItem);
  wl_enc_type encryptionType(uint8_t networkItem);
  
  // IP Configuration
  IPAddress localIP();
  IPAddress subnetMask();
  IPAddress gatewayIP();
  IPAddress dnsIP(uint8_t dnsnum = 0);
  
  // MAC Address
  uint8_t* macAddress(uint8_t* mac);
  
  // WiFi Scanning
  int8_t scanNetworks();
  void scanDelete();
  
  // DHCP
  int hostByName(const char* hostname, IPAddress& aResult);
  
  // Deprecated functions for compatibility
  uint8_t getSocket();
  void setPins(uint8_t ss, uint8_t irq, uint8_t rst, uint8_t en);

private:
  // Internal state
  wl_status_t _status;
  char _ssid[WL_SSID_MAX_LENGTH + 1];
  wl_scan_result _scanResults[WL_MAX_SCAN_RESULTS];
  uint8_t _scanResultsCount;
  IPAddress _ip;
  IPAddress _gateway;
  IPAddress _subnet;
  uint8_t _macaddr[WL_MAC_ADDR_LENGTH];
  
  // Helper functions
  int _initializeModem();
  int _connectNetwork(const char* ssid, const char* password);
};

extern SpresenseWiFiHalowClass SpresenseWiFiHalow;

#endif // _SPRESENSE_WIFI_HALOW_H_
