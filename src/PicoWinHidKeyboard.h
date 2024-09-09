#pragma once
#include "tusb.h"
#include "PicoDisplay.h"

class PicoWinHidKeyboard {
private:
  PicoDisplay *_display;
  bool _capslock;
  uint8_t old_value;
  uint32_t repeat_timer;
  unsigned char curr_hidKeyCode;
  uint8_t curr_modifiers;
  uint8_t curr_ascii;

  bool keyPressed(uint8_t keycode, uint8_t modifiers, uint8_t ascii);
  void handleJoystickPress(const uint8_t value, const uint8_t old_value);
  void cancelRepeat();
public:
  PicoWinHidKeyboard(PicoDisplay *display);

  int processHidReport(hid_keyboard_report_t const *report, hid_keyboard_report_t const *prev_report);
  int processJoystick(uint8_t value);
  int processJoystickMenuEnter(uint8_t value);
  void processKeyRepeat();
};
