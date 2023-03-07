#include "main.state.hpp"
#include "const.hpp"
#include "buttons.hpp"

void MainState::enter(){
    Serial.println("MainState::enter() - Start");

    this->_screen->openMainScreen();

    Buttons::setOnButtonPressedCallback([this](Button button) -> void {
        switch (button) {
            case BUTTON_1:
                // main state row
                // nothing to do
                break;
            case BUTTON_2:
                // charging state row
                this->_bms->toggleCharging();
                break;
            case BUTTON_3:
                // discharging state row
                this->_bms->toggleDischarging();
                break;
            case BUTTON_4:
                // 4S mosfet row
                digitalWrite(MOSFET_4S_PIN, !digitalRead(MOSFET_4S_PIN));
                this->_screen->setFourSOutputIsEnabled(digitalRead(MOSFET_4S_PIN));
                break;
            case BUTTON_5:
                // 6S mosfet row
                digitalWrite(MOSFET_6S_PIN, !digitalRead(MOSFET_6S_PIN));
                this->_screen->setSixSIsEnabled(digitalRead(MOSFET_6S_PIN));
                break;
        }
    });

    // TODO: move scheduler to central instance and call scheduler.loop()
    this->_bmsFetchTask = this->_scheduler->every(BMS_TICK_INTERVAL, [this](){
        bool ret = this->_bms->fetchState();
        if (!ret) {
            Serial.println("MainState::enter() - BMS fetch task - Error (going to error state)");
            this->_stateMachine->toState(ERROR_BMS_COMMUNICATION_STATE);
            return;
        }
    });
};

void MainState::leave(){
    Serial.println("MainState::leave() - Start");

    Buttons::setOnButtonPressedCallback(nullptr);

    if (this->_bmsFetchTask != nullptr) {
        Serial.println("MainState::leave() - Removing BMS fetch task");
        this->_bmsFetchTask->cancel();
        this->_scheduler->removeTask(this->_bmsFetchTask);
    }

    Serial.println("MainState::leave() - End");
};
