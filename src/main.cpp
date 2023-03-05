#include <arduino.h>
#include <HardwareSerial.h>

#include "screen.hpp"
#include "bms.hpp"
#include "const.hpp"

Screen screen;
BMS bms(screen);

void setup() {
  Serial.begin(115200);

  Serial.println("Starting Screen");
  screen.begin();

  Serial.println("Starting BMS Communication");
  bms.begin(BMS_SERIAL_RX_PIN, BMS_SERIAL_TX_PIN);

  pinMode(BTN_1_PIN, INPUT_PULLUP);
  pinMode(BTN_2_PIN, INPUT_PULLUP);
  pinMode(BTN_3_PIN, INPUT_PULLUP);
  pinMode(BTN_4_PIN, INPUT_PULLUP);
  pinMode(BTN_5_PIN, INPUT_PULLUP);

  pinMode(MOSFET_4S_PIN, OUTPUT);
  pinMode(MOSFET_6S_PIN, OUTPUT);

  attachInterrupt(BTN_1_PIN, []() {
    // main state row -> nothing
  }, FALLING);

  attachInterrupt(BTN_2_PIN, []() {
    // charging state row
    bms.toggleCharging();
  }, FALLING);

  attachInterrupt(BTN_3_PIN, []() {
    // discharging state row
    bms.toggleDischarging();
  }, FALLING);

  attachInterrupt(BTN_4_PIN, []() {
    // 4S mosfet row
    digitalWrite(MOSFET_4S_PIN, !digitalRead(MOSFET_4S_PIN));
    screen.setFourSOutputIsEnabled(digitalRead(MOSFET_4S_PIN));
  }, FALLING);

  attachInterrupt(BTN_5_PIN, []() {
    // 6S mosfet row
    digitalWrite(MOSFET_6S_PIN, !digitalRead(MOSFET_6S_PIN));
    screen.setSixSIsEnabled(digitalRead(MOSFET_6S_PIN));
  }, FALLING);

  Serial.println("Setup done");
}

void loop() {
  bms.tick();
  screen.tick();

  delay(1000);
}