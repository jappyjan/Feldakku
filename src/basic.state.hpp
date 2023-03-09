#pragma once

#include <bluefairy.h>
#include "app-state.hpp"
#include "bms.hpp"
#include "screen.hpp"

class BasicState: public bluefairy::State {
    public:
        BasicState(
            BMS *bms,
            Screen *screen,
            AppStateMachine *stateMachine,
            bluefairy::Scheduler *scheduler
        ) {
            this->_bms = bms;
            this->_screen = screen;
            this->_stateMachine = stateMachine;
            this->_scheduler = scheduler;
        };

    protected:
        BMS *_bms;
        Screen *_screen;
        AppStateMachine *_stateMachine;
        bluefairy::Scheduler *_scheduler;
};
