#pragma once

enum AppState {
    MAIN_STATE = 0,
    ERROR_BMS_STATE = 1,
    MENU_STATE = 2,
};

typedef bluefairy::StateMachine<3> AppStateMachine;