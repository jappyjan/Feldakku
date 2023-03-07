/*

#include <Arduino.h>
#include <BLEServer.h>
#include <BLEDevice.h>
#include <BLEUtils.h>

static BLEUUID serviceUUID("0000ff00-0000-1000-8000-00805f9b34fb"); //xiaoxiang bms original module
static BLEUUID charUUID_rx("0000ff02-0000-1000-8000-00805f9b34fb"); //xiaoxiang bms original module
static BLEUUID charUUID_tx("0000ff01-0000-1000-8000-00805f9b34fb"); //xiaoxiang bms original module

// define ble callbacks
class MyBleCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Serial.println("BLE -> ESP");

    uint8_t *rxValue = pCharacteristic->getData();
    // store length of the rxValue
    int rxLength = pCharacteristic->getValue().length();
    
    Serial.print("BLE rxValue: ");
    Serial.write(rxValue, rxLength);
    Serial.println();

    Serial.println("ESP -> BMS");
    BMSSerial.write(rxValue, rxLength);
  }
};

MyBleCallbacks bleCb = MyBleCallbacks();

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharacteristicTx;
BLECharacteristic *pCharacteristicRx;
BLEAdvertising *pAdvertising;

// define serial callback serial_cb_t
void bmsSerialCallback(const uint8_t *data, const size_t length) {
  Serial.println("BMS -> ESP");
  Serial.print("Serial rxValue: ");
  Serial.write(data, length);
  Serial.println();
  // pCharacteristicTx->setValue(const_cast<uint8_t*>(data), length);
  // pCharacteristicTx->notify();
}

void setupNot() {
  Serial.println("Starting BLE");
  BLEDevice::init("ESP-Feldakku");

  Serial.println("Creating BLE Server");
  pServer = BLEDevice::createServer();

  Serial.println("Creating BLE Service");
  pService = pServer->createService(serviceUUID);

  Serial.println("Creating BLE Characteristics");
  pCharacteristicTx = pService->createCharacteristic(
                                        charUUID_tx,
                                        BLECharacteristic::PROPERTY_NOTIFY
                                      );
  pCharacteristicRx = pService->createCharacteristic(
                                        charUUID_rx,
                                        BLECharacteristic::PROPERTY_WRITE
                                      );

  pCharacteristicTx->setValue("Hello World");

  Serial.println("Starting BLE Service");
  pService->start();

  Serial.println("Starting BLE Advertising");
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(serviceUUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);

  BLEDevice::startAdvertising();
  Serial.println("Registering BLE Callbacks");
  // pCharacteristicRx->setCallbacks(&bleCb);

  // Serial.println("Registering BMS Serial Callbacks");
  // bms.setSerialCb(bmsSerialCallback);
}

*/