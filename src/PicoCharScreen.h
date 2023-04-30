#pragma once
#include "PicoRectXYWH.h"

class PicoCharScreen {
private:
  uint16_t *_chars;
  int32_t _s;
  PicoRectXYWH _rect;
  
public:

  PicoCharScreen(uint16_t *chars, uint32_t w, uint32_t h);
  
  inline void set(int32_t x, int32_t y, uint16_t c) {
    _chars[x +(y * _rect._w)] = c;
  }

  inline uint32_t frameWidthPixels() {
    return _rect.w() << 3;
  }

  inline uint32_t frameHeightPixels() {
    return _rect.h() << 3;
  }

  inline uint16_t *chars() {
    return _chars;
  }

  void clear();

  PicoRectXYWH *rect() { return &_rect; } 
};
