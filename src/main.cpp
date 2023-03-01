
#include <SPI.h>               // Built-in 
#define  ENABLE_GxEPD2_display 0
#include <GxEPD2_BW.h>         // GxEPD2 from Sketch, Include Library, Manage Libraries, search for GxEDP2
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include "epaper_fonts.h"

static const uint8_t EPD_BUSY = 4;  // to EPD BUSY
static const uint8_t EPD_CS   = 5;  // to EPD CS
static const uint8_t EPD_RST  = 16; // to EPD RST
static const uint8_t EPD_DC   = 17; // to EPD DC
static const uint8_t EPD_SCK  = 18; // to EPD CLK
static const uint8_t EPD_MISO = 19; // Master-In Slave-Out not used, as no data from display
static const uint8_t EPD_MOSI = 23; // to EPD DIN

GxEPD2_3C<GxEPD2_213_Z98c, GxEPD2_213_Z98c::HEIGHT> display(GxEPD2_213_Z98c(/*CS=5*/ EPD_CS, /*DC=17*/ EPD_DC, /*RST=16*/ EPD_RST, /*BUSY=4*/ EPD_BUSY));

enum Alignment {LEFT, CENTER, RIGHT};

void InitialiseDisplay() {
  Serial.println("Initialising Display");

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

  Serial.println("Display Initialised");
}


void drawString(int x, int y, String text, Alignment align = LEFT) {
  Serial.println("Drawing String");

  int16_t  x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, x, y, &x1, &y1, &w, &h);
  if (align == RIGHT)  x = x - w;
  if (align == CENTER) x = x - w / 2;

  display.setCursor(x, y + h);
  display.print(text);

  Serial.println("String Drawn");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");

  InitialiseDisplay();

  drawString(0, 0, "000, 000");
  drawString(0, 50, "000, 050");
  drawString(0, 100, "000, 100");
  drawString(0, 150, "000, 150");
  drawString(0, 200, "000, 200");
  drawString(0, 210, "000, 210");
  drawString(0, 220, "000, 220");
  drawString(0, 230, "000, 230");
  drawString(0, 240, "000, 240");
  drawString(0, 250, "000, 250");

  display.display(true);

  Serial.println("Done");
}

void loop() {
  Serial.println("Loop");
  delay(5000);
}