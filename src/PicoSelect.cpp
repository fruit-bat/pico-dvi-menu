#include "PicoSelect.h"
#include "PicoPen.h"
// #include <pico/printf.h>

PicoSelect::PicoSelect(int32_t x, int32_t y, int32_t w, int32_t r, int32_t rh) :
  PicoWin(x, y, w, r * rh),
  _i(0), _r(r), _rh(rh),
  _quickKeys(false)
{
  onKeydown([=](uint8_t keycode, uint8_t modifiers, uint8_t ascii) {
    // printf("Select key pressed %d %d %d %c\n", keycode, modifiers, ascii, ascii);
    // up    82 
    // down  81
    // left  80 
    // right 79
    if (_quickKeys) {
      for (int i = 0; i < optionCount(); ++ i) {
        PicoOption * option = _options[i];
        if (option->isQuickKey(keycode, modifiers, ascii)) {
          // printf("Toggling %d %d %d %c\n", keycode, modifiers, ascii, ascii);
          _i = i;
          toggleSelection(i);
          return false;
        }
      }
    }

    switch(keycode) {
      case HID_KEY_PAGE_UP: { // Page up
        if (_i > _r) { 
          _i -= _r;
          repaint();
        }
        return false;
      }
      case HID_KEY_PAGE_DOWN: { // Page Down
        if (_i + _r < optionCount()) {
          _i += r;
          repaint();
        }
        return false;
      }
      default: break;
    }
    
    switch(ascii) {
      //case 32:  // Space - Don't use by default - upsets the membrane keyboard layout
      case 13:  // Enter
      case 128: // Right
      { // Select
        if (_i >= 0 && _i < optionCount()) {
          toggleSelection(_i);
        }
        return false;
      }
      case 131: { // Up
        if (_i > 0) { 
          --_i;
          repaint();
        }
        return false;
      }
      case 130: { // Down
        if (_i + 1 < optionCount()) {
          ++_i;
          repaint();
        }
        return false;
      }
    }
    
    return true;
  });
  
  onPaint([=](PicoPen *pen) {
    const int32_t s0 = _i - (_r >> 1);  
    const int32_t s1 = optionCount() - _r;
    const int32_t s = s0 <= 0 || s1 <= 0 ? 0 : (s0 > s1 ? s1 : s0);
    int32_t y = 0;
    for(int32_t r = 0; r < _r; ++r) {
      const int32_t i = r + s;
      PicoPen rpen(pen, 0, y, ww(), 1);
      if (!rpen.clipped()) {
        if (i < optionCount()) {
          paintRow(&rpen, r + s == _i, i);
        }
        else {
          rpen.clear();
        }
      }
      y += _rh;
    }
  });
  
  onClear([=](PicoPen *pen) {});
}

void PicoSelect::addOption(PicoOption *option) {
  _options.push_back(option);
  repaint();
}

void PicoSelect::clearOptions() {
  _options.clear();
  _i = 0;
  repaint();
}

void PicoSelect::deleteOptions() {
  for (auto option : _options) {
    delete option;
  }
  clearOptions();
}

void PicoSelect::paintRow(PicoPen *pen, bool focused, int32_t i) {
  _options[i]->paintRow(pen, focused, i, ww());
}

void PicoSelect::toggleSelection(int32_t i) {
  PicoOption *option = _options[i];
  option->toggle();
  if (_toggle) _toggle(option, i);
  repaint();
}

void PicoSelect::focus(int32_t i) {
  if (_i == i) return;
  if (i >= 0) {
    _i = i;
    if (i >= optionCount()) {
      i = optionCount() - 1;
    }
  }
  repaint();
}

