#include <Arduino.h>
#include <bluefairy.h>
#include <HardwareSerial.h>

#include "screen.hpp"
#include "bms.hpp"
#include "const.hpp"
#include "main.state.hpp"
#include "error-bms.state.hpp"
#include "buttons.hpp"
#include "menu.state.hpp"
#include "my_logger.hpp"

HardwareSerial &BMSSerial = Serial1;
HardwareSerial &BLESerial = Serial2;

TaskHandle_t UiTask;
TaskHandle_t BackendTask;

Screen screen;

AppStateMachine stateMachine;

JbdBms bmsLib(BMSSerial);
BMS bms(&bmsLib, &screen);

bluefairy::Scheduler scheduler;
MainState mainState(&bms, &screen, &stateMachine, &scheduler);
ErrorBmsState errorBmsState(&bms, &screen, &stateMachine, &scheduler);
MenuState menuState(&bms, &screen, &stateMachine, &scheduler, BLESerial, BMSSerial);

inline void setupButtons() {
  pinMode(BTN_1_PIN, INPUT_PULLUP);
  pinMode(BTN_2_PIN, INPUT_PULLUP);
  pinMode(BTN_3_PIN, INPUT_PULLUP);
  pinMode(BTN_4_PIN, INPUT_PULLUP);
  pinMode(BTN_5_PIN, INPUT_PULLUP);
}

inline void setupIO() {
  pinMode(MOSFET_4S_PIN, OUTPUT);
  pinMode(MOSFET_USB_PIN, OUTPUT);
  pinMode(BLE_PIN_ENABLE, OUTPUT);
  digitalWrite(MOSFET_4S_PIN, LOW);
  digitalWrite(MOSFET_USB_PIN, LOW);
  digitalWrite(BLE_PIN_ENABLE, LOW);
}

void UiCode(void * parameter) {
  loggingStream.print("UiCode() - Starting UI");
  // print current core
  loggingStream.print(" on core ");
  loggingStream.println(xPortGetCoreID());

  screen.begin();

  for (;;) {
    screen.loop();
  }
}

void setup() {
  Serial.begin(115200);

  loggingStream.println("setup() - Starting UI");

  xTaskCreatePinnedToCore(
      UiCode, /* Function to implement the task */
      "UI", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &UiTask,  /* Task handle. */
      0 /* Core where the task should run */
  );

  delay(2000);

  loggingStream.println("setup() - Starting BMS");
  BMSSerial.begin(9600, SERIAL_8N1, BMS_SERIAL_RX_PIN, BMS_SERIAL_TX_PIN);
  bms.begin();

  loggingStream.println("setup() - Configuring BLE Serial");
  BLESerial.begin(9600, SERIAL_8N1, BLE_PIN_RX, BLE_PIN_TX);

  loggingStream.println("setup() - Preparing IO");
  Buttons::begin();
  setupIO();

  loggingStream.println("setup() - Starting State Machine");                                    
  stateMachine[AppState::MAIN_STATE] = mainState;
  stateMachine[AppState::ERROR_BMS_STATE] = errorBmsState;
  stateMachine[AppState::MENU_STATE] = menuState;

  stateMachine.toState(AppState::MAIN_STATE);

  loggingStream.println("setup() - Scheduling Buttons::update()");
  scheduler.every(25, []() {
    Buttons::update();
  });

  loggingStream.println("setup() - done");
}

void loop() {
  scheduler.loop();
}