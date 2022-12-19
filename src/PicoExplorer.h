#pragma once

#include "PicoWin.h"
#include "PicoOption.h"
#include <vector>
#include <functional>
#include "FatFsDirCache.h"

class PicoExplorer : public PicoWin {

private:
  FatFsDirCache* _cache;

  std::function<void(FILINFO *info, int32_t i)> _toggle;

  int32_t _i, _r, _rh;

  void paintRow(PicoPen *pen, bool focused, int32_t i);
  int32_t optionCount() { return (int32_t)_cache->size(); }
  void toggleSelection(int32_t i);
  void toggleSelection(int32_t i, FILINFO* finfo);

public:

  PicoExplorer(FatFsDirCache* cache, int32_t x, int32_t y, int32_t w, int32_t r, int32_t rh);
  void onToggle(std::function<void(FILINFO *info, int32_t i)> toggle) { _toggle = toggle; }
  void focus(int32_t i);
  int32_t focus() { return _i; };
  void next(std::function<bool(FILINFO *info)> filter, int d);
};
