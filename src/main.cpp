#include <arduino.h>
#include <HardwareSerial.h>
#include <bluefairy.h>

#include "screen.hpp"
#include "bms.hpp"
#include "const.hpp"
#include "main.state.hpp"
#include "error-bms.state.hpp"
#include "buttons.hpp"

TaskHandle_t UiTask;
TaskHandle_t BackendTask;

Screen screen;

HardwareSerial &BMSSerial = Serial1;
bluefairy::StateMachine<2> stateMachine;

JbdBms bmsLib(BMSSerial);
BMS bms(&bmsLib, &screen);

bluefairy::Scheduler scheduler;
MainState mainState(&bms, &screen, &stateMachine, &scheduler);
ErrorBmsState errorBmsState(&bms, &screen, &stateMachine, &scheduler);

inline void setupButtons() {
  pinMode(BTN_1_PIN, INPUT_PULLUP);
  pinMode(BTN_2_PIN, INPUT_PULLUP);
  pinMode(BTN_3_PIN, INPUT_PULLUP);
  pinMode(BTN_4_PIN, INPUT_PULLUP);
  pinMode(BTN_5_PIN, INPUT_PULLUP);
}

inline void setupMosfetSwitches() {
  pinMode(MOSFET_4S_PIN, OUTPUT);
  pinMode(MOSFET_6S_PIN, OUTPUT);
}

void UiCode(void * parameter) {
  Serial.print("Starting Screen");
  // print current core
  Serial.print(" on core ");
  Serial.println(xPortGetCoreID());

  screen.begin();

  for (;;) {
    screen.loop();
  }
}

void setup() {
  Serial.begin(115200);

  xTaskCreatePinnedToCore(
      UiCode, /* Function to implement the task */
      "UI", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &UiTask,  /* Task handle. */
      0 /* Core where the task should run */
  );

  Serial.println("Starting BMS Communication");
  BMSSerial.begin(9600, SERIAL_8N1, BMS_SERIAL_RX_PIN, BMS_SERIAL_TX_PIN);
  bms.begin();

  Buttons::begin();
  setupMosfetSwitches();

  Serial.println("Attaching state machines");                                    
  stateMachine[AppState::MAIN_STATE] = mainState;
  stateMachine[AppState::ERROR_BMS_STATE] = errorBmsState;

  Serial.println("Starting state machine");
  stateMachine.toState(AppState::MAIN_STATE);

  Serial.println("Setup done");
  scheduler.every(25, []() {
    Buttons::update();
  });
}

void loop() {
  scheduler.loop();
}