#include "74HC595.h"
#include <Arduino.h>

CY_74HC595::CY_74HC595(uint8_t DATA, uint8_t CLOCK, uint8_t LATCH) {
  shift_reg_clock = CLOCK;
  reg_clock = LATCH;
  data_output = DATA;
  
  pinMode(shift_reg_clock, OUTPUT);
  pinMode(data_output, OUTPUT);
  pinMode(reg_clock, OUTPUT);

  digitalWrite(shift_reg_clock, LOW);
  digitalWrite(data_output, LOW);
  digitalWrite(reg_clock, LOW);

  clearAll();
}

CY_74HC595::CY_74HC595() {

}


void CY_74HC595::clearAll() {
  digitalWrite(reg_clock, LOW); 
  delayMicroseconds(DELAY);
  digitalWrite(reg_clock, HIGH);
  delayMicroseconds(DELAY);
  digitalWrite(reg_clock, LOW);
}

void CY_74HC595::turnOn(uint8_t regPin) {
  writeData((uint8_t)(1 << regPin), MSBFIRST);
}

void CY_74HC595::writeData(uint8_t Q, uint8_t MODE) {

  clearAll();
  delayMicroseconds(DELAY);
  digitalWrite(shift_reg_clock, LOW);

  for (int i = 0; i < 8; i++) {
    if (MODE == MSBFIRST) {
      digitalWrite(data_output, !!(Q & (1 << (7-i))));
    } else {
      digitalWrite(data_output, !!(Q & (1 << i)));
    }
    delayMicroseconds(DELAY);
    digitalWrite(shift_reg_clock, HIGH);
    delayMicroseconds(DELAY);
    digitalWrite(shift_reg_clock, LOW);
  }

  digitalWrite(shift_reg_clock, LOW);

  digitalWrite(reg_clock, HIGH); 
  delayMicroseconds(DELAY);
  digitalWrite(reg_clock, LOW);

}