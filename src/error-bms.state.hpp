#pragma once

#include "basic.state.hpp"
#include <bluefairy.h>
#include "bms.hpp"

class ErrorBmsState: public BasicState {
    public:
        ErrorBmsState(
            BMS *bms,
            Screen *screen,
            AppStateMachine *stateMachine,
            bluefairy::Scheduler *scheduler
        ): BasicState(
            bms,
            screen,
            stateMachine,
            scheduler
        ) {};

        void enter();
        void leave();

    private:
        bluefairy::TaskNode *_bmsFetchTask;
        BMSState lastBmsState;
};
