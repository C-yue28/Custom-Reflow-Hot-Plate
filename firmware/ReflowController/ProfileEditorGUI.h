#include <Arduino.h>
#include "LCD1602.h"

// GUI text
#define HEATING_RATE_TXT "Heat Rate(C/s):"
#define PREHEAT_TEMP_TXT "Preheat T(C):"
#define SOAK_TEMP_TXT "Soak Temp(C):"
#define SOAK_DURATION_TXT "Soak Time(s):"
#define REFLOW_TEMP_TXT "Reflow T(C):"
#define REFLOW_TAL_TXT "TAL Time(s):"
#define COOLING_RATE_TXT "Cool Rate(C/s):"

#define MAX_SCREENS 8

class ProfileEditorGUI {
  public:
    ProfileEditorGUI(LCD1602 _display);
    bool advance_screen(float val);
    void edit_param(float newVal);
    void update(float val);
    int screen;

  private:
    const char* SCREEN_TEXT[8] = {PREHEAT_TEMP_TXT, SOAK_TEMP_TXT, REFLOW_TEMP_TXT, SOAK_DURATION_TXT, REFLOW_TAL_TXT, HEATING_RATE_TXT, COOLING_RATE_TXT, "CONFIRM START?"};
    LCD1602 display;
};