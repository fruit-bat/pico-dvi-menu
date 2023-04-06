#pragma once

#include "PicoCharScreen.h"

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

extern "C" __attribute__((aligned(4))) uint16_t charScreen[];
extern "C" uint8_t charFont[];
extern "C" PicoCharScreen picoCharScreen;
extern "C" PicoCharScreen *pcw_screen();
extern "C" void pcw_init_renderer();