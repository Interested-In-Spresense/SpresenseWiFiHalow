/*
 *  HTTPClient.ino - Simple HTTP client example
 *  
 *  This example demonstrates basic HTTP communication using SpresenseWiFiHalow library.
 */

#include "SpresenseWiFiHalow.h"

// WiFi network credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// HTTP server details
const char* host = "example.com";
const uint16_t port = 80;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\nSpresense WiFi HTTP Client Example");
  Serial.println("===================================\n");
  
  // Initialize WiFi
  Serial.println("Initializing WiFi...");
  if (SpresenseWiFiHalow.begin() < 0) {
    Serial.println("ERROR: WiFi initialization failed!");
    return;
  }
  
  // Connect to WiFi network
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  if (SpresenseWiFiHalow.connect(ssid, password) < 0) {
    Serial.println("ERROR: WiFi connection failed!");
    return;
  }
  
  // Wait for connection
  int timeout = 30;
  while (SpresenseWiFiHalow.status() != WL_CONNECTED && timeout-- > 0) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println();
  
  if (SpresenseWiFiHalow.status() != WL_CONNECTED) {
    Serial.println("ERROR: Failed to connect to WiFi!");
    return;
  }
  
  Serial.println("WiFi connected!\n");
  
  // Make HTTP request
  performHTTPRequest();
}

void performHTTPRequest() {
  WiFiClient client;
  
  Serial.print("Connecting to ");
  Serial.print(host);
  Serial.print(":");
  Serial.println(port);
  
  if (!client.connect(host, port)) {
    Serial.println("ERROR: Failed to connect to server!");
    return;
  }
  
  Serial.println("Connected to server!");
  
  // Send HTTP GET request
  Serial.println("Sending HTTP GET request...\n");
  
  String request = "GET / HTTP/1.1\r\n";
  request += "Host: " + String(host) + "\r\n";
  request += "Connection: close\r\n";
  request += "User-Agent: SpresenseWiFiHalow\r\n";
  request += "\r\n";
  
  client.write((const uint8_t*)request.c_str(), request.length());
  
  // Read response
  Serial.println("Response:");
  Serial.println("----------");
  
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      Serial.write(c);
    }
  }
  
  client.stop();
  
  Serial.println("\n----------");
  Serial.println("Request completed!");
}

void loop() {
  delay(1000);
}
