#include "MAX6675.h"
#include "LCD1602.h"

/*

Reflow Hot Plate Firmware
Written by Christopher Yue

*/

// PIN DESIGNATION
#define SW_1 0
#define SW_2 1
#define SRCLK 3 // shift register clock 74HC595
#define RCLK 4 // register clock 74HC595
#define RELAY_1 5
#define RELAY_2 6
#define SRCLR 7 // shift register clear 74HC595
#define SERIAL_OUT 8 // SER pin of 74HC595
#define SERIAL_INPUT 11 // SO pin of MAX6675
#define CHIP_SELECT 12 // CS pin MAX6675
#define SERIAL_CLOCK 13 // SCK pin MAX6675

const int aluminum_mass_g = 38.3975426;

//CY_MAX6675 thermocouple(SERIAL_CLOCK, CHIP_SELECT, SERIAL_INPUT);
CY_LCD1602 display(SRCLK, RCLK, SERIAL_OUT); // controlled using 74HC595, RS is B, R/W is C, E is D, data pins 4-7 correspond to E-H
// CY_74HC595 shiftRegister(SERIAL_OUT, SRCLK, RCLK, SRCLR);
float temp = 0;

void updateDisplay(bool button, bool n) {
  if (button && n) {
    // go to next screen
  } else if (button && !n){
    // update values on current screen if applicable
  } else {
    // update values with the temperature
  }
}

void displayHandler_1() {
  updateDisplay(true, true);
}

void displayHandler_2() {
  updateDisplay(true, false);
}

void setup() {

  Serial.begin(115200);
  delay(1000);

  // attachInterrupt(digitalPinToInterrupt(SW_1), displayHandler_1, LOW);
  // attachInterrupt(digitalPinToInterrupt(SW_2), displayHandler_2, LOW);

  // display.print("A");
  // delay(1000);
  // display.print((char*)01001000);
  display.begin();
  Serial.println("Ready.");

  delay(500);
  display.print("Hello, World!");
  delay(500);

}

void loop() {

}