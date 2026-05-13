#include <Arduino.h>

#define DELAY 0.1 // microseconds

class CY_74HC595 {

  public:
    CY_74HC595(uint8_t DATA, uint8_t CLOCK, uint8_t LATCH);
    CY_74HC595();
    void writeData(uint8_t Q, uint8_t MODE);
    void clearAll();
    void turnOn(uint8_t regPin);

  private:
    uint8_t shift_reg_clock;
    uint8_t reg_clock;
    uint8_t data_output;

};