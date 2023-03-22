#include <pgmspace.h>

#include <SPI.h>               // Built-in 
#define  ENABLE_GxEPD2_display 0
#include <GxEPD2_BW.h>         // GxEPD2 from Sketch, Include Library, Manage Libraries, search for GxEDP2
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include "epaper_fonts.h"

#include "screen.hpp"
#include "bms.hpp"
#include "my_logger.hpp"

// ----------------------------------- Drawing functions -----------------------------------

void Screen::drawString(
    int x,
    int y,
    String text,
    HorizontalAlignment horizontalAlignment,
    VerticalAlignment verticalAlignment,
    uint16_t color
) {
    int16_t  x1, y1;
    uint16_t w, h;
    this->display->getTextBounds(text, x, y, &x1, &y1, &w, &h);
    if (horizontalAlignment == HRIGHT)  x = x - w;
    if (horizontalAlignment == HCENTER) x = x - w / 2;

    if (verticalAlignment == VBOTTOM)  y = y - h;
    if (verticalAlignment == VCENTER) y = y - h / 2;

    this->display->setCursor(x, y + h);
    this->display->setTextColor(color);
    this->display->print(text);
}

void Screen::drawRow(
    uint8_t row,
    String label,
    String valueRowOne,
    String valueRowTwo,
    uint16_t bgRowOne,
    uint16_t bgRowTwo
) {
    this->drawMenuBox(row, 0, label);
    this->drawMenuBox(row, 1, valueRowOne, valueRowTwo, bgRowOne, bgRowTwo);

    if (row == 0) {
        return;
    }

    // Draw the horizontal line between the two rows
    this->display->drawLine(
        0,
        row * SCREEN_ROW_HEIGHT,
        SCREEN_WIDTH / 2,
        row * SCREEN_ROW_HEIGHT,
        bgRowOne == GxEPD_BLACK ? GxEPD_WHITE : GxEPD_BLACK
    );

    this->display->drawLine(
        SCREEN_WIDTH / 2,
        row * SCREEN_ROW_HEIGHT,
        SCREEN_WIDTH / 2,
        row * SCREEN_ROW_HEIGHT,
        bgRowTwo == GxEPD_BLACK ? GxEPD_WHITE : GxEPD_BLACK
    );

    // Draw the vertical line between the two columns
    this->display->drawLine(
        SCREEN_COLUMN_WIDTH,
        0,
        SCREEN_COLUMN_WIDTH,
        SCREEN_HEIGHT / 2,
        bgRowOne == GxEPD_BLACK ? GxEPD_WHITE : GxEPD_BLACK
    );

    this->display->drawLine(
        SCREEN_COLUMN_WIDTH,
        SCREEN_HEIGHT / 2,
        SCREEN_COLUMN_WIDTH,
        SCREEN_HEIGHT,
        bgRowTwo == GxEPD_BLACK ? GxEPD_WHITE : GxEPD_BLACK
    );
}

void Screen::drawMenuBox(
    uint8_t row,
    uint8_t collumn,
    String textRowOne,
    String textRowTwo,
    uint16_t bgRowOne,
    uint16_t bgRowTwo
) {
    uint8_t x = collumn * SCREEN_COLUMN_WIDTH;
    uint8_t y = row * SCREEN_ROW_HEIGHT;

    if (textRowTwo.length() == 0) {
        this->display->fillRect(x + 1, y + 1, SCREEN_COLUMN_WIDTH - 2, SCREEN_ROW_HEIGHT - 2, bgRowOne);
        this->drawString(
            x + (SCREEN_COLUMN_WIDTH / 2),
            y + (SCREEN_ROW_HEIGHT / 2),
            textRowOne,
            HCENTER,
            VCENTER,
            bgRowOne == GxEPD_WHITE ? GxEPD_BLACK : GxEPD_WHITE
        );
        return;
    }

    this->display->fillRect(
        x + 1,
        y + 1,
        SCREEN_COLUMN_WIDTH - 2,
        SCREEN_ROW_HEIGHT / 2 - 1,
        bgRowOne
    );
    this->display->fillRect(
        x + 1, 
        SCREEN_ROW_HEIGHT / 2 + y, 
        SCREEN_COLUMN_WIDTH - 2, 
        SCREEN_ROW_HEIGHT / 2 - 1, 
        bgRowTwo
    );

    this->drawString(
        x + (SCREEN_COLUMN_WIDTH / 2),
        y + (SCREEN_ROW_HEIGHT / 3),
        textRowOne,
        HCENTER,
        VCENTER,
        bgRowOne == GxEPD_WHITE ? GxEPD_BLACK : GxEPD_WHITE
    );

    this->drawString(
        x + (SCREEN_COLUMN_WIDTH / 2),
        y + SCREEN_ROW_HEIGHT - (SCREEN_ROW_HEIGHT / 3),
        textRowTwo,
        HCENTER,
        VCENTER,
        bgRowTwo == GxEPD_WHITE ? GxEPD_BLACK : GxEPD_WHITE
    );
}

void Screen::drawMainRow() {
    this->drawRow(
        0,
        "State",
        String(this->batteryVoltage, 1) + "V",
        String(this->batteryPercentage) + "%",
        this->batteryVoltageWarning ? GxEPD_RED : GxEPD_WHITE,
        this->batteryTemperatureWarning ? GxEPD_RED : GxEPD_WHITE
    );
}

void Screen::drawChargeRow() {
    String state = "";
    if (this->chargingIsEnabled) {
        state.concat("ON");
    } else {
        state.concat("OFF");
    }

    uint16_t bgColor = GxEPD_WHITE;
    if (this->chargeOvercurrentWarning) {
        bgColor = GxEPD_RED;
    } else if (this->chargingIsEnabled) {
        bgColor = GxEPD_BLACK;
    }

    this->drawRow(
        1,
        "IN",
        state,
        String(this->chargeCurrent, 1) + "A",
        bgColor,
        bgColor
    );
}

void Screen::drawDischargeRow() {
    String state = "";
    if (this->dischargingIsEnabled) {
        state.concat("ON");
    } else {
        state.concat("OFF");
    }

    uint16_t bgColor = GxEPD_WHITE;
    if (this->dischargeOvercurrentWarning) {
        bgColor = GxEPD_RED;
    } else if (this->dischargingIsEnabled) {
        bgColor = GxEPD_BLACK;
    }

    this->drawRow(
        2,
        "OUT",
        state,
        String(this->dischargeCurrent, 1) + "A",
        bgColor,
        bgColor
    );
}

void Screen::drawUsbRow() {
    String state = "";
    if (this->usbIsEnabled) {
        state.concat("ON");
    } else {
        state.concat("OFF");
    }

    this->drawRow(
        4,
        "USB",
        state,
        "",
        this->usbIsEnabled ? GxEPD_BLACK : GxEPD_WHITE
    );
}

void Screen::drawFourSRow() {
    String state = "";
    if (this->fourSOutputIsEnabled) {
        state.concat("ON");
    } else {
        state.concat("OFF");
    }

    this->drawRow(
        3,
        "4S",
        state,
        "",
        this->fourSOutputIsEnabled ? GxEPD_BLACK : GxEPD_WHITE
    );
}

// ------------------------------------ Screen functions -----------------------------------

void Screen::drawMainScreen() {
    loggingStream.println("Drawing main screen");

    this->display->fillScreen(GxEPD_BLACK);
    this->drawMainRow();
    this->drawChargeRow();
    this->drawDischargeRow();
    this->drawUsbRow();
    this->drawFourSRow();
}

void Screen::openMainScreen() {
    this->screenLayout = ScreenLayout::MAIN_SCREEN;
    loggingStream.println("Screen::openMainScreen()");
    this->needsRedraw = true;
}

void Screen::drawPopup() {
    this->display->fillScreen(this->popupBackgroundColour);

    if (this->popupClosable) {
        this->drawRow(0, "", "CLOSE");
    }

    // three rows of text, no space between, in the center of the remaining screen by determining the height of the text using display->getTextBounds()
    int16_t x1, y1;
    uint16_t w, h;
    this->display->getTextBounds(this->popupLineOne, 0, 0, &x1, &y1, &w, &h);
    this->drawString(
        SCREEN_WIDTH / 2,
        SCREEN_HEIGHT / 2 - h * 2,
        this->popupLineOne,
        HorizontalAlignment::HCENTER,
        VerticalAlignment::VCENTER,
        this->popupTextColour
    );

    this->display->getTextBounds(this->popupLineTwo, 0, 0, &x1, &y1, &w, &h);
    this->drawString(
        SCREEN_WIDTH / 2,
        SCREEN_HEIGHT / 2,
        this->popupLineTwo,
        HorizontalAlignment::HCENTER,
        VerticalAlignment::VCENTER,
        this->popupTextColour
    );

    this->display->getTextBounds(this->popupLineThree, 0, 0, &x1, &y1, &w, &h);
    this->drawString(
        SCREEN_WIDTH / 2,
        SCREEN_HEIGHT / 2 + h * 2,
        this->popupLineThree,
        HorizontalAlignment::HCENTER,
        VerticalAlignment::VCENTER,
        this->popupTextColour
    );
}

void Screen::openBMSErrorPopup(BMSState bmsState) {
    this->popupLineOne = "BMS";
    
    switch (bmsState) {
        case BMSState::BMS_OK:
            this->popupLineTwo = "OK";
            break;
        case BMSState::BMS_ERROR_COMMUNICATION:
            this->popupLineTwo = "COMMUNICATION";
            break;
        case BMSState::BMS_ERROR_SHORT_CIRCUIT:
            this->popupLineTwo = "SHORT CIRCUIT";
            break;
        case BMSState::BMS_ERROR_CELL_OVERVOLTAGE:
            this->popupLineTwo = "CELL OVERVOLTAGE";
            break;
        case BMSState::BMS_ERROR_CELL_UNDERVOLTAGE:
            this->popupLineTwo = "CELL UNDERVOLTAGE";
            break;
    }

    this->popupLineThree = "Error";
    this->popupTextColour = GxEPD_WHITE;
    this->popupBackgroundColour = GxEPD_RED;
    this->popupClosable = true;

    this->screenLayout = ScreenLayout::POPUP;
    this->needsRedraw = true;
    loggingStream.println("Screen::openBMSErrorPopup()");
}

void Screen::openBLEPopup() {
    this->popupLineOne = "BLE";
    this->popupLineTwo = "Active";
    this->popupLineThree = "";
    this->popupTextColour = GxEPD_WHITE;
    this->popupBackgroundColour = GxEPD_BLACK;
    this->popupClosable = true;

    this->screenLayout = ScreenLayout::POPUP;
    this->needsRedraw = true;
    loggingStream.println("Screen::openBLEPopup()");
}

void Screen::drawMenu() {
    loggingStream.println("Drawing menu");

    this->display->fillScreen(GxEPD_WHITE);
    this->drawRow(0, "MENU", "EXIT");

    this->drawRow(
        1,
        "APP BT",
        this->bleIsActive ? "ON" : "OFF", 
        "", 
        bleIsActive ? GxEPD_BLACK : GxEPD_WHITE,
        bleIsActive ? GxEPD_BLACK : GxEPD_WHITE
    );

    this->drawRow(4, "Reboot", "Do it", "Now", GxEPD_RED, GxEPD_RED);
}

void Screen::openMenu() {
    this->screenLayout = ScreenLayout::MENU;
    this->needsRedraw = true;
    loggingStream.println("Screen::openMenu()");
}

// ------------------------------------- Data functions ------------------------------------

void Screen::setBatteryVoltage(float voltage) {
    if (this->batteryVoltage == voltage) {
        return;
    }

    this->batteryVoltage = voltage;
    if (this->screenLayout == ScreenLayout::MAIN_SCREEN) {
        loggingStream.println("Screen::setBatteryVoltage() - needs redraw");
        this->needsRedraw = true;
    }
}

void Screen::setBatteryVoltageWarning(bool batteryVoltageWarning) {
    if (this->batteryVoltageWarning == batteryVoltageWarning) {
        return;
    }

    this->batteryVoltageWarning = batteryVoltageWarning;
    if (this->screenLayout == ScreenLayout::MAIN_SCREEN) {
        loggingStream.println("Screen::setBatteryVoltageWarning() - needs redraw");
        this->needsRedraw = true;
    }
}

void Screen::setBatteryPercentage(uint8_t percentage) {
    if (this->batteryPercentage == percentage) {
        return;
    }

    this->batteryPercentage = percentage;
    if (this->screenLayout == ScreenLayout::MAIN_SCREEN) {
        loggingStream.println("Screen::setBatteryPercentage() - needs redraw");
        this->needsRedraw = true;
    }
}

void Screen::setBatteryTemperatureWarning(bool batteryTemperatureWarning) {
    if (this->batteryTemperatureWarning == batteryTemperatureWarning) {
        return;
    }

    this->batteryTemperatureWarning = batteryTemperatureWarning;
    if (this->screenLayout == ScreenLayout::MAIN_SCREEN) {
        loggingStream.println("Screen::setBatteryTemperatureWarning() - needs redraw");
        this->needsRedraw = true;
    }
}

void Screen::setChargingIsEnabled(bool chargingIsEnabled) {
    if (this->chargingIsEnabled == chargingIsEnabled) {
        return;
    }

    this->chargingIsEnabled = chargingIsEnabled;
    if (this->screenLayout == ScreenLayout::MAIN_SCREEN) {
        loggingStream.println("Screen::setChargingIsEnabled() - needs redraw");
        this->needsRedraw = true;
    }
}

void Screen::setChargeCurrent(float chargeCurrent) {
    if (this->chargeCurrent == chargeCurrent) {
        return;
    }

    this->chargeCurrent = chargeCurrent;
    if (this->screenLayout == ScreenLayout::MAIN_SCREEN) {
        loggingStream.println("Screen::setChargeCurrent() - needs redraw");
        this->needsRedraw = true;
    }
}

void Screen::setChargeOvercurrentWarning(bool chargeOvercurrentWarning) {
    if (this->chargeOvercurrentWarning == chargeOvercurrentWarning) {
        return;
    }

    this->chargeOvercurrentWarning = chargeOvercurrentWarning;
    if (this->screenLayout == ScreenLayout::MAIN_SCREEN) {
        loggingStream.println("Screen::setChargeOvercurrentWarning() - needs redraw");
        this->needsRedraw = true;
    }
}

void Screen::setDischargingIsEnabled(bool dischargingIsEnabled) {
    if (this->dischargingIsEnabled == dischargingIsEnabled) {
        return;
    }

    this->dischargingIsEnabled = dischargingIsEnabled;
    if (this->screenLayout == ScreenLayout::MAIN_SCREEN) {
        loggingStream.println("Screen::setDischargingIsEnabled() - needs redraw");
        this->needsRedraw = true;
    }
}

void Screen::setDischargeCurrent(float dischargeCurrent) {
    if (this->dischargeCurrent == dischargeCurrent) {
        return;
    }

    this->dischargeCurrent = dischargeCurrent;
    if (this->screenLayout == ScreenLayout::MAIN_SCREEN) {
        loggingStream.println("Screen::setDischargeCurrent() - needs redraw");
        this->needsRedraw = true;
    }
}

void Screen::setDischargeOvercurrentWarning(bool dischargeOvercurrentWarning) {
    if (this->dischargeOvercurrentWarning == dischargeOvercurrentWarning) {
        return;
    }

    this->dischargeOvercurrentWarning = dischargeOvercurrentWarning;
    if (this->screenLayout == ScreenLayout::MAIN_SCREEN) {
        loggingStream.println("Screen::setDischargeOvercurrentWarning() - needs redraw");
        this->needsRedraw = true;
    }
}

void Screen::setUsbIsEnabled(bool isEnabled) {
    if (this->usbIsEnabled == isEnabled) {
        return;
    }

    this->usbIsEnabled = isEnabled;
    if (this->screenLayout == ScreenLayout::MAIN_SCREEN) {
        loggingStream.println("Screen::setUsbIsEnabled() - needs redraw");
        this->needsRedraw = true;
    }
}

void Screen::setFourSOutputIsEnabled(bool fourSOutputIsEnabled) {
    if (this->fourSOutputIsEnabled == fourSOutputIsEnabled) {
        return;
    }

    this->fourSOutputIsEnabled = fourSOutputIsEnabled;
    if (this->screenLayout == ScreenLayout::MAIN_SCREEN) {
        loggingStream.println("Screen::setFourSOutputIsEnabled() - needs redraw");
        this->needsRedraw = true;
    }
}

void Screen::setBLEIsActive(bool bleIsActive) {
    if (this->bleIsActive == bleIsActive) {
        return;
    }

    this->bleIsActive = bleIsActive;
    if (this->screenLayout == ScreenLayout::MENU) {
        loggingStream.println("Screen::setBleIsActive() - needs redraw");
        this->needsRedraw = true;
    }
}

// ------------------------------------- Basic functions -----------------------------------

Screen::Screen() {
}

void Screen::initaliseScreen() {
    this->display->init(115200, true, 2, false);
    //// this->display->init(); for older Waveshare HAT's
    SPI.end();
    SPI.begin(EPD_SCK, EPD_MISO, EPD_MOSI, EPD_CS);
    this->display->setRotation(0);
    this->display->setTextSize(1);
    this->display->setFont(&DejaVu_Sans_Bold_11);
    this->display->setTextColor(GxEPD_BLACK);
    this->display->setFullWindow();
    this->display->setTextWrap(false);
}

void Screen::initVariables() {
    this->batteryVoltage = 0.0;
    this->batteryVoltageWarning = false;
    this->batteryPercentage = 0;
    this->batteryTemperatureWarning = false;
    this->chargingIsEnabled = false;
    this->chargeCurrent = 0.0;
    this->dischargingIsEnabled = false;
    this->dischargeCurrent = 0.0;
    this->fourSOutputIsEnabled = false;
    this->usbIsEnabled = false;
    this->needsRedraw = false;
    this->screenLayout = MAIN_SCREEN;
    this->popupBackgroundColour = GxEPD_WHITE;
    this->popupTextColour = GxEPD_BLACK;
    this->popupClosable = false;
    this->popupLineOne = "";
    this->popupLineTwo = "";
    this->popupLineThree = "";
    this->bleIsActive = false;
}

void Screen::begin() {
    loggingStream.println("Screen begin");
    this->display = new GxEPD2_3C<GxEPD2_213_Z98c, GxEPD2_213_Z98c::HEIGHT>(GxEPD2_213_Z98c(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));
    this->initVariables();
    this->initaliseScreen();

    loggingStream.println("Screen initialised");
}

void Screen::loop() {
    if (!this->needsRedraw) {
        return;
    }

    this->needsRedraw = false;

    switch (this->screenLayout) {
        case ScreenLayout::MAIN_SCREEN:
            this->drawMainScreen();
            break;
        case ScreenLayout::POPUP:
            this->drawPopup();
            break;
        case ScreenLayout::MENU:
            this->drawMenu();
            break;
    }

    this->display->display();
}
