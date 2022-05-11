#include "PicoCharRendererVga.h"
#include "PicoFontCushion.h"


__attribute__((aligned(4))) static uint16_t charScreen[PCS_COLS * PCS_ROWS];
static uint8_t charFont[256 * 8]; // TODO too big ?
static PicoCharScreen picoCharScreen(charScreen, PCS_COLS, PCS_ROWS);
static uint32_t nibblebits[16];

void __not_in_flash_func(pcw_prepare_vga332_scanline_80)(uint32_t* buf, int y, uint32_t frames) {
  const uint16_t m = (frames >> 5) & 1;
  int np = 0;
  int li = __mul_instruction(y >> 3, PCS_COLS);
  int yb = y & 7;
  // Assume even number of character columns.
  // charScreen must be 32bit word aligned.
  uint32_t *cl = (uint32_t *)(charScreen + li);
  for (int x = 0; x < (PCS_COLS >> 1); ++x) {
    uint32_t ch = *cl++;
    uint16_t z;
    int b;
    z = ((ch >> 8) ^ ((ch >> 9) & m)) & 1; 
    b = charFont[(ch & 0xff) + __mul_instruction(yb, PCS_COLS)] ^ __mul_instruction(z, 0xff);
    buf[np++] = nibblebits[b & 0xf];
    buf[np++] = nibblebits[b >> 4];
    ch = ch >> 16;
    z = ((ch >> 8) ^ ((ch >> 9) & m)) & 1; 
    b = charFont[(ch & 0xff) + __mul_instruction(yb, PCS_COLS)] ^ __mul_instruction(z, 0xff);
    buf[np++] = nibblebits[b & 0xf];
    buf[np++] = nibblebits[b >> 4];
  }
}

void pcw_init_renderer() {
  memcpy(&charFont[32*8], PicoFontCushion, sizeof(PicoFontCushion));
  
  for(int i = 0; i < 16; ++i) {
    uint32_t a = 0;
    for(int j = 0; j < 4; ++j) {
      if (i & (1 << j)) a |= (0xff << (j << 3));
    }
    nibblebits[i] = a;
  }  
}

PicoCharScreen *pcw_screen() {
  return &picoCharScreen;
}
