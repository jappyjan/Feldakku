#pragma once

#include "basic.state.hpp"
#include <bluefairy.h>
#include "bms.hpp"
#include "my_logger.hpp"
#include "buttons.hpp"

class MenuState: public BasicState {
    public:
        MenuState(
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
        bool needsReboot = false;

        void bleBegin();
        void bleEnd();
};
