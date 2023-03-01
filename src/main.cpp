#include "screen.hpp"

Screen screen;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");
  
  screen.begin();
  screen.setBatteryVoltage(6*4.12);
  screen.setBatteryTemperature(25.0);
  screen.setChargingIsEnabled(true);
  screen.setChargeCurrent(11.0);
  screen.setDischargingIsEnabled(false);
  screen.setDischargeCurrent(0.0);
  screen.setSixSIsEnabled(true);
  screen.setFourSOutputIsEnabled(false);

  Serial.println("Done");
}

void loop() {
  screen.tick();
}