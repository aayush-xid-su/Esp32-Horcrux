#include <BLEDevice.h>
#include <BLEAdvertising.h>

bool spamRunning = true;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  BLEDevice::init("AYUSH_Spammer");
  
  Serial.println("\n=== Strong BLE Spammer Started ===");
}

void loop() {
  if (spamRunning) {
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    
    for (int i = 0; i < 12; i++) {
      String name = "AYUSH_" + String(random(1000, 99999));
      
      BLEAdvertisementData advertisementData;
      advertisementData.setName(name.c_str());
      
      String data = String("AY") + String(random(100, 999));
      advertisementData.setManufacturerData(data);
      
      pAdvertising->setAdvertisementData(advertisementData);
      pAdvertising->setScanResponseData(advertisementData);
      
      pAdvertising->start();
      delay(25);
      pAdvertising->stop();
      delay(5);
    }
  }
  
  delay(100);
  
  static unsigned long last = 0;
  if (millis() - last > 5000) {
    last = millis();
    Serial.println("BLE Spamming Active - Check nRF Connect");
  }
}
