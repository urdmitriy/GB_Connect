#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define SERVICE_BATTERY BLEUUID((uint16_t)0x180F)
#define SERVICE_LEDLIGHT BLEUUID((uint16_t)0x1815)

BLECharacteristic battLevelCharact(BLEUUID((uint16_t) 0x2A19),BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
BLECharacteristic ledLightCharact(BLEUUID((uint16_t) 0x2A3F), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

BLEDescriptor battLevelDescriptor(BLEUUID((uint16_t)0x2901));
BLEDescriptor ledLightDescriptor(BLEUUID((uint16_t)0x2901));

class LEDCallBacks : public BLECharacteristicCallbacks
{
private:
  /* data */
public:
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();

    if (value == "1"){
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else
    {
      digitalWrite(LED_BUILTIN, LOW);
    }
    Serial.print("Write data ");

  }
};



void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("Starting service");

  BLEDevice::init("GB IoT device");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *battService = pServer->createService(SERVICE_BATTERY);
  BLEService *ledLightService = pServer->createService(SERVICE_LEDLIGHT);
  
  battService->addCharacteristic(&battLevelCharact);
  battLevelCharact.addDescriptor(new BLE2902());

  ledLightService->addCharacteristic(&ledLightCharact);
  ledLightCharact.addDescriptor(new BLE2902());

  battLevelDescriptor.setValue("Level from 0 to 100");
  battLevelCharact.addDescriptor(&battLevelDescriptor);

  ledLightDescriptor.setValue("1 - ON, 0 - OFF");
  ledLightCharact.addDescriptor(&ledLightDescriptor);
  ledLightCharact.setCallbacks(new LEDCallBacks());

  battService->start();
  ledLightService->start();

  BLEAdvertising *battLevelAdv = BLEDevice::getAdvertising();
  battLevelAdv->addServiceUUID(SERVICE_BATTERY);
  battLevelAdv->setScanResponse(true);
  battLevelAdv->start();

  BLEAdvertising *ledLightAdv = BLEDevice::getAdvertising();
  ledLightAdv->addServiceUUID(SERVICE_LEDLIGHT);
  ledLightAdv->setScanResponse(true);
  ledLightAdv->start();
}

uint8_t level = 12;


void loop() {
  battLevelCharact.setValue(&level,1);
  battLevelCharact.notify();
  delay(1000);
  level++;
  if ((int)level > 100){
    level = 0;
  }
  // Serial.printf("Level: %d\n\r", level);
  
  
}