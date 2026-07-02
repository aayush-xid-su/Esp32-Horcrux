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

String randomWords[100] = {
  "Home", "Guest", "Office", "Cafe", "Hotel", "Airport", "School",
  "Public", "Secure", "Link", "Zone", "Premium", "Hotspot", "Fiber", 
  "Smart", "Ultra", "Express", "Speed", "Cloud", "Wave", "Prime",
  "Digital", "Wireless", "Network", "Internet", "Broadband", "Router",
  "Mesh", "Gateway", "Access", "Node", "Hub", "Online", "Private", 
  "Family", "Business", "Campus", "Library", "Studio", "Gaming", 
  "Stream", "Mobile", "Remote", "Virtual", "Crystal", "Elite", 
  "Rapid", "Power", "Turbo", "Infinity", "Mall", "Station", "Metro",
  "BusStop", "Terminal", "Plaza", "Market", "Park", "Garden", 
  "Hospital", "Clinic", "Museum", "Theater", "Cinema", "Arena", 
  "Gym", "Lounge", "Resort", "FoodCourt", "Starbucks", "Costa", 
  "Barista", "Reception", "Admin", "Accounts", "Finance", "HR", 
  "Sales", "Marketing", "Support", "IT", "Engineering", "Security",
  "Operations", "Conference", "Warehouse", "Faculty", "Admissions"
};

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP("AYUSH", NULL, 1, false, 4);
  
  esp_wifi_set_promiscuous(true);
  
  Serial.println("\n=== AYUSH Random Beacon Flooder Started ===");
  Serial.println("Flooding 40 random APs per cycle...");
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
    // 40 random APs per cycle
    for (int i = 0; i < 40; i++) {
      String name = randomWords[random(0, 85)] + "_" + String(random(1000, 99999));
      sendBeacon(name.c_str());
      delay(3);
    }
  }
  
  delay(20);
  
  static unsigned long last = 0;
  if (millis() - last > 3000) {
    last = millis();
    Serial.println("Spamming 40 random networks per cycle...");
  }
}