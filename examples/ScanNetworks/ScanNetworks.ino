/*
 *  ScanNetworks.ino - WiFi network scan example
 *  
 *  This example demonstrates WiFi network scanning using SpresenseWiFiHalow library.
 */

#include "SpresenseWiFiHalow.h"

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\nSpresense WiFi Scan Example");
  Serial.println("============================\n");
  
  // Initialize WiFi
  Serial.println("Initializing WiFi...");
  if (SpresenseWiFiHalow.begin() < 0) {
    Serial.println("ERROR: WiFi initialization failed!");
    return;
  }
  
  // Scan for available networks
  Serial.println("\nScanning for available WiFi networks...\n");
  
  int numNetworks = SpresenseWiFiHalow.scanNetworks();
  
  if (numNetworks < 0) {
    Serial.println("ERROR: WiFi scan failed!");
    return;
  }
  
  Serial.print("Found ");
  Serial.print(numNetworks);
  Serial.println(" networks:\n");
  
  // Print details about each network
  for (int i = 0; i < numNetworks; i++) {
    Serial.print("Network #");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(SpresenseWiFiHalow.SSID(i));
    Serial.print(" (RSSI: ");
    Serial.print(SpresenseWiFiHalow.RSSI(i));
    Serial.print("dBm)");
    
    wl_enc_type enc = SpresenseWiFiHalow.encryptionType(i);
    Serial.print(" [");
    switch (enc) {
      case ENC_TYPE_OPEN:
        Serial.print("OPEN");
        break;
      case ENC_TYPE_WEP:
        Serial.print("WEP");
        break;
      case ENC_TYPE_TKIP:
        Serial.print("WPA");
        break;
      case ENC_TYPE_CCMP:
        Serial.print("WPA2");
        break;
      case ENC_TYPE_AUTO:
        Serial.print("AUTO");
        break;
      default:
        Serial.print("UNKNOWN");
    }
    Serial.println("]");
  }
  
  Serial.println("\nScan completed.");
}

void loop() {
  delay(1000);
}
