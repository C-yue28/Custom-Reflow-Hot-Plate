#include <Arduino.h>
#include "SHIFT_REG_74HC595.h"

#define SCREEN_CLEAR 1
#define CURSOR_RETURN 2

// input_set
#define INPUT_SET 4
#define INCREMENT 2
#define SHIFT 1

// display
#define DISPLAY_SET 8
#define DISPLAY_ON 4
#define CURSOR_ON 2
#define BLINK_ON 1

// shift functions
#define SHIFT_FUNCTION 16
#define DISPLAY_SHIFT 8
#define RIGHT_SHIFT 4

// function set
#define FUNCTION_SET 32
#define DL_8D 16
#define TWO_ROWS 8
#define FONT_5X10 4

// address set
#define CGRAM_AD_SET 64
#define DDRAM_AD_SET 128

// 74HC595 bits
#define REGISTER_SELECT 2
#define ENABLE 8

class LCD1602 {

  public:
    LCD1602(int SRCLK, int RCLK, int SER);
    LCD1602();
    void begin();
    void writeData(int data, bool mode, bool is_init);
    void print(const char* data);
    void setRow(int row);
    void clear_all();

  private:
    SHIFT_REG_74HC595 reg;

};