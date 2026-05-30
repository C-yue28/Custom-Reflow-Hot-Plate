#include <Arduino.h>
#include "ReflowGUI.h" 

ReflowGUI::ReflowGUI(LCD1602 _display) {
  display = _display;
  screen = 0;
}

void ReflowGUI::update(float val) {
  display.clear_all();
  display.print(SCREEN_TEXT[screen]);
  if (val == NULL) return;
  display.setRow(1);
  display.print(String(val).c_str());
}

const char* phaseFromInt(int phase) {
  if (phase == 0) {
    return "PREHEAT";
  } else if (phase == 1) {
    return "SOAK";
  } else if (phase == 2) {
    return "MELT";
  } else {
    return "REFLOW";
  }
}

void ReflowGUI::printTemperatureWhileHeating(float temp, int phase) {
  display.clear_all();
  display.print(phaseFromInt(phase));
  display.setRow(1);
  display.print(String(temp).c_str());
}

void ReflowGUI::printHeating(int phase) {
  display.clear_all();
  display.setRow(0);
  display.print(phaseFromInt(phase));
  display.setRow(1);
  display.print("HOLD 1S TO STOP");
}

bool ReflowGUI::advance_screen(float val) {
  screen++;
  update(val);
  if (screen >= MAX_SCREENS) {
    return true;
  }
  return false;
}

void ReflowGUI::edit_param(float newVal) {
  update(newVal);
}