#include "PicoStaticScreen.h"
#include "PicoFontCushion.h"
#include <cstring>

__attribute__((aligned(4))) uint16_t charScreen[PCS_COLS * PCS_ROWS];
uint8_t charFont[256 * 8];
PicoCharScreen picoCharScreen(charScreen, PCS_COLS, PCS_ROWS);

void pcw_init_renderer() {
  memcpy(&charFont[32*8], PicoFontCushion, sizeof(PicoFontCushion));

  for (int x = 0; x < PCS_COLS; ++x) {
    for (int y = 0; y < PCS_ROWS; ++y) {
      int i = x + (y * PCS_COLS);
      charScreen[i] = 32;
    }
  }  
}

PicoCharScreen *pcw_screen() {
  return &picoCharScreen;
}
