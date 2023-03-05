#include "bms.hpp"
#include "const.hpp"

#include <jbdbms.h>

HardwareSerial &BMSSerial = Serial1;

BMS::BMS(Screen screen) {}

void BMS::begin(int8_t rxPin, int8_t txPin) {
    Serial.println("Starting BMS Serial");
    BMSSerial.begin(9600, SERIAL_8N1, rxPin, txPin);
    Serial.println("Starting BMS Library");
    this->bms = new JbdBms(BMSSerial);
    bms->begin(-1);
    this->lastBmsTick = 0;
}

void BMS::tick() {
  if (millis() - lastBmsTick < BMS_TICK_INTERVAL) {
    return;
  }

  lastBmsTick = millis();
  JbdBms::Status status;
  if (!bms->getStatus(status)) {
    Serial.println("Failed to get BMS status");
    screen.setBMSError(BMSError::COMMUNICATION_ERROR);
    return;
  }

  screen.setBMSError(BMSError::NO_ERROR);

  Serial.println("Got BMS status");
  Serial.print(status.voltage);
  Serial.println("mV");

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

  Serial.println("Finished BMS tick");
}

void BMS::toggleCharging() {
  JbdBms::Status status;
  if (!bms->getStatus(status)) {
    return;
  }

  switch(status.mosfetStatus) {
    case JbdBms::mosfet::MOSFET_CHARGE:
      bms->setMosfetStatus(JbdBms::mosfet::MOSFET_NONE);
      screen.setChargingIsEnabled(false);
      break;
    case JbdBms::mosfet::MOSFET_DISCHARGE:
      bms->setMosfetStatus(JbdBms::mosfet::MOSFET_BOTH);
      screen.setDischargingIsEnabled(true);
      break;
    case JbdBms::mosfet::MOSFET_BOTH:
      bms->setMosfetStatus(JbdBms::mosfet::MOSFET_DISCHARGE);
      screen.setDischargingIsEnabled(false);
      break;
    case JbdBms::mosfet::MOSFET_NONE:
      bms->setMosfetStatus(JbdBms::mosfet::MOSFET_CHARGE);
      screen.setChargingIsEnabled(true);
      break;
  }
}

void BMS::toggleDischarging() {
  JbdBms::Status status;
  if (!bms->getStatus(status)) {
    return;
  }

  switch(status.mosfetStatus) {
    case JbdBms::mosfet::MOSFET_CHARGE:
      bms->setMosfetStatus(JbdBms::mosfet::MOSFET_BOTH);
      screen.setChargingIsEnabled(true);
      break;
    case JbdBms::mosfet::MOSFET_DISCHARGE:
      bms->setMosfetStatus(JbdBms::mosfet::MOSFET_NONE);
      screen.setDischargingIsEnabled(false);
      break;
    case JbdBms::mosfet::MOSFET_BOTH:
      bms->setMosfetStatus(JbdBms::mosfet::MOSFET_CHARGE);
      screen.setChargingIsEnabled(false);
      break;
    case JbdBms::mosfet::MOSFET_NONE:
      bms->setMosfetStatus(JbdBms::mosfet::MOSFET_DISCHARGE);
      screen.setDischargingIsEnabled(true);
      break;
  }
}