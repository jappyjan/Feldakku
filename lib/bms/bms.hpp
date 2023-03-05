#pragma once

#include <Arduino.h>
#include <jbdbms.h>

#include "screen.hpp"

#define BMS_TICK_INTERVAL 10000

class BMS {
    public:
        BMS(Screen screen);
        void begin(int8_t rxPin, int8_t txPin);
        void tick();
        void toggleCharging();
        void toggleDischarging();
    
    private:
        JbdBms *bms;
        Screen screen;
        unsigned long lastBmsTick = millis() - (BMS_TICK_INTERVAL / 2);
};
