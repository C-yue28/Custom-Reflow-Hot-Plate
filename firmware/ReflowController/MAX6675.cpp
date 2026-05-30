#include "MAX6675.h"
#include <Arduino.h>

MAX6675::MAX6675(int _SCK, int _CS, int _SO) {
  SCK = _SCK;
  CS = _CS;
  SO = _SO;

  pinMode(SCK, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(SO, INPUT);

  digitalWrite(CS, HIGH);
  digitalWrite(SCK, LOW);
}

float MAX6675::readData() {
  digitalWrite(CS, LOW);
  delayMicroseconds(1); // minimum 100 ns = 0.1 us + some tolerance
  int data = 0;
  
  // timing:
  // SCK fall to output data valid max 100 ns = 0.1 us, I'm just going to round it up to one microsecond, can't hurt

  for (int i = 0; i < 16; i++) {
    digitalWrite(SCK, HIGH);
    delayMicroseconds(1);

    data |= digitalRead(SO) << (15-i);
    
    digitalWrite(SCK, LOW);
    delayMicroseconds(1);
  }
  digitalWrite(CS, HIGH);

  // bit 0 is useless, bit 1 is useless, bit 2 is thermocouple checker, bit 15 is useless

  if (data & 4) return -1;
  return (float)((data >> 3) & 4095)/4;
  

}