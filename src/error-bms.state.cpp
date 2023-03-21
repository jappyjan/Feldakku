#include "error-bms.state.hpp"
#include "const.hpp"
#include "buttons.hpp"

void ErrorBmsState::enter(){
    Serial.println("ErrorBmsState::enter() - Start");

    this->_screen->openBMSErrorPopup(this->_bms->getState());

    Buttons::setOnButtonPressedCallback([this](Button button) -> void {
        if (button != BUTTON_1) {
            return;
        }

        // reboot
        ESP.restart();
    });

    this->_bmsFetchTask = this->_scheduler->every(BMS_TICK_INTERVAL, [this](){
        this->_bms->fetchState();

        BMSState currentState = this->_bms->getState();
        if (currentState == this->lastBmsState) {
            return;
        }

        this->lastBmsState = this->_bms->getState();

        switch (currentState) {
            case BMSState::BMS_OK:
                this->_stateMachine->toState(AppState::MAIN_STATE);
                break;
            case BMSState::BMS_ERROR_COMMUNICATION:
            case BMSState::BMS_ERROR_SHORT_CIRCUIT:
            case BMSState::BMS_ERROR_CELL_OVERVOLTAGE:
            case BMSState::BMS_ERROR_CELL_UNDERVOLTAGE:
                this->_screen->openBMSErrorPopup(currentState);
                break;
        }
    });

    Serial.println("ErrorBmsState::enter() - End");
};

void ErrorBmsState::leave() {
    Serial.println("ErrorBmsState::leave() - Start");

    Buttons::setOnButtonPressedCallback(nullptr);

    if (this->_bmsFetchTask != nullptr) {
        this->_bmsFetchTask->cancel();
        this->_scheduler->removeTask(this->_bmsFetchTask);
    }

    Serial.println("ErrorBmsState::leave() - End");
};
