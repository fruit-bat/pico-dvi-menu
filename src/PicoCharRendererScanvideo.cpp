#include "PicoCharRendererScanvideo.h"
#include "PicoFontCushion.h"
#include "pico/scanvideo.h"
#include "pico/scanvideo/composable_scanline.h"

__attribute__((aligned(4))) static uint16_t charScreen[PCS_COLS * PCS_ROWS];
static uint8_t charFont[256 * 8]; // TODO too big ?
static PicoCharScreen picoCharScreen(charScreen, PCS_COLS, PCS_ROWS);

#define VGA_RGB_555(r,g,b) ((r##UL<<0)|(g##UL<<6)|(b##UL << 11))

static uint32_t bgc = 0;
static uint32_t fgc = VGA_RGB_555(10, 28, 10);

static uint32_t bbcw[4] {
  bgc | (bgc << 16),
  bgc | (fgc << 16),
  fgc | (bgc << 16),
  fgc | (fgc << 16)
};

void __not_in_flash_func(pcw_prepare_scanvideo_scanline_80)(
    struct scanvideo_scanline_buffer *scanline_buffer, 
    int y,
    uint32_t frames)
{
  uint32_t* buf = scanline_buffer->data;
  
  
  uint32_t cw[2], cs[2];
  cw[0] = bgc;
  cw[1] = fgc;
  cs[0] = cw[0] << 16;
  cs[1] = cw[1] << 16;
  
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
