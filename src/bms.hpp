#pragma once

#include <Arduino.h>
#include <jbdbms.h>
#include <bluefairy.h>

#include "screen.hpp"

#define BMS_TICK_INTERVAL 10000

class BMS {
    public:
        BMS(JbdBms *bms, Screen *screen) {
            this->_screen = screen;
            this->_bms = bms;
        }
        void begin();
        void toggleCharging();
        void toggleDischarging();
        bool fetchState();
    
    private:
        Screen *_screen;
        JbdBms *_bms;
        JbdBms::mosfet lastMosfetState;
};
