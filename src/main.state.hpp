#pragma once

#include "basic.state.hpp"
#include <bluefairy.h>

class MainState: public BasicState {
    public:
        MainState(
            BMS *bms,
            Screen *screen,
            bluefairy::StateMachine<2> *stateMachine,
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
};