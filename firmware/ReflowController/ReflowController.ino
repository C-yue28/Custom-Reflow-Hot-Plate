#include "MAX6675.h"
#include "ProfileEditorGUI.h" 

/*

Reflow Hot Plate Firmware
Written by Christopher Yue

*/

// PIN DESIGNATION
#define SW_UP 2
#define SW_DOWN 3
#define SW_NEXT 4
#define RELAY_1 5
#define RELAY_2 6
#define SERIAL_INPUT 7 // SO pin of MAX6675
#define SERIAL_OUT 8 // SER pin of 74HC595
#define SRCLK 9 // shift register clock 74HC595
#define RCLK 10 // register clock 74HC595
#define CHIP_SELECT 12 // CS pin MAX6675
#define SERIAL_CLOCK 13 // SCK pin MAX6675

// PARAMETERS
// default HEATING_PARAMETERS
#define HEATING_RATE 3.0
#define PREHEAT_TEMP 150.0
#define SOAK_TEMP 180.0
#define SOAK_DURATION 90.0
#define REFLOW_TEMP 245.0
#define REFLOW_TAL 45.0
#define COOLING_RATE -3.0

float HEATING_PARAMETERS[7] = {PREHEAT_TEMP, SOAK_TEMP, REFLOW_TEMP, SOAK_DURATION, REFLOW_TAL, HEATING_RATE, COOLING_RATE};
const float INCREMENTS[7] = {2.5, 2.5, 2.5, 5, 5, 0.25, 0.25};

const int aluminum_mass_g = 38.3975426;

MAX6675 thermocouple(SERIAL_CLOCK, CHIP_SELECT, SERIAL_INPUT);
LCD1602 display(SRCLK, RCLK, SERIAL_OUT); // controlled using 74HC595, RS is B, R/W is C, E is D, data pins 4-7 correspond to E-H
ProfileEditorGUI gui(display);
// SHIFT_REG_74HC595 shiftRegister(SERIAL_OUT, SRCLK, RCLK, SRCLR);
float temp = 0;

bool HEATING = false;
bool STATE_SW_NEXT = false;

bool hasAdvanced = false;
bool check_gui = true;
volatile bool hasEdited = false;

// bool hasIncremented = false;
// bool hasIncremented = false;

void incrementValue() {
  delay(50);
  if (!digitalRead(SW_UP)) {
    if (!hasEdited) {
      HEATING_PARAMETERS[gui.screen]+=INCREMENTS[gui.screen];
      gui.edit_param(HEATING_PARAMETERS[gui.screen]);
      hasEdited = true;
    }
  } else {
    hasEdited = false;
  }
  delay(50);
}

void decrementValue() {
  delay(50);
  if (!digitalRead(SW_DOWN)) {
    if (!hasEdited) {
      HEATING_PARAMETERS[gui.screen]-=INCREMENTS[gui.screen];
      gui.edit_param(HEATING_PARAMETERS[gui.screen]);
      hasEdited = true;
    }
  } else {
    hasEdited = false;
  }
  delay(50);
}

void nextValue() {
  if (gui.advance_screen(gui.screen >= 6 ? NULL : HEATING_PARAMETERS[gui.screen+1])) {
    start();
    return;
  }
  delay(50);
}

void start() {
  detachInterrupt(digitalPinToInterrupt(SW_UP));
  detachInterrupt(digitalPinToInterrupt(SW_DOWN));
  HEATING = true;
  check_gui = false;

  display.clear_all();
  display.setRow(0);
  display.print("HEATING");
  display.setRow(1);
  display.print("HOLD 1S TO STOP");
  delay(50);
}

void setup() {

  Serial.begin(115200);
  delay(1000);

  pinMode(SW_UP, INPUT_PULLUP);
  pinMode(SW_DOWN, INPUT_PULLUP);
  pinMode(SW_NEXT, INPUT_PULLUP);

  delay(500);

  attachInterrupt(digitalPinToInterrupt(SW_UP), incrementValue, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SW_DOWN), decrementValue, CHANGE);

  display.begin();
  Serial.println("Ready.");
  display.print("Ready.");

  delay(500);

  gui.update(HEATING_PARAMETERS[gui.screen]);

}

void loop() {
  bool state = digitalRead(SW_NEXT);

  if (check_gui) {
    if (!state && STATE_SW_NEXT) {
      if (!hasAdvanced) {
        nextValue();
        hasAdvanced = true;
      }
    } else if (state && !STATE_SW_NEXT) {
      hasAdvanced = false;
    }
  } else if (!state) {
    // reset when next button is pressed while heating is going on
    // must hold for at least a second to stop
    delay(1000);
    if (!digitalRead(SW_NEXT)) {
      HEATING = false;
      gui.screen = 0;
      gui.update(HEATING_PARAMETERS[0]);
      check_gui = true;
      interrupts();
    }
  }

  STATE_SW_NEXT = state;
  delay(50);
  // display.print(String(thermocouple.readData()).c_str());
  // delay(1);
  // display.clear_all();
}