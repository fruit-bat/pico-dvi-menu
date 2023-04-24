#pragma once
#include "PicoWin.h"
#include <string>
#include <functional>

class PicoSlider : public PicoWin {

  std::function<void(uint32_t v)> _set;
  std::function<uint32_t()> _get;
  uint32_t _step;
  uint32_t _steps;
  char *_chars;

public:
  PicoSlider(int32_t x, int32_t y, int32_t w, uint32_t step);

  void config(
    std::function<void(uint32_t v)> set,
    std::function<uint32_t()> get) 
  { 
    _set = set; 
    _get = get;
  }

  const char* getChars();
};
