#include "LCD1602.h"
#include <Arduino.h>

CY_LCD1602::CY_LCD1602(int SRCLK, int RCLK, int SER) {
  reg = CY_74HC595(SER, SRCLK, RCLK);
  reg.clearAll();
}

void CY_LCD1602::begin() {

  int function_set_initialize = (FUNCTION_SET | DL_8D) >> 4;

  delay(100); // min 45ms

  writeData(function_set_initialize, false, true);
  delay(5); // min 4.1ms
  writeData(function_set_initialize, false, true);
  delayMicroseconds(200); 
  writeData(function_set_initialize, false, true);
  delayMicroseconds(200); 

  writeData(FUNCTION_SET >> 4, false, true);
  writeData(FUNCTION_SET | TWO_ROWS | FONT_5X10, true, true); 
  writeData(DISPLAY_SET | DISPLAY_ON, true, true);
  writeData(SCREEN_CLEAR, true, true);
  delay(5); // min 1.52 ms
  writeData(INPUT_SET | INCREMENT, true, true);
  reg.clearAll();
}

void CY_LCD1602::clear_all() {
  writeData(SCREEN_CLEAR, true, true);
}

void CY_LCD1602::print(char* data) {
  while (*data != '\0') {
    writeData((int)*data, true, false);
    data++;
  }
}

void CY_LCD1602::writeData(int _data, bool mode, bool is_init) { // receives 8 bit data but we are using 4 bit interface
  int data1 = (_data & 240);
  int data2 = ((_data & 15) << 4);

  if (!is_init) {
    data1 |= REGISTER_SELECT; 
    data2 |= REGISTER_SELECT; 
  }

  if (mode) {

    if (!is_init) {
      reg.writeData(REGISTER_SELECT, MSBFIRST);
    }

    delayMicroseconds(0.5); // min 60 ns
    reg.writeData(data1, MSBFIRST);
    reg.writeData(data1 | ENABLE, MSBFIRST);
    delayMicroseconds(1); // min 450 ns pulse width
    reg.writeData(data1, MSBFIRST); // switch enable off
    delayMicroseconds(0.5); // min 10 ns data hold time & 20 ns address hold time
    reg.clearAll(); // turn all off

    delayMicroseconds(0.5); // min cycle time 1000 ns, add a bit of extra buffer for safety

  }

  if (!is_init) {
    reg.writeData(REGISTER_SELECT, MSBFIRST);
  }

  delayMicroseconds(0.5); // min 60 ns
  reg.writeData(data2, MSBFIRST);
  reg.writeData(data2 | ENABLE, MSBFIRST);
  delayMicroseconds(1); // min 450 ns pulse width
  reg.writeData(data2, MSBFIRST); // switch enable off
  delayMicroseconds(0.5); // min 10 ns data hold time & 20 ns address hold time
  reg.clearAll(); // turn all off

  delayMicroseconds(1000); // almost all commands need 37us; only return home needs like 2 ms or something, we also account for enable cycle time here
}

