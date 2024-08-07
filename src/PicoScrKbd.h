#pragma once
#include "PicoWin.h"
#include <string>
#include <functional>

class PicoScrKbd : public PicoWin {

  std::function<void(uint32_t v)> _set;
  std::function<uint32_t()> _get;
  uint32_t _step;
  uint32_t _steps;
  std::string _chars;

 uint8_t _x=0;
 uint8_t _y=0;

  uint8_t Line0[10]={'1','2','3','4','5','6','7','8','9','0'};
  uint8_t Line1[10]={'Q','W','E','R','T','Y','U','I','O','P'};
  uint8_t Line3[10]={'A','S','D','F','G','H','J','K','L','<'};
  uint8_t Line4[10]={'x','Z','X','C','V','B','N','M','x','_'};
  
public:
  PicoScrKbd(int32_t x, int32_t y, int32_t w);

  void config(
    std::function<void(uint32_t v)> set,
    std::function<uint32_t()> get) 
  { 
    _set = set; 
    _get = get;
  }

  const char* getChars();
};
