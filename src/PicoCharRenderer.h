#pragma once
#include "PicoStaticScreen.h"

extern "C" { 
  uint __not_in_flash_func(pcw_prepare_scanline_80)(struct dvi_inst *dvi0, const uint y, const uint ys, const uint frames);
}
