#pragma once

#include <GxEPD2_3C.h>
#include <pgmspace.h>
#include <bluefairy.h>
#include "bms-state.hpp"
#include <WiFi.h>

static const uint8_t EPD_BUSY = 4;  // to EPD BUSY
static const uint8_t EPD_CS   = 5;  // to EPD CS
static const uint8_t EPD_RST  = 16; // to EPD RST
static const uint8_t EPD_DC   = 17; // to EPD DC
static const uint8_t EPD_SCK  = 18; // to EPD CLK
static const uint8_t EPD_MISO = 19; // Master-In Slave-Out not used, as no data from display
static const uint8_t EPD_MOSI = 23; // to EPD DIN

static const uint8_t SCREEN_WIDTH = 122;
static const uint8_t SCREEN_HEIGHT = 250;
static const uint8_t SCREEN_ROWS = 5;
static const uint8_t SCREEN_COLUMNS = 2;
static const uint8_t SCREEN_ROW_HEIGHT = SCREEN_HEIGHT / SCREEN_ROWS;
static const uint8_t SCREEN_COLUMN_WIDTH = SCREEN_WIDTH / SCREEN_COLUMNS;

enum HorizontalAlignment {HLEFT, HCENTER, HRIGHT};
enum VerticalAlignment {VTOP, VCENTER, VBOTTOM};

enum ScreenLayout {
    MAIN_SCREEN,
    POPUP,
    MENU,
};

class Screen {
    public:
        Screen();
        void begin();
        void loop();

        void openMainScreen();
        void openBMSErrorPopup(BMSState bmsState);
        void openBLEPopup();
        void openMenu();

        void setBatteryVoltage(float voltage);
        void setBatteryVoltageWarning(bool batteryVoltageWarning);
        void setBatteryPercentage(uint8_t percentage);
        void setBatteryTemperatureWarning(bool batteryTemperatureWarning);
        void setChargingIsEnabled(bool chargingIsEnabled);
        void setChargeCurrent(float chargeCurrent);
        void setChargeOvercurrentWarning(bool chargeOvercurrentWarning);
        void setDischargingIsEnabled(bool dischargingIsEnabled);
        void setDischargeCurrent(float dischargeCurrent);
        void setDischargeOvercurrentWarning(bool dischargeOvercurrentWarning);
        void setUsbIsEnabled(bool usbIsEnabled);
        void setFourSOutputIsEnabled(bool fourSOutputIsEnabled);
        void setBLEIsActive(bool bleIsActive);
        void setUSBUartIsActive(bool usbUartIsActive);
    
    private:
        float batteryVoltage;
        bool batteryVoltageWarning;
        uint8_t batteryPercentage;
        bool batteryTemperatureWarning;
        bool chargingIsEnabled;
        float chargeCurrent;
        bool chargeOvercurrentWarning;
        bool dischargingIsEnabled;
        float dischargeCurrent;
        bool dischargeOvercurrentWarning;
        bool usbIsEnabled;
        bool fourSOutputIsEnabled;
        ScreenLayout screenLayout;
        GxEPD2_3C<GxEPD2_213_Z98c, GxEPD2_213_Z98c::HEIGHT> *display;
        String popupLineOne;
        String popupLineTwo;
        String popupLineThree;
        uint16_t popupTextColour;
        uint16_t popupBackgroundColour;
        bool popupClosable;
        bool needsRedraw;
        bool bleIsActive;
        bool usbUartIsActive;

        void initaliseScreen();
        void initVariables();

        void drawMainScreen();
        void drawPopup();
        void drawMenu();

        void drawString(int x, int y, String text, HorizontalAlignment align = HLEFT, VerticalAlignment valign = VTOP, uint16_t color = GxEPD_BLACK);
        void drawMenuBox(uint8_t row, uint8_t collumn, String textRowOne, String textRowTwo = "", uint16_t bgRowOne = GxEPD_WHITE, uint16_t bgRowTwo = GxEPD_WHITE);
        void drawRow(uint8_t row, String label, String valueRowOne, String valueRowTwo = "", uint16_t bgRowOne = GxEPD_WHITE, uint16_t bgRowTwo = GxEPD_WHITE);

        void drawMainRow();
        void drawChargeRow();
        void drawDischargeRow();
        void drawUsbRow();
        void drawFourSRow();
        void drawError();
};
