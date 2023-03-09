#include "ble.state.hpp"
#include "const.hpp"
#include "buttons.hpp"

void BLEState::enter(){
    Serial.println("BleState::enter() - Start");

    this->_screen->openBLEPopup();

    pinMode(BLE_PIN_ENABLE, OUTPUT);
    digitalWrite(BLE_PIN_ENABLE, HIGH);

    Buttons::setOnButtonPressedCallback([this](Button button) -> void {
        if (button != BUTTON_1) {
            return;
        }

        this->_stateMachine->toState(AppState::MAIN_STATE);
    });

    this->_serialBridgeTask = this->_scheduler->every(BLE_BRIDGE_POLL_INTERVAL, [this](){
        while (this->_bleSerial.available()) {
            this->_bmsSerial.write(this->_bleSerial.read());
        }
        while(this->_bmsSerial.available()) {
            this->_bleSerial.write(this->_bmsSerial.read());
        }
    });

    Serial.println("BLEState::enter() - End");
};

void BLEState::leave() {
    Serial.println("BleState::leave() - Start");

    digitalWrite(BLE_PIN_ENABLE, LOW);

    if (this->_serialBridgeTask != nullptr) {
        this->_serialBridgeTask->cancel();
        this->_scheduler->removeTask(this->_serialBridgeTask);
    }

    Serial.println("BleState::leave() - End");
};
