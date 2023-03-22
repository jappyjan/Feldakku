#include "main.state.hpp"
#include "const.hpp"
#include "buttons.hpp"
#include "bms.hpp"
#include "my_logger.hpp"

void MainState::enter(){
    loggingStream.println("MainState::enter() - Start");

    this->_bms->fetchState();
    delay(50);
    this->_screen->openMainScreen();

    Buttons::setOnButtonPressedCallback([this](Button button) -> void {
        switch (button) {
            case BUTTON_1:
                // main state row
                this->_stateMachine->toState(AppState::MENU_STATE);
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
                // USB mosfet row
                digitalWrite(MOSFET_USB_PIN, !digitalRead(MOSFET_USB_PIN));
                this->_screen->setUsbIsEnabled(digitalRead(MOSFET_USB_PIN));
                break;
        }
    });

    // TODO: move scheduler to central instance and call scheduler.loop()
    this->_bmsFetchTask = this->_scheduler->every(BMS_TICK_INTERVAL, [this](){
        this->_bms->fetchState();

        auto currentState = this->_bms->getState();

        switch (currentState) {
            case BMSState::BMS_ERROR_CELL_OVERVOLTAGE:
                loggingStream.println("BMSState::BMS_ERROR_CELL_OVERVOLTAGE");
                break;
            case BMSState::BMS_ERROR_CELL_UNDERVOLTAGE:
                loggingStream.println("BMSState::BMS_ERROR_CELL_UNDERVOLTAGE");
                break;
            case BMSState::BMS_ERROR_COMMUNICATION:
                loggingStream.println("BMSState::BMS_ERROR_COMMUNICATION");
                break;
            case BMSState::BMS_ERROR_SHORT_CIRCUIT:
                loggingStream.println("BMSState::BMS_ERROR_SHORT_CIRCUIT");
                break;
        }

        if(currentState != BMSState::BMS_OK) {
            this->_stateMachine->toState(AppState::ERROR_BMS_STATE);
        }
    });
};

void MainState::leave(){
    loggingStream.println("MainState::leave() - Start");

    Buttons::setOnButtonPressedCallback(nullptr);

    if (this->_bmsFetchTask != nullptr) {
        loggingStream.println("MainState::leave() - Removing BMS fetch task");
        this->_bmsFetchTask->cancel();
        this->_scheduler->removeTask(this->_bmsFetchTask);
    }

    loggingStream.println("MainState::leave() - End");
};
