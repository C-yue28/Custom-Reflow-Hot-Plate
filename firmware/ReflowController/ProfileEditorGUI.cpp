#include <Arduino.h>
#include "ProfileEditorGUI.h" 

ProfileEditorGUI::ProfileEditorGUI(LCD1602 _display) {
  display = _display;
  screen = 0;
}

void ProfileEditorGUI::update(float val) {
  display.clear_all();
  display.print(SCREEN_TEXT[screen]);
  if (val == NULL) return;
  display.setRow(1);
  display.print(String(val).c_str());
}

bool ProfileEditorGUI::advance_screen(float val) {
  if (++screen >= MAX_SCREENS) {
    return true;
  }
  update(val);
  return false;
}

void ProfileEditorGUI::edit_param(float newVal) {
  update(newVal);
}