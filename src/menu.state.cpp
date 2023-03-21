#include "menu.state.hpp"
#include "buttons.hpp"
#include "ota_connection.hpp"

OTA_CONNECTION ota_connection;

void MenuState::enter(){
    Serial.println("MenuState::enter() - Start");

    pinMode(BLE_PIN_ENABLE, OUTPUT);

    this->_screen->openMenu();

    Buttons::setOnButtonPressedCallback([this](Button button) {
        switch(button) {
            case Button::BUTTON_1:
                // EXIT
                this->_stateMachine->toState(AppState::MAIN_STATE);
                break;
            case Button::BUTTON_2:
                // BLE
                if (digitalRead(BLE_PIN_ENABLE) == HIGH) {
                    this->bleEnd();
                } else {
                    this->bleBegin();
                }
                break;
            case Button::BUTTON_3:
                // OTA
                if (ota_connection.getIsActive()) {
                    this->_screen->setOTAIsActive(false);
                    ota_connection.end();
                } else {
                    this->_screen->setOTAIsActive(true);
                    ota_connection.begin();
                }
                break;
            case Button::BUTTON_4:
                // NOTHING
                break;
            case Button::BUTTON_5:
                // reboot
                ESP.restart();
                break;
        }
    });

    Serial.println("MenuState::enter() - End");
};

void MenuState::leave() {
    Serial.println("MenuState::leave() - Start");

    Buttons::setOnButtonPressedCallback(nullptr);

    this->_screen->setOTAIsActive(false);
    ota_connection.end();

    this->bleEnd();

    Serial.println("MenuState::leave() - End");
};


void MenuState::bleBegin() {
    Serial.println("MenuState::bleBegin() - Start");

    this->_screen->setBLEIsActive(true);

    // just to make sure, previous Serial communication is stopped
    this->_bmsSerial.flush();
    this->_bleSerial.flush();
    delay(1000);

    digitalWrite(BLE_PIN_ENABLE, HIGH);

    this->_serialBridgeTask = this->_scheduler->every(BLE_BRIDGE_POLL_INTERVAL, [this](){
        while (this->_bleSerial.available()) {
            this->_bmsSerial.write(this->_bleSerial.read());
        }
        while(this->_bmsSerial.available()) {
            this->_bleSerial.write(this->_bmsSerial.read());
        }
    });

    Serial.println("MenuState::bleBegin() - End");
}

void MenuState::bleEnd() {
    Serial.println("MenuState::bleEnd() - Start");

    this->_screen->setBLEIsActive(false);

    digitalWrite(BLE_PIN_ENABLE, LOW);

    if (this->_serialBridgeTask != nullptr) {
        this->_serialBridgeTask->cancel();
        this->_scheduler->removeTask(this->_serialBridgeTask);
    }

    delay(500);
    
    // just to make sure, previous Serial communication is stopped
    this->_bmsSerial.flush();
    this->_bleSerial.flush();
    delay(1000);

    Serial.println("MenuState::bleEnd() - End");
}