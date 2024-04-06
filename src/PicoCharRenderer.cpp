#include "PicoCharRenderer.h"
#include <cstring>

extern "C" {
#include "dvi.h"
#include "dvi_serialiser.h"
#include "common_dvi_pin_configs.h"
#include "tmds_encode.h"
}

uint pcw_prepare_scanline_80(struct dvi_inst *dvi0, const uint y, const uint ys, const uint frames) {
  static uint32_t scanbuf32[(PCS_COLS + 3) >> 2];
  uint8_t *scanbuf = (uint8_t *)&scanbuf32;
  const uint16_t m = (frames >> 5) & 1;
  const uint cr = y & 7;
  for (uint i = 0; i < PCS_COLS; ++i) {
    const uint16_t ca = charScreen[i + ys];
    const uint8_t cf = charFont[((ca & 0xff) << 3) + cr];
    // bit 8 inverse video
    // bit 9 flash
    const uint16_t z = ((ca >> 8) ^ ((ca >> 9) & m)) & 1; 
    scanbuf[i] = cf ^ __mul_instruction(z, 0xff);
  }
  uint32_t *tmdsbuf;
  queue_remove_blocking(&dvi0->q_tmds_free, &tmdsbuf);
  tmds_encode_1bpp(scanbuf32, tmdsbuf, PCS_FRAME_WIDTH);
#if !DVI_MONOCHROME_TMDS
  uint16_t *p = (uint16_t *)tmdsbuf;
  memcpy(p + PCS_FRAME_WIDTH, p, PCS_FRAME_WIDTH << 1);
  p+= PCS_FRAME_WIDTH;
  memcpy(p + PCS_FRAME_WIDTH, p, PCS_FRAME_WIDTH << 1);
#endif
  queue_add_blocking(&dvi0->q_tmds_valid, &tmdsbuf);
  return PCS_COLS;
}

uint pcw_prepare_blankline_80(struct dvi_inst *dvi0, const uint frames) {
  static uint32_t scanbuf32[(PCS_COLS + 3) >> 2];
  uint8_t *scanbuf = (uint8_t *)&scanbuf32;
  for (uint i = 0; i < PCS_COLS; ++i) {
    const uint8_t cf = 0;
    scanbuf[i] = cf ;
  }
  uint32_t *tmdsbuf;
  queue_remove_blocking(&dvi0->q_tmds_free, &tmdsbuf);
  tmds_encode_1bpp(scanbuf32, tmdsbuf, PCS_FRAME_WIDTH);
#if !DVI_MONOCHROME_TMDS
  uint16_t *p = (uint16_t *)tmdsbuf;
  memcpy(p + PCS_FRAME_WIDTH, p, PCS_FRAME_WIDTH << 1);
  p+= PCS_FRAME_WIDTH;
  memcpy(p + PCS_FRAME_WIDTH, p, PCS_FRAME_WIDTH << 1);
#endif
  queue_add_blocking(&dvi0->q_tmds_valid, &tmdsbuf);
  return PCS_COLS;
}

