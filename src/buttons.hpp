#pragma once

#include "const.hpp"
#include <functional>
#include <Bounce2.h>

enum Button {
    BUTTON_1 = BTN_1_PIN,
    BUTTON_2 = BTN_2_PIN,
    BUTTON_3 = BTN_3_PIN,
    BUTTON_4 = BTN_4_PIN,
    BUTTON_5 = BTN_5_PIN,
};

// a callback function that has acces to parent context (this) and gets called when a button is pressed with the button as argument
typedef std::function<void(Button)> ButtonCallback;

class Buttons {
    public:
        static void setOnButtonPressedCallback(ButtonCallback callback);

        static void begin();
        static void update();
        static ButtonCallback _onButtonPressedCallback;
        static Bounce2::Button buttons[5];
};
