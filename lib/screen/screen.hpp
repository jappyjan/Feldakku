#pragma once

#include <GxEPD2_3C.h>
#include <pgmspace.h>

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
enum BMSError {
    NO_ERROR = 0,
    COMMUNICATION_ERROR = 1,
};

class Screen {
    public:
        Screen();
        void begin();
        void tick();

        void setBatteryVoltage(float voltage);
        void setBatteryVoltageWarning(bool batteryVoltageWarning);
        void setBatteryTemperature(float temperature);
        void setBatteryTemperatureWarning(bool batteryTemperatureWarning);
        void setChargingIsEnabled(bool chargingIsEnabled);
        void setChargeCurrent(float chargeCurrent);
        void setDischargingIsEnabled(bool dischargingIsEnabled);
        void setDischargeCurrent(float dischargeCurrent);
        void setSixSIsEnabled(bool usbIsEnabled);
        void setFourSOutputIsEnabled(bool fourSOutputIsEnabled);
        void setBMSError(BMSError error);
    
    private:
        float batteryVoltage;
        bool batteryVoltageWarning;
        float batteryTemperature;
        bool batteryTemperatureWarning;
        bool chargingIsEnabled;
        float chargeCurrent;
        bool dischargingIsEnabled;
        float dischargeCurrent;
        bool sixSIsEnabled;
        bool fourSOutputIsEnabled;
        bool hasChanged;
        unsigned long lastTick;
        GxEPD2_3C<GxEPD2_213_Z98c, GxEPD2_213_Z98c::HEIGHT> *display;
        uint8_t bmsError;

        void initaliseScreen();
        void initVariables();

        void drawString(int x, int y, String text, HorizontalAlignment align = HLEFT, VerticalAlignment valign = VTOP);
        void drawMenuBox(uint8_t row, uint8_t collumn, String textRowOne, String textRowTwo = "", bool warningRowOne = false, bool warningRowTwo = false);
        void drawRow(uint8_t row, String label, String valueRowOne, String valueRowTwo = "", bool warningRowOne = false, bool warningRowTwo = false);

        void updateScreen();

        void drawMainRow();
        void drawChargeRow();
        void drawDischargeRow();
        void drawSixSRow();
        void drawFourSRow();
        void drawError();
};
