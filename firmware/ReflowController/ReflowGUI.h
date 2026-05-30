#include <Arduino.h>
#include "LCD1602.h"

// GUI text
#define HEATING_RATE_TXT "Heat Rate(C/s):"
#define PREHEAT_TEMP_TXT 
#define SOAK_TEMP_TXT "Soak Temp(C):"
#define SOAK_DURATION_TXT "Soak Time(s):"
#define REFLOW_TEMP_TXT "Reflow T(C):"
#define REFLOW_TAL_TXT "TAL Time(s):"
#define COOLING_RATE_TXT "Cool Rate(C/s):"

#define PREHEAT_TEMP_TXT "Preheat T(C):"
#define PREHEAT_DURATION_TXT "Preheat Time(s):"
#define SOAK_TEMP_TXT "Soak Temp(C):"
#define SOAK_DURATION_TXT "Soak Time(s):"
#define MELTING_POINT_TXT "Melt Temp(C):"
#define RAMP_DURATION_TXT "Ramp Time(s):"
#define REFLOW_TEMP_TXT "Reflow T(C):"
#define REFLOW_DURATION_TXT "Reflow TAL(s):"

#define MAX_SCREENS 9

class ReflowGUI {
  public:
    ReflowGUI(LCD1602 _display);
    bool advance_screen(float val);
    void edit_param(float newVal);
    void update(float val);
    void printTemperatureWhileHeating(float temp, int phase);
    void printHeating(int phase);
    int screen;

  private:
    const char* SCREEN_TEXT[MAX_SCREENS] = {
        PREHEAT_TEMP_TXT,
        PREHEAT_DURATION_TXT,
        SOAK_TEMP_TXT,
        SOAK_DURATION_TXT,
        MELTING_POINT_TXT,
        RAMP_DURATION_TXT,
        REFLOW_TEMP_TXT,
        REFLOW_DURATION_TXT,
        "CONFIRM START?"
    };    
    LCD1602 display;
};