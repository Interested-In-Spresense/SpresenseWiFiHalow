/*
 *  BasicWiFi.ino - Basic WiFi Halo example
 *  
 *  This example demonstrates basic WiFi connection using SpresenseWiFiHalow library.
 */

#include "SpresenseWiFiHalow.h"

// WiFi network credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\nSpresense WiFi Halo Example");
  Serial.println("============================\n");
  
  // Initialize WiFi
  Serial.println("Initializing WiFi...");
  if (SpresenseWiFiHalow.begin() < 0) {
    Serial.println("ERROR: WiFi initialization failed!");
    return;
  }
  
  Serial.println("WiFi initialized successfully.");
  
  // Connect to WiFi network
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  if (SpresenseWiFiHalow.connect(ssid, password) < 0) {
    Serial.println("ERROR: Connection failed!");
    return;
  }
  
  // Wait for connection
  int timeout = 30;
  while (SpresenseWiFiHalow.status() != WL_CONNECTED && timeout-- > 0) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println();
  
  if (SpresenseWiFiHalow.status() == WL_CONNECTED) {
    Serial.println("WiFi connected successfully!");
    
    // Print connection information
    Serial.print("SSID: ");
    Serial.println(SpresenseWiFiHalow.SSID());
    
    Serial.print("IP Address: ");
    Serial.println(SpresenseWiFiHalow.localIP().toString());
    
    Serial.print("Gateway: ");
    Serial.println(SpresenseWiFiHalow.gatewayIP().toString());
    
    Serial.print("Subnet Mask: ");
    Serial.println(SpresenseWiFiHalow.subnetMask().toString());
  } else {
    Serial.println("ERROR: Failed to connect to WiFi!");
  }
}

void loop() {
  delay(1000);
}
