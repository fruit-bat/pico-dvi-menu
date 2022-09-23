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
#include "pico/scanvideo.h"

extern "C" { 
  void __not_in_flash_func(pcw_prepare_scanvideo_scanline_80)(
    struct scanvideo_scanline_buffer *scanline_buffer, 
    int y,
    uint32_t frames
  );
  
  void pcw_init_renderer();
}

PicoCharScreen *pcw_screen();

