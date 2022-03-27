#include "PicoQuickKeyAscii.h"
#include <ctype.h>

PicoQuickKeyAscii::PicoQuickKeyAscii(char quickKey) :  _quickKey(toupper(quickKey))
{
}

bool PicoQuickKeyAscii::isQuickKey(uint8_t keycode, uint8_t modifiers, uint8_t ascii) {
  return toupper(ascii) == _quickKey;
}

