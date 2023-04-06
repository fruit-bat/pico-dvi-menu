#include "PicoCharRendererSt7789.h"

static uint32_t colour_words[4] = {
  0x00000000,
  0x000fff00,
  0xfff00000,
  0xffffff00,
};

void __not_in_flash_func(pcw_prepare_st7789_scanline)(uint32_t* buf, int y, uint32_t frames) {
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
    b = charFont[((ch & 0xff) << 3) + yb] ^ __mul_instruction(z, 0xff);
    buf[np++] = colour_words[(b >> 6) & 3];
    buf[np++] = colour_words[(b >> 4) & 3];
    buf[np++] = colour_words[(b >> 2) & 3];
    buf[np++] = colour_words[b & 3];
    ch = ch >> 16;
    z = ((ch >> 8) ^ ((ch >> 9) & m)) & 1; 
    b = charFont[((ch & 0xff) << 3) + yb] ^ __mul_instruction(z, 0xff);
    buf[np++] = colour_words[(b >> 6) & 3];
    buf[np++] = colour_words[(b >> 4) & 3];
    buf[np++] = colour_words[(b >> 2) & 3];
    buf[np++] = colour_words[b & 3];
  }
}

static inline void st7789_lcd_put_rgb444(PIO pio, uint sm, uint32_t x) {
    while (pio_sm_is_tx_fifo_full(pio, sm));
    *(volatile uint32_t*)&pio->txf[sm] = x;
}

void __not_in_flash_func(pcw_send_st7789_scanline)(PIO pio, uint sm, int y, uint32_t frames) {
  const uint16_t m = (frames >> 5) & 1;
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
    b = charFont[((ch & 0xff) << 3) + yb] ^ __mul_instruction(z, 0xff);
    st7789_lcd_put_rgb444(pio, sm, colour_words[(b >> 6) & 3]);
    st7789_lcd_put_rgb444(pio, sm, colour_words[(b >> 4) & 3]);
    st7789_lcd_put_rgb444(pio, sm, colour_words[(b >> 2) & 3]);
    st7789_lcd_put_rgb444(pio, sm, colour_words[b & 3]);
    ch = ch >> 16;
    z = ((ch >> 8) ^ ((ch >> 9) & m)) & 1; 
    b = charFont[((ch & 0xff) << 3) + yb] ^ __mul_instruction(z, 0xff);
    st7789_lcd_put_rgb444(pio, sm, colour_words[(b >> 6) & 3]);
    st7789_lcd_put_rgb444(pio, sm, colour_words[(b >> 4) & 3]);
    st7789_lcd_put_rgb444(pio, sm, colour_words[(b >> 2) & 3]);
    st7789_lcd_put_rgb444(pio, sm, colour_words[b & 3]);
  }
}

void __not_in_flash_func(pcw_send_st7789_frame)(
  PIO pio,
  uint sm, 
  uint32_t frame
) {
  for (uint32_t y = 0; y < 240; ++y) pcw_send_st7789_scanline(pio, sm, y, frame);
}
