#include <pgmspace.h>

#include <SPI.h>               // Built-in 
#define  ENABLE_GxEPD2_display 0
#include <GxEPD2_BW.h>         // GxEPD2 from Sketch, Include Library, Manage Libraries, search for GxEDP2
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include "epaper_fonts.h"

#include "screen.hpp"

GxEPD2_3C<GxEPD2_213_Z98c, GxEPD2_213_Z98c::HEIGHT> display(GxEPD2_213_Z98c(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

// default constructor
Screen::Screen() {
} //Screen

void Screen::initaliseScreen() {
    display.init(115200, true, 2, false);
    //// display.init(); for older Waveshare HAT's
    SPI.end();
    SPI.begin(EPD_SCK, EPD_MISO, EPD_MOSI, EPD_CS);
    display.setRotation(0);
    display.setTextSize(1);
    display.setFont(&DejaVu_Sans_Bold_11);
    display.setTextColor(GxEPD_BLACK);
    display.fillScreen(GxEPD_WHITE);
    display.setFullWindow();
    display.setTextWrap(false);
}

void Screen::initVariables() {
    this->batteryVoltage = 0.0;
    this->hasChanged = true;
    this->lastTick = 0;
    this->batteryTemperature = 0.0;
    this->chargingIsEnabled = false;
    this->chargeCurrent = 0.0;
    this->dischargingIsEnabled = false;
    this->dischargeCurrent = 0.0;
    this->sixSIsEnabled = false;
}

void Screen::begin() {
    this->initVariables();
    this->initaliseScreen();
}

void Screen::drawString(
    int x,
    int y,
    String text,
    HorizontalAlignment horizontalAlignment,
    VerticalAlignment verticalAlignment
) {
    int16_t  x1, y1;
    uint16_t w, h;
    display.getTextBounds(text, x, y, &x1, &y1, &w, &h);
    if (horizontalAlignment == HRIGHT)  x = x - w;
    if (horizontalAlignment == HCENTER) x = x - w / 2;

    if (verticalAlignment == VBOTTOM)  y = y - h;
    if (verticalAlignment == VCENTER) y = y - h / 2;

    display.setCursor(x, y + h);
    display.print(text);
}

void Screen::drawRow(
    uint8_t row,
    String label,
    String valueRowOne,
    String valueRowTwo
) {
    this->drawMenuBox(row, 0, label);
    this->drawMenuBox(row, 1, valueRowOne, valueRowTwo);

    if (row == 0) {
        return;
    }

    display.drawLine(
        0,
        row * SCREEN_ROW_HEIGHT,
        SCREEN_WIDTH,
        row * SCREEN_ROW_HEIGHT,
        GxEPD_BLACK
    );

    display.drawLine(
        SCREEN_COLUMN_WIDTH,
        0,
        SCREEN_COLUMN_WIDTH,
        SCREEN_HEIGHT,
        GxEPD_BLACK
    );
}

void Screen::drawMenuBox(
    uint8_t row,
    uint8_t collumn,
    String textRowOne,
    String textRowTwo
) {
    uint8_t x = collumn * SCREEN_COLUMN_WIDTH;
    uint8_t y = row * SCREEN_ROW_HEIGHT;

    display.fillRect(
        x,
        y,
        SCREEN_COLUMN_WIDTH,
        SCREEN_ROW_HEIGHT,
        GxEPD_WHITE
    );

    if (textRowTwo.length() == 0) {
         this->drawString(
            x + SCREEN_COLUMN_WIDTH / 2,
            y + SCREEN_ROW_HEIGHT / 2,
            textRowOne,
            HorizontalAlignment::HCENTER,
            VerticalAlignment::VCENTER
        );
        return;
    }

    this->drawString(
        x + SCREEN_COLUMN_WIDTH / 2,
        y + SCREEN_ROW_HEIGHT / 4,
        textRowOne,
        HorizontalAlignment::HCENTER,
        VerticalAlignment::VCENTER
    );

    this->drawString(
        x + SCREEN_COLUMN_WIDTH / 2,
        y + SCREEN_ROW_HEIGHT / 4 * 3,
        textRowTwo,
        HorizontalAlignment::HCENTER,
        VerticalAlignment::VCENTER
    );
}

void Screen::drawMainRow() {
    this->drawRow(0, "State", String(this->batteryVoltage) + "V", String(this->batteryTemperature) + "°C");
}

void Screen::drawChargeRow() {
    String state = "";
    if (this->chargingIsEnabled) {
        state.concat("ON");
    } else {
        state.concat("OFF");
    }

    this->drawRow(1, "IN", state, String(this->chargeCurrent) + "A");
}

void Screen::drawDischargeRow() {
    String state = "";
    if (this->dischargingIsEnabled) {
        state.concat("ON");
    } else {
        state.concat("OFF");
    }

    this->drawRow(2, "OUT", state, String(this->dischargeCurrent) + "A");
}

void Screen::drawSixSRow() {
    String state = "";
    if (this->sixSIsEnabled) {
        state.concat("ON");
    } else {
        state.concat("OFF");
    }

    this->drawRow(4, "6S", state);
}

void Screen::drawFourSRow() {
    String state = "";
    if (this->fourSOutputIsEnabled) {
        state.concat("ON");
    } else {
        state.concat("OFF");
    }

    this->drawRow(3, "4S", state);
}

void Screen::updateScreen() {
    display.fillScreen(GxEPD_BLACK);

    this->drawMainRow();
    this->drawChargeRow();
    this->drawDischargeRow();
    this->drawSixSRow();
    this->drawFourSRow();

    display.display(true);
    this->hasChanged = false;
}

void Screen::tick() {
    if (!this->hasChanged) {
        return;
    }

    if (this->lastTick + 5000 > millis()) {
        return;
    }

    this->updateScreen();
}

void Screen::setBatteryVoltage(float voltage) {
    if (this->batteryVoltage == voltage) {
        return;
    }

    this->batteryVoltage = voltage;
    this->hasChanged = true;
}

void Screen::setBatteryTemperature(float temperature) {
    if (this->batteryTemperature == temperature) {
        return;
    }

    this->batteryTemperature = temperature;
    this->hasChanged = true;
}

void Screen::setChargingIsEnabled(bool chargingIsEnabled) {
    if (this->chargingIsEnabled == chargingIsEnabled) {
        return;
    }

    this->chargingIsEnabled = chargingIsEnabled;
    this->hasChanged = true;
}

void Screen::setChargeCurrent(float chargeCurrent) {
    if (this->chargeCurrent == chargeCurrent) {
        return;
    }

    this->chargeCurrent = chargeCurrent;
    this->hasChanged = true;
}

void Screen::setDischargingIsEnabled(bool dischargingIsEnabled) {
    if (this->dischargingIsEnabled == dischargingIsEnabled) {
        return;
    }

    this->dischargingIsEnabled = dischargingIsEnabled;
    this->hasChanged = true;
}

void Screen::setDischargeCurrent(float dischargeCurrent) {
    if (this->dischargeCurrent == dischargeCurrent) {
        return;
    }

    this->dischargeCurrent = dischargeCurrent;
    this->hasChanged = true;
}

void Screen::setSixSIsEnabled(bool sixSIsEnabled) {
    if (this->sixSIsEnabled == sixSIsEnabled) {
        return;
    }

    this->sixSIsEnabled = sixSIsEnabled;
    this->hasChanged = true;
}

void Screen::setFourSOutputIsEnabled(bool fourSOutputIsEnabled) {
    if (this->fourSOutputIsEnabled == fourSOutputIsEnabled) {
        return;
    }

    this->fourSOutputIsEnabled = fourSOutputIsEnabled;
    this->hasChanged = true;
}
