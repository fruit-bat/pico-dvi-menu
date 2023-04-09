#pragma once

#include "PicoStaticScreen.h"
#include <hardware/pio.h>

#ifndef LCD_COLS
#define LCD_COLS 40
#endif

extern "C" { 
  void __not_in_flash_func(pcw_prepare_st7789_scanline)(uint32_t* buf, int y, uint32_t frames);
  void __not_in_flash_func(pcw_send_st7789_scanline)(PIO pio, uint sm, int y, uint32_t frames);
  void __not_in_flash_func(pcw_send_st7789_frame)(PIO pio, uint sm, uint32_t frame);
}

