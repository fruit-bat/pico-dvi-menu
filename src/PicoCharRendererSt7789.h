#pragma once

#ifndef PCS_COLS
#define PCS_COLS 80
#endif

#ifndef PCS_ROWS
#define PCS_ROWS 30
#endif

#ifndef PCS_FRAME_WIDTH
#define PCS_FRAME_WIDTH (8 * PCS_COLS)
#endif

#ifndef PCS_FRAME_HEIGHT
#define PCS_FRAME_HEIGHT (8 * PCS_ROWS)
#endif

#include "tusb.h"
#include "PicoCharScreen.h"
#include <hardware/pio.h>

extern "C" { 
  void __not_in_flash_func(pcw_prepare_st7789_scanline)(uint32_t* buf, int y, uint32_t frames);
  void __not_in_flash_func(pcw_send_st7789_scanline)(PIO pio, uint sm, int y, uint32_t frames);
  void __not_in_flash_func(pcw_send_st7789_frame)(PIO pio, uint sm, uint32_t frame);
  void pcw_init_renderer();
}

PicoCharScreen *pcw_screen();

