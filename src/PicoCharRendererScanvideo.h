#pragma once

#include "PicoStaticScreen.h"
#include "pico/scanvideo.h"

extern "C" { 
  void __not_in_flash_func(pcw_prepare_scanvideo_scanline_80)(
    struct scanvideo_scanline_buffer *scanline_buffer, 
    int y,
    uint32_t frames
  );
}

