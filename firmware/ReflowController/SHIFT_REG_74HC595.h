#include <Arduino.h>

#define DELAY 0.1 // microseconds

class SHIFT_REG_74HC595 {

  public:
    SHIFT_REG_74HC595(uint8_t DATA, uint8_t CLOCK, uint8_t LATCH);
    SHIFT_REG_74HC595();
    void writeData(uint8_t Q, uint8_t MODE);
    void clearAll();
    void turnOn(uint8_t regPin);

  private:
    uint8_t shift_reg_clock;
    uint8_t reg_clock;
    uint8_t data_output;

};