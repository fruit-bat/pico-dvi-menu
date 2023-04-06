#pragma once

#include "PicoStaticScreen.h"

extern "C" { 
  void __not_in_flash_func(pcw_prepare_vga332_scanline_80)(uint32_t* buf, int y, uint32_t frames);
  void pcw_init_vga332_renderer();
}
