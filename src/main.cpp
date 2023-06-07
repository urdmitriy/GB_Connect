#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <TFT_eSPI.h>

#define SERVICE_BATTERY BLEUUID((uint16_t)0x180F)

BLECharacteristic battLevelCharact(BLEUUID((uint16_t) 0x2A19),BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor battLevelDescriptor(BLEUUID((uint16_t)0x2901));

TFT_eSPI tft = TFT_eSPI();  // Invoke custom library

void setup() {
  Serial.begin(115200);

  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 4);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  

  Serial.println("Starting service");

  BLEDevice::init("GB IoT device");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *battService = pServer->createService(SERVICE_BATTERY);
  
  battService->addCharacteristic(&battLevelCharact);
  battLevelCharact.addDescriptor(new BLE2902());

  battLevelDescriptor.setValue("Level from 0 to 100");
  battLevelCharact.addDescriptor(&battLevelDescriptor);

  battService->start();

  BLEAdvertising *battLevelAdv = BLEDevice::getAdvertising();
  battLevelAdv->addServiceUUID(SERVICE_BATTERY);
  battLevelAdv->setScanResponse(true);
  battLevelAdv->start();
}

float read_voltage(uint8_t pin){
  uint16_t adc_in = analogRead(pin);
  static float voltage_old = 0;
  float voltage = round(((float)adc_in * 4.2 / 4096)*100)/100;
  float EMA = 0.01f * voltage + (1.0f-0.01) * voltage_old;
  voltage_old = voltage;
  return EMA;
}

uint8_t convert_percent(float value){
  if (value < 3.5) value = 3.5;
  uint8_t percent = 142.86 * value - 500;
  return percent;
}

void loop() {
  static float voltage_old = 0;
  float voltage = read_voltage(2);
  if (voltage_old != voltage){
    tft.setCursor(0, 0, 4);
    tft.setTextColor(TFT_BLACK, TFT_BLACK);
    tft.printf("V_bat = %3.2f V", voltage_old);

    tft.setCursor(0, 30, 4);
    tft.setTextColor(TFT_BLACK, TFT_BLACK);
    tft.printf("Level = %d pr", convert_percent(voltage_old));

    tft.setCursor(0, 0, 4);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.printf("V_bat = %3.2f V", voltage);

    tft.setCursor(0, 30, 4);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    uint8_t level = convert_percent(voltage);
    tft.printf("Level = %d pr", level);

    battLevelCharact.setValue(&level,1);
    battLevelCharact.notify();

    voltage_old = voltage;
  }
  
  
  delay(500);

}