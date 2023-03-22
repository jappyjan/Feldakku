#include "buttons.hpp"
#include "const.hpp"
#include <Arduino.h>
#include <Bounce2.h>
#include "my_logger.hpp"

inline Button getButtonPinByIndex(uint8_t index) {
    switch (index) {
        case 0:
            return Button::BUTTON_1;
        case 1:
            return Button::BUTTON_2;
        case 2:
            return Button::BUTTON_3;
        case 3:
            return Button::BUTTON_4;
        case 4:
            return Button::BUTTON_5;
        default:
            // throw error
            throw "Invalid button index";
    }
}

inline uint8_t getButtonIndexByPin(Button button) {
    switch (button) {
        case Button::BUTTON_1:
            return 0;
        case Button::BUTTON_2:
            return 1;
        case Button::BUTTON_3:
            return 2;
        case Button::BUTTON_4:
            return 3;
        case Button::BUTTON_5:
            return 4;
    }
}

Bounce2::Button Buttons::buttons[5] = {
    Bounce2::Button(),
    Bounce2::Button(),
    Bounce2::Button(),
    Bounce2::Button(),
    Bounce2::Button()
};

inline int getButtonCount() {
    return sizeof(Buttons::buttons) / sizeof(Buttons::buttons[0]);
}

void Buttons::begin() {
    loggingStream.print("Initializing ");
    loggingStream.print(getButtonCount());
    loggingStream.println(" buttons");

    for (int i = 0; i < getButtonCount(); i++) {
        pinMode(getButtonPinByIndex(i), INPUT_PULLUP);
        loggingStream.print("Button ");
        loggingStream.print(i);
        loggingStream.print(" on pin ");
        loggingStream.print(getButtonPinByIndex(i));
        loggingStream.print(" with debounce time of ");
        loggingStream.print(BTN_DEBOUNCE_TIME);
        loggingStream.println("ms initialized");

        buttons[i].attach(getButtonPinByIndex(i));
        buttons[i].interval(BTN_DEBOUNCE_TIME);
        buttons[i].setPressedState(LOW);

        attachInterrupt(digitalPinToInterrupt(getButtonPinByIndex(i)), []() {
            Buttons::update();
        }, FALLING);
    }
}

ButtonCallback Buttons::_onButtonPressedCallback = nullptr;

void Buttons::setOnButtonPressedCallback(ButtonCallback callback) {
    Buttons::_onButtonPressedCallback = callback;
}

ICACHE_RAM_ATTR void Buttons::update() {
    if (Buttons::_onButtonPressedCallback == nullptr) {
        return;
    }

    for (int i = 0; i < getButtonCount(); i++) {
        Buttons::buttons[i].update();

        if (Buttons::buttons[i].pressed()) {
            loggingStream.print("Button pressed: ");
            loggingStream.println(getButtonPinByIndex(i));
            Buttons::_onButtonPressedCallback(getButtonPinByIndex(i));
        }
    }
}