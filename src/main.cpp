#include <arduino.h>
#include <HardwareSerial.h>
#include <jbdbms.h>
#include <BLEServer.h>
#include <BLEDevice.h>
#include <BLEUtils.h>

#include "screen.hpp"

HardwareSerial &BMSSerial = Serial1;
Screen screen;
JbdBms bms(BMSSerial);

static BLEUUID serviceUUID("0000ff00-0000-1000-8000-00805f9b34fb"); //xiaoxiang bms original module
static BLEUUID charUUID_rx("0000ff02-0000-1000-8000-00805f9b34fb"); //xiaoxiang bms original module
static BLEUUID charUUID_tx("0000ff01-0000-1000-8000-00805f9b34fb"); //xiaoxiang bms original module

// define ble callbacks
class MyBleCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    uint8_t *rxValue = pCharacteristic->getData();
    // store length of the rxValue
    int rxLength = pCharacteristic->getValue().length();
    
    Serial.print("BLE rxValue: ");
    Serial.write(rxValue, rxLength);
    Serial.println();
    BMSSerial.write(rxValue, rxLength);
  }
};

auto bleCb = MyBleCallbacks();

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharacteristicTx;
BLECharacteristic *pCharacteristicRx;
BLEAdvertising *pAdvertising;

// define serial callback serial_cb_t
void serial_cb(const uint8_t *data, const size_t length) {
  Serial.print("Serial rxValue: ");
  Serial.write(data, length);
  Serial.println();
  pCharacteristicTx->setValue(const_cast<uint8_t*>(data), length);
  pCharacteristicTx->notify();
}

void setup() {
  Serial.begin(9600);
  BMSSerial.begin(9600, SERIAL_8N1, 25, 26);
  bms.begin(-1);

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

  bms.setSerialCb(serial_cb);

  screen.begin();
  screen.setSixSIsEnabled(true);
  screen.setFourSOutputIsEnabled(false);

  Serial.println("Setup done");
}

unsigned long lastBmsTick = 0;
inline void bmsTick() {
  if (millis() - lastBmsTick < 10000) {
    return;
  }

  lastBmsTick = millis();
  JbdBms::Status status;
  if (!bms.getStatus(status)) {
    return;
  }

  screen.setBatteryVoltage(status.voltage / 100.0);
  screen.setBatteryVoltageWarning(status.currentCapacity < 20);

  float temperature = JbdBms::deciCelsius(status.temperatures[0]) / 10.0;
  screen.setBatteryTemperature(temperature);
  screen.setBatteryTemperatureWarning(temperature > 40 || temperature < 5);

  float chargeCurrent = 0;
  float dischargeCurrent = 0;
  if (status.current > 0) {
    chargeCurrent = status.current / 100.0;
    dischargeCurrent = 0;
  } else {
    chargeCurrent = 0;
    dischargeCurrent = -status.current / 100.0;
  }

  screen.setChargeCurrent(chargeCurrent);
  screen.setDischargeCurrent(dischargeCurrent);

  switch(status.mosfetStatus) {
    case JbdBms::mosfet::MOSFET_CHARGE:
      screen.setChargingIsEnabled(true);
      screen.setDischargingIsEnabled(false);
      break;
    case JbdBms::mosfet::MOSFET_DISCHARGE:
      screen.setChargingIsEnabled(false);
      screen.setDischargingIsEnabled(true);
      break;
    case JbdBms::mosfet::MOSFET_BOTH:
      screen.setChargingIsEnabled(true);
      screen.setDischargingIsEnabled(true);
      break;
    case JbdBms::mosfet::MOSFET_NONE:
      screen.setChargingIsEnabled(false);
      screen.setDischargingIsEnabled(false);
      break;
  }
}

void loop() {
  bmsTick();
  screen.tick();
}