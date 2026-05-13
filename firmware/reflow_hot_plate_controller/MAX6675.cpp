#include "MAX6675.h"
#include <Arduino.h>

CY_MAX6675::CY_MAX6675(int _SCK, int _CS, int _SO) {
  SCK = _SCK;
  CS = _CS;
  SO = _SO;

  pinMode(SCK, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(SO, INPUT);
}

float CY_MAX6675::readData() {
  digitalWrite(CS, LOW);
  delayMicroseconds(1); // minimum 100 ns = 0.1 us + some tolerance
  int data = 0;
  
  // timing:
  // SCK fall to output data valid max 100 ns = 0.1 us, I'm just going to round it up to one microsecond, can't hurt

  for (int i = 0; i < 16; i++) {
    digitalWrite(SCK, LOW);
    delayMicroseconds(1);
    if (digitalRead(SO)) {
      data = data | (1 << i);
    }
    digitalWrite(SCK, HIGH);
    delayMicroseconds(1);
  }
  digitalWrite(CS, HIGH);

  // LSB is useless, bit 1 is useless, bit 2 is thermocouple checker, bit 15 is useless

  if (data & 4) return -1;
  return (float)(data >> 3) / 4;
  

}