#include "PicoExplorer.h"
#include "PicoPen.h"

PicoExplorer::PicoExplorer(FatFsDirCache* cache, int32_t x, int32_t y, int32_t w, int32_t r, int32_t rh) :
  PicoWin(x, y, w, r * rh),
  _cache(cache),
  _i(0), _r(r), _rh(rh)
{
  onKeydown([=](uint8_t keycode, uint8_t modifiers, uint8_t ascii) {
    
    switch(ascii) {
      case 32: case 13: {
        if (_i >= 0 && _i < optionCount()) {
          toggleSelection(_i);
        }
        return false;
      }
      case 129: {
        _i -= _r;
        while (_i < 0) _i += optionCount();
        repaint();
        return false;
      }
      case 128: {
        _i += r;
        while (_i >= optionCount()) _i -= optionCount();
        repaint();
        return false;
      }
      case 131: {
        --_i;
        while (_i < 0) _i += optionCount();
        repaint();
        return false;
      }
      case 130: {
        ++_i;
        while (_i >= optionCount()) _i -= optionCount();
        repaint();
        return false;
      }
    }
    
    return true;
  });
  
  onPaint([=](PicoPen *pen) {
    const int32_t s0 = _i - (_r >> 1);  
    const int32_t s1 = optionCount() - _r;
    const int32_t s = s0 <= 0 || s1 <= 0 ? 0 : (s0 > s1 ? s1 : s0);
    int32_t y = 0;
    for(int32_t r = 0; r < _r; ++r) {
      const int32_t i = r + s;
      PicoPen rpen(pen, 0, y, ww(), 1);
      if (!rpen.clipped()) {
        if (i < optionCount()) {
          paintRow(&rpen, r + s == _i, i);
        }
        else {
          rpen.clear();
        }
      }
      y += _rh;
    }
  });
  
  onClear([=](PicoPen *pen) {});
}

void PicoExplorer::paintRow(PicoPen *pen, bool focused, int32_t i) {
  FILINFO finfo;
  const char *fname = _cache->read(i, &finfo) ? finfo.fname : "????????.???";
  pen->printAtF(0, 0, false, "%c %c %s", (focused ? '>' : ' '), (finfo.fattrib & AM_DIR ? '*' : ' '), fname);
  for(int32_t i = strlen(fname) + 4; i < pen->cw(); ++i) pen->printAt(i, 0, false, " ");
}

void PicoExplorer::toggleSelection(int32_t i) {
  if (_toggle) { 
    FILINFO finfo;
    if (_cache->read(i, &finfo)) {
      if (finfo.fattrib & AM_DIR) {
        // TODO open the folder
      }
      else {
        _toggle(&finfo, i);
      }
    }
  }
}

void PicoExplorer::focus(int32_t i) {
  if (_i == i) return;
  if (i >= 0) {
    _i = i;
    if (i >= optionCount()) {
      i = optionCount() - 1;
    }
  }
  repaint();
}

