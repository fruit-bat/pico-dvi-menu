#include "PicoCharRendererSt7789.h"

#ifdef INVERSE_LCD
#define LCD_COL(c) (0xf-c##UL)
#else
#define LCD_COL(c) (c##UL)
#endif
#define LCD_RGB_444_POS(r,g,b) ((r<<8)|(g<<4)|b)
#define LCD_RGB_444(r,g,b) LCD_RGB_444_POS(LCD_COL(r),LCD_COL(g), LCD_COL(b))
#define LCD_BG LCD_RGB_444(0x0,0x0,0x0)
#define LCD_FG LCD_RGB_444(0xf,0xf,0xf)
#define LCD_WORD(a,b) ((a << 8) | (b << 20))
#define LCD_CW0 LCD_WORD(LCD_BG, LCD_BG)
#define LCD_CW1 LCD_WORD(LCD_FG, LCD_BG)
#define LCD_CW2 LCD_WORD(LCD_BG, LCD_FG)
#define LCD_CW3 LCD_WORD(LCD_FG, LCD_FG)

static uint32_t colour_words[4] = {
  LCD_CW0,
  LCD_CW1,
  LCD_CW2,
  LCD_CW3
};

void __not_in_flash_func(pcw_prepare_st7789_scanline)(uint32_t* buf, int y, uint32_t frames) {
  const uint16_t m = (frames >> 5) & 1;
  int np = 0;
  int li = __mul_instruction(y >> 3, PCS_COLS);
  int yb = y & 7;
  // Assume even number of character columns.
  // charScreen must be 32bit word aligned.
  uint32_t *cl = (uint32_t *)(charScreen + li);
  for (int x = 0; x < (LCD_COLS >> 1); ++x) {
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
  for (int x = 0; x < (LCD_COLS >> 1); ++x) {
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
