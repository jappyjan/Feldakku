#include <arduino.h>
#include "screen.hpp"

#include <BLEServer.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <HardwareSerial.h>

HardwareSerial BMSSerial(1);
Screen screen;

static BLEUUID serviceUUID("0000ff00-0000-1000-8000-00805f9b34fb"); //xiaoxiang bms original module
static BLEUUID charUUID_tx("0000ff02-0000-1000-8000-00805f9b34fb"); //xiaoxiang bms original module
static BLEUUID charUUID_rx("0000ff01-0000-1000-8000-00805f9b34fb"); //xiaoxiang bms original module

// define ble callbacks
class MyBleCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    uint8_t *rxValue = pCharacteristic->getData();
    // store length of the rxValue
    int rxLength = pCharacteristic->getValue().length();
    

    BMSSerial.write(rxValue, rxLength);
  }
};

auto bleCb = MyBleCallbacks();

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharacteristicTx;
BLECharacteristic *pCharacteristicRx;
BLEAdvertising *pAdvertising;

void setup() {
  Serial.begin(115200);
  BMSSerial.begin(9600, SERIAL_8N1, 9, 10);

  BLEDevice::init("ESP-Feldakku");
  pServer = BLEDevice::createServer();
  pService = pServer->createService(serviceUUID);
  pCharacteristicTx = pService->createCharacteristic(
                                        charUUID_tx,
                                        BLECharacteristic::PROPERTY_NOTIFY
                                      );
  pCharacteristicRx = pService->createCharacteristic(
                                        charUUID_rx,
                                        BLECharacteristic::PROPERTY_WRITE
                                      );
  pService->start();
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(serviceUUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  // listen for ble messages
  pCharacteristicRx->setCallbacks(&bleCb);
  
  screen.begin();
  screen.setBatteryVoltage(6*4.12);
  screen.setBatteryTemperature(25.0);
  screen.setChargingIsEnabled(true);
  screen.setChargeCurrent(11.0);
  screen.setDischargingIsEnabled(false);
  screen.setDischargeCurrent(0.0);
  screen.setSixSIsEnabled(true);
  screen.setFourSOutputIsEnabled(false);

  Serial.println("Setup done");
}

void loop() {
  /*
  screen.tick();

  if (BMSSerial.available()) {
    // push to ble characteristic
    pCharacteristicTx->setValue(BMSSerial.readString().c_str());
  }
  */
}

void serialEvent1() {
  while (BMSSerial.available()) {
    char inChar = (char)Serial.read();
    Serial.write(inChar);
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    Serial.write(inChar);
  }
}