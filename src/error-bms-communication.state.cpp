#include "error-bms-communication.state.hpp"
#include "const.hpp"

void ErrorBmsCommunicationState::enter(){
    Serial.println("ErrorBmsCommunicationState::enter() - Start");

    this->_screen->openPopup("BMS", "Communication", "Error", GxEPD_WHITE, GxEPD_RED);

    Serial.println("ErrorBmsCommunicationState::enter() - Adding BMS fetch task");
    this->_bmsFetchTask = this->_scheduler->every(BMS_TICK_INTERVAL, [this](){
        Serial.println("ErrorBmsCommunicationState::enter() - Inside BMS fetch task");
        bool success = this->_bms->fetchState();
        if (success) {
            Serial.println("ErrorBmsCommunicationState::enter() - BMS fetch task - Success (going to main state)");
            this->_stateMachine->toState(MAIN_STATE);
        }
    });

    Serial.println("ErrorBmsCommunicationState::enter() - End");
};

void ErrorBmsCommunicationState::leave() {
    Serial.println("ErrorBmsCommunicationState::leave() - Start");

    if (this->_bmsFetchTask != nullptr) {
        Serial.println("ErrorBmsCommunicationState::leave() - Removing BMS fetch task");
        this->_bmsFetchTask->cancel();
        this->_scheduler->removeTask(this->_bmsFetchTask);
    } else {
        Serial.println("ErrorBmsCommunicationState::leave() - BMS fetch task is null");
    }
};
