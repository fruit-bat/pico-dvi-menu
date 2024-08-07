#include "PicoScrKbd.h"
#include "PicoPen.h"

PicoScrKbd::PicoScrKbd(
  int32_t x, int32_t y, int32_t w
) :
  PicoWin(x, y, 21, 4),
{
  for (uint32_t i = 0; i < _steps; ++i) _chars.append(" ");
  
  onKeydown([=](uint8_t keycode, uint8_t modifiers, uint8_t ascii) {
    if (ascii == 131) { // Up
        if (_x==0) _x=3; else _x--;
        repaint();
        return false;
    } else if (ascii == 130) { // Down
    if (_x==3) _x=0; else _x++;
        repaint();
        return false;
    } else if (ascii == 129) { // Left
    if (_y==0) _y=9; else _y--;
        repaint();
        return false;
    } else if (ascii == 128) { // Left
    if (_y==9) _y=0; else _y++;
        repaint();
        return false;
    } else  if (ascii==)

    return true;
  });

  onPaint([=](PicoPen *pen) {
    pen->printAt(0, 0, false, getChars());
  });
};

const char* PicoScrKbd::getChars() {
  uint32_t v = _get ? _get() : 0;
  for (uint32_t x = 0; x < _steps; ++x) {
    _chars[x] = (uint32_t)__mul_instruction(_step, x + 1) <= v ? '#' : '_';
  }
  return _chars.c_str();
}
