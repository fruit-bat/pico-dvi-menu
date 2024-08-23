#include "PicoCharRendererScanvideo.h"
#include "PicoFontCushion.h"
#include "pico/scanvideo.h"
#include "pico/scanvideo/composable_scanline.h"

#define VGA_RGB_555(r,g,b) ((r##UL<<0)|(g##UL<<6)|(b##UL << 11))
#define VGA_BGYR_1111(r,g,b,y) ((y##UL<<2)|(r##UL<<3)|(g##UL<<1)|b##UL)
#define VGA_RGBY_1111(r,g,b,y) ((y##UL<<3)|(r##UL<<2)|(g##UL<<1)|b##UL)
#define VGA_RGB_332(r,g,b) ((r##UL<<5)|(g##UL<<2)|b##UL)
#define VGA_RGB_222(r,g,b) ((r##UL<<4)|(g##UL<<2)|b##UL)

#if defined(VGA_ENC_BGYR_1111)
#define COLOR_BLACK VGA_BGYR_1111(0,0,0,0)
#define COLOR_BLUE VGA_BGYR_1111(0,0,1,0)
#define COLOR_RED VGA_BGYR_1111(1,0,0,0)
#define COLOR_MAGENTA VGA_BGYR_1111(1,0,1,0)
#define COLOR_GREEN VGA_BGYR_1111(0,1,0,0)
#define COLOR_CYAN VGA_BGYR_1111(0,1,1,0)
#define COLOR_YELLOW VGA_BGYR_1111(1,1,0,0)
#define COLOR_WHITE VGA_BGYR_1111(1,1,1,0)
#define COLOR_BRIGHT_BLUE VGA_BGYR_1111(0,0,1,1)
#define COLOR_BRIGHT_RED VGA_BGYR_1111(1,0,0,1)
#define COLOR_BRIGHT_MAGENTA VGA_BGYR_1111(1,0,1,1)
#define COLOR_BRIGHT_GREEN VGA_BGYR_1111(0,1,0,1)
#define COLOR_BRIGHT_CYAN VGA_BGYR_1111(0,1,1,1)
#define COLOR_BRIGHT_YELLOW VGA_BGYR_1111(1,1,0,1)
#define COLOR_BRIGHT_WHITE VGA_BGYR_1111(1,1,1,1)
#elif defined(VGA_ENC_RGBY_1111)
#define COLOR_BLACK VGA_RGBY_1111(0,0,0,0)
#define COLOR_BLUE VGA_RGBY_1111(0,0,1,0)
#define COLOR_RED VGA_RGBY_1111(1,0,0,0)
#define COLOR_MAGENTA VGA_RGBY_1111(1,0,1,0)
#define COLOR_GREEN VGA_RGBY_1111(0,1,0,0)
#define COLOR_CYAN VGA_RGBY_1111(0,1,1,0)
#define COLOR_YELLOW VGA_RGBY_1111(1,1,0,0)
#define COLOR_WHITE VGA_RGBY_1111(1,1,1,0)
#define COLOR_BRIGHT_BLUE VGA_RGBY_1111(0,0,1,1)
#define COLOR_BRIGHT_RED VGA_RGBY_1111(1,0,0,1)
#define COLOR_BRIGHT_MAGENTA VGA_RGBY_1111(1,0,1,1)
#define COLOR_BRIGHT_GREEN VGA_RGBY_1111(0,1,0,1)
#define COLOR_BRIGHT_CYAN VGA_RGBY_1111(0,1,1,1)
#define COLOR_BRIGHT_YELLOW VGA_RGBY_1111(1,1,0,1)
#define COLOR_BRIGHT_WHITE VGA_RGBY_1111(1,1,1,1)
#elif defined(VGA_ENC_RGB_222)
#define COLOR_BLACK VGA_RGB_222(0,0,0)
#define COLOR_BLUE VGA_RGB_222(0,0,2)
#define COLOR_RED VGA_RGB_222(2,0,0)
#define COLOR_MAGENTA VGA_RGB_222(2,0,2)
#define COLOR_GREEN VGA_RGB_222(0,2,0)
#define COLOR_CYAN VGA_RGB_222(0,2,2)
#define COLOR_YELLOW VGA_RGB_222(2,2,0)
#define COLOR_WHITE VGA_RGB_222(2,2,2)
#define COLOR_BRIGHT_BLUE VGA_RGB_222(0,0,3)
#define COLOR_BRIGHT_RED VGA_RGB_222(3,0,0)
#define COLOR_BRIGHT_MAGENTA VGA_RGB_222(3,0,3)
#define COLOR_BRIGHT_GREEN VGA_RGB_222(0,3,0)
#define COLOR_BRIGHT_CYAN VGA_RGB_222(0,3,3)
#define COLOR_BRIGHT_YELLOW VGA_RGB_222(3,3,0)
#define COLOR_BRIGHT_WHITE VGA_RGB_222(3,3,3)
#elif defined(VGA_ENC_RGB_332)
#define COLOR_BLACK VGA_RGB_332(0,0,0)
#define COLOR_BLUE VGA_RGB_332(0,0,2)
#define COLOR_RED VGA_RGB_332(5,0,0)
#define COLOR_MAGENTA VGA_RGB_332(5,0,2)
#define COLOR_GREEN VGA_RGB_332(0,5,0)
#define COLOR_CYAN VGA_RGB_332(0,5,2)
#define COLOR_YELLOW VGA_RGB_332(5,5,0)
#define COLOR_WHITE VGA_RGB_332(5,5,2)
#define COLOR_BRIGHT_BLUE VGA_RGB_332(0,0,3)
#define COLOR_BRIGHT_RED VGA_RGB_332(7,0,0)
#define COLOR_BRIGHT_MAGENTA VGA_RGB_332(7,0,3)
#define COLOR_BRIGHT_GREEN VGA_RGB_332(0,7,0)
#define COLOR_BRIGHT_CYAN VGA_RGB_332(0,7,3)
#define COLOR_BRIGHT_YELLOW VGA_RGB_332(7,7,0)
#define COLOR_BRIGHT_WHITE VGA_RGB_332(7,7,3)
#else    
#define COLOR_BLACK VGA_RGB_555(0, 0, 0 )
#define COLOR_BLUE VGA_RGB_555(0, 0, 20)
#define COLOR_RED VGA_RGB_555(20,0, 0 )
#define COLOR_MAGENTA VGA_RGB_555(20,0, 20)
#define COLOR_GREEN VGA_RGB_555(0, 20,0 )
#define COLOR_CYAN VGA_RGB_555(0, 20,20)
#define COLOR_YELLOW VGA_RGB_555(20,20,0 )
#define COLOR_WHITE VGA_RGB_555(20,20,20)
#define COLOR_BRIGHT_BLUE VGA_RGB_555(0, 0, 31)
#define COLOR_BRIGHT_RED VGA_RGB_555(31,0, 0 )
#define COLOR_BRIGHT_MAGENTA VGA_RGB_555(31,0, 31)
#define COLOR_BRIGHT_GREEN VGA_RGB_555(0, 31,0 )
#define COLOR_BRIGHT_CYAN VGA_RGB_555(0, 31,31)
#define COLOR_BRIGHT_YELLOW VGA_RGB_555(31,31,0 )
#define COLOR_BRIGHT_WHITE VGA_RGB_555(31,31,31)
#endif

#ifndef PICO_MENU_BACKGROUND_COLOR
#define PICO_MENU_BACKGROUND_COLOR COLOR_BLACK
#endif
#ifndef PICO_MENU_FOREGROUND_COLOR
#define PICO_MENU_FOREGROUND_COLOR COLOR_BRIGHT_WHITE
#endif

static uint32_t bgc = PICO_MENU_BACKGROUND_COLOR;
static uint32_t fgc = PICO_MENU_FOREGROUND_COLOR;

static uint32_t bbcw[4] {
  bgc | (bgc << 16),
  bgc | (fgc << 16),
  fgc | (bgc << 16),
  fgc | (fgc << 16)
};

static uint32_t cw[2] {
  bgc,
  fgc
};

static uint32_t cs[2] {
  bgc << 16,
  fgc << 16
};

void __not_in_flash_func(pcw_prepare_scanvideo_scanline_80)(
    struct scanvideo_scanline_buffer *scanline_buffer, 
    int y,
    uint32_t frames)
{
  uint32_t* buf = scanline_buffer->data;

  
  const uint16_t m = (frames >> 5) & 1;

  int li = __mul_instruction(y >> 3, PCS_COLS);
  int yb = y & 7;
  // Assume even number of character columns.
  // charScreen must be 32bit word aligned.
  uint32_t *cl = (uint32_t *)(charScreen + li);
  for (int x = 0; x < (PCS_COLS >> 1); ++x) {
    uint32_t ch = *cl++;
    uint16_t z = ((ch >> 8) ^ ((ch >> 9) & m)) & 1; 
    int p = charFont[((ch & 0xff) << 3) + yb] ^ __mul_instruction(z, 0xff);
    
    *buf++ = COMPOSABLE_RAW_RUN | cs[(p >> 7) & 1];
    *buf++ = (16 - 3) | cs[(p >> 6) & 1];
    *buf++ = bbcw[(p >> 4) & 3];
    *buf++ = bbcw[(p >> 2) & 3];
    *buf++ = bbcw[p & 3];
    
    ch = ch >> 16;
    z = ((ch >> 8) ^ ((ch >> 9) & m)) & 1; 
    p = charFont[((ch & 0xff) << 3) + yb] ^ __mul_instruction(z, 0xff);
    
    *buf++ = bbcw[(p >> 6) & 3];
    *buf++ = bbcw[(p >> 4) & 3];
    *buf++ = bbcw[(p >> 2) & 3];
    *buf++ = bbcw[p & 3];
  }

  // end with a black pixel
  *buf++ = COMPOSABLE_RAW_1P;
  *buf++ = COMPOSABLE_EOL_SKIP_ALIGN;

  scanline_buffer->data_used = buf - scanline_buffer->data;
  scanline_buffer->status = SCANLINE_OK;  
}
