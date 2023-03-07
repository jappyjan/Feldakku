#include "bms.hpp"
#include "const.hpp"
#include "app-state.hpp"

#include <jbdbms.h>

void BMS::begin() {
    Serial.println("Starting BMS Library");
    this->_bms->begin(-1);
}

bool BMS::fetchState() {
  JbdBms::Status status;
  if (!this->_bms->getStatus(status)) {
    Serial.println("BMS::fetchState() - failed");
    this->_state = BMSState::BMS_ERROR_COMMUNICATION;
    return false;
  }

  Serial.println("BMS::fetchState() - Got BMS status");

  if (this->_bms->isShortCircuit(status.fault)) {
    this->_state = BMSState::BMS_ERROR_SHORT_CIRCUIT;
    return false;
  }

  if (this->_bms->isCellOvervoltage(status.fault)) {
    this->_state = BMSState::BMS_ERROR_CELL_OVERVOLTAGE;
    return false;
  }

  if (this->_bms->isCellUndervoltage(status.fault)) {
    this->_state = BMSState::BMS_ERROR_CELL_UNDERVOLTAGE;
    return false;
  }

  // divide it by int 10 to reduce one decimal place
  // then divide it by float 10 to reduce it to a float of Volts
  int voltageWithOneDecimalPlace = status.voltage / 10;
  float voltage = voltageWithOneDecimalPlace / 10.0;
  this->_screen->setBatteryVoltage(voltage);

  bool hasVoltageWarning = false;
  if (this->_bms->isOvervoltage(status.fault)) {
    hasVoltageWarning = true;
  }
  if (this->_bms->isUndervoltage(status.fault)) {
    hasVoltageWarning = true;
  }
  this->_screen->setBatteryVoltageWarning(hasVoltageWarning);

  float temperature = JbdBms::deciCelsius(status.temperatures[0]) / 10.0;
  this->_screen->setBatteryTemperature(temperature);

  bool hasTemperatureWarning = false;
  if (this->_bms->isChargeOvertemperature(status.fault)) {
    hasTemperatureWarning = true;
  }
  if (this->_bms->isDischargeOvertemperature(status.fault)) {
    hasTemperatureWarning = true;
  }
  this->_screen->setBatteryTemperatureWarning(hasTemperatureWarning);

  float chargeCurrent = 0;
  float dischargeCurrent = 0;
  if (status.current > 0) {
    chargeCurrent = status.current / 100.0;
    dischargeCurrent = 0;
  } else {
    chargeCurrent = 0;
    dischargeCurrent = -status.current / 100.0;
  }

  this->_screen->setChargeCurrent(chargeCurrent);
  this->_screen->setChargeOvercurrentWarning(this->_bms->isChargeOvercurrent(status.fault));

  this->_screen->setDischargeCurrent(dischargeCurrent);
  this->_screen->setDischargeOvercurrentWarning(this->_bms->isDischargeOvercurrent(status.fault));

  this->lastMosfetState = (JbdBms::mosfet)status.mosfetStatus;
  switch(status.mosfetStatus) {
    case JbdBms::mosfet::MOSFET_CHARGE:
      this->_screen->setChargingIsEnabled(true);
      this->_screen->setDischargingIsEnabled(false);
      break;
    case JbdBms::mosfet::MOSFET_DISCHARGE:
      this->_screen->setChargingIsEnabled(false);
      this->_screen->setDischargingIsEnabled(true);
      break;
    case JbdBms::mosfet::MOSFET_BOTH:
      this->_screen->setChargingIsEnabled(true);
      this->_screen->setDischargingIsEnabled(true);
      break;
    case JbdBms::mosfet::MOSFET_NONE:
      this->_screen->setChargingIsEnabled(false);
      this->_screen->setDischargingIsEnabled(false);
      break;
  }

  Serial.println("BMS::fetchState() - Done");
  this->_state = BMSState::BMS_OK;
  return true;
}

void BMS::toggleCharging() {
  Serial.println("BMS::toggleCharging() - Toggling charging");

  bool isEnabled = false;
  JbdBms::mosfet nextMosfetState;
  switch(lastMosfetState) {
    case JbdBms::mosfet::MOSFET_CHARGE:
      nextMosfetState = JbdBms::mosfet::MOSFET_NONE;
      isEnabled = false;
      break;
    case JbdBms::mosfet::MOSFET_DISCHARGE:
      nextMosfetState = JbdBms::mosfet::MOSFET_BOTH;
      isEnabled = true;
      break;
    case JbdBms::mosfet::MOSFET_BOTH:
      nextMosfetState = JbdBms::mosfet::MOSFET_DISCHARGE;
      isEnabled = false;
      break;
    case JbdBms::mosfet::MOSFET_NONE:
      nextMosfetState = JbdBms::mosfet::MOSFET_CHARGE;
      isEnabled = true;
      break;
  }

  if (!this->_bms->setMosfetStatus(nextMosfetState)) {
    Serial.println("BMS::toggleCharging() - failed");
    return;
  }

  this->_screen->setChargingIsEnabled(isEnabled);
  this->lastMosfetState = nextMosfetState;

  Serial.println("BMS::toggleCharging() - Done");
}

void BMS::toggleDischarging() {
  Serial.println("BMS::toggleDischarging() - Toggling discharging");

  bool isEnabled = false;
  JbdBms::mosfet nextMosfetState;

  switch(lastMosfetState) {
    case JbdBms::mosfet::MOSFET_CHARGE:
      nextMosfetState = JbdBms::mosfet::MOSFET_BOTH;
      isEnabled = true;
      break;
    case JbdBms::mosfet::MOSFET_DISCHARGE:
      nextMosfetState = JbdBms::mosfet::MOSFET_NONE;
      isEnabled = false;
      break;
    case JbdBms::mosfet::MOSFET_BOTH:
      nextMosfetState = JbdBms::mosfet::MOSFET_CHARGE;
      isEnabled = false;
      break;
    case JbdBms::mosfet::MOSFET_NONE:
      nextMosfetState = JbdBms::mosfet::MOSFET_DISCHARGE;
      isEnabled = true;
      break;
  }

  if (!this->_bms->setMosfetStatus(nextMosfetState)) {
    Serial.println("BMS::toggleDischarging() - failed");
    return;
  }

  this->_screen->setDischargingIsEnabled(isEnabled);

  Serial.println("BMS::toggleDischarging() - Done");
}

BMSState BMS::getState() {
  return this->_state;
}