#include "PicoSlider.h"
#include "PicoPen.h"

PicoSlider::PicoSlider(
  int32_t x, int32_t y, int32_t w, uint32_t step
) :
  PicoWin(x, y, w, 1),
  _step(step),
  _steps(w)
{
  for (uint32_t i = 0; i < _steps; ++i) _chars.append(" ");
  
  onKeydown([=](uint8_t keycode, uint8_t modifiers, uint8_t ascii) {
    if (keycode == 0x4e || ascii == 131) { // Up
        uint32_t v = _get();
        uint32_t m = (uint32_t)__mul_instruction(_step, _steps);
        uint32_t vn = v + _step;
        _set(vn > m ? m : vn);
        repaint();
        return false;
    } else if (keycode == 0x4b || ascii == 130) { // Down
        uint32_t v = _get();
        _set(v > _step ? v - _step : 0);
        repaint();
        return false;
    }
    return true;
  });

  onPaint([=](PicoPen *pen) {
    pen->printAt(0, 0, false, getChars());
  });
};

const char* PicoSlider::getChars() {
  uint32_t v = _get ? _get() : 0;
  for (uint32_t x = 0; x < _steps; ++x) {
    _chars[x] = (uint32_t)__mul_instruction(_step, x + 1) <= v ? '#' : '_';
  }
  return _chars.c_str();
}
