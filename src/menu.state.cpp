#include "menu.state.hpp"
#include "const.hpp"

void MenuState::enter(){
    loggingStream.println("MenuState::enter() - Start");

    pinMode(BLE_PIN_ENABLE, OUTPUT);

    this->_screen->openMenu();

    Buttons::setOnButtonPressedCallback([this](Button button) {
        switch(button) {
            case Button::BUTTON_1: {
                // EXIT
                this->_stateMachine->toState(AppState::MAIN_STATE);
                break;
            }
            case Button::BUTTON_2: {
                // BLE
                if (digitalRead(BLE_PIN_ENABLE) == HIGH) {
                    this->bleEnd();
                } else {
                    this->bleBegin();
                }
                break;
            }
            case Button::BUTTON_3: {
                // nothing
                break;
            }
            case Button::BUTTON_4: {  
                // nothing
                break;
            }
            case Button::BUTTON_5: {
                // reboot
                ESP.restart();
                break;
            }
        }
    });

    loggingStream.println("MenuState::enter() - End");
};

void MenuState::bleBegin() {
    loggingStream.println("MenuState::bleBegin() - Start");

    digitalWrite(BLE_PIN_ENABLE, HIGH);
    this->_screen->setBLEIsActive(true);

    this->_serialBridgeTask = this->_scheduler->every(1, [this]() {
        while (this->_bleSerial.available()) {
            this->_bmsSerial.write(this->_bleSerial.read());
        }

        while (this->_bmsSerial.available()) {
            this->_bleSerial.write(this->_bmsSerial.read());
        }
    });

    loggingStream.println("MenuState::bleBegin() - End");
}

void MenuState::bleEnd() {
    loggingStream.println("MenuState::bleEnd() - Start");

    if (this->_serialBridgeTask != nullptr) {
        this->_scheduler->removeTask(this->_serialBridgeTask);
        this->_serialBridgeTask = nullptr;
    }

    digitalWrite(BLE_PIN_ENABLE, LOW);
    this->_screen->setBLEIsActive(false);

    loggingStream.println("MenuState::bleEnd() - End");
}

void MenuState::leave() {
    loggingStream.println("MenuState::leave() - Start");

    if (this->needsReboot) {
        ESP.restart();
    }

    Buttons::setOnButtonPressedCallback(nullptr);

    this->bleEnd();

    loggingStream.println("MenuState::leave() - End");
};
