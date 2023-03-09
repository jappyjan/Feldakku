#pragma once

#include "basic.state.hpp"
#include <bluefairy.h>
#include "bms.hpp"

class BLEState: public BasicState {
    public:
        BLEState(
            BMS *bms,
            Screen *screen,
            AppStateMachine *stateMachine,
            bluefairy::Scheduler *scheduler,
            Stream &bleSerial,
            Stream &bmsSerial
        ): BasicState(
            bms,
            screen,
            stateMachine,
            scheduler
        ), _bleSerial(bleSerial), _bmsSerial(bmsSerial) {};

        void enter();
        void leave();

    private:
        bluefairy::TaskNode *_serialBridgeTask;
        Stream &_bleSerial;
        Stream &_bmsSerial;
};
