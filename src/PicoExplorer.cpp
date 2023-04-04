#include "PicoExplorer.h"
#include "PicoPen.h"
#include "FatFsSpiOutputStream.h"
#include "FatFsSpiInputStream.h"

// #define DEBUG_PICO_EXPLORER

#ifdef DEBUG_PICO_EXPLORER
#define DBG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif

PicoExplorer::PicoExplorer(SdCardFatFsSpi* sdCard, FatFsFilePath* root, int32_t x, int32_t y, int32_t w, int32_t r, int32_t rh) :
  PicoWin(x, y, w, r * rh),
  _sdCard(sdCard),
  _cache(sdCard),
  _path(root),
  _i(0), _r(r), _rh(rh)
{
  _cache.filter([] (const char *fname) {
    return fname[0] != '.';
  });
  load();
  
  onKeydown([=](uint8_t keycode, uint8_t modifiers, uint8_t ascii) {
    
    DBG_PRINTF("PicoExplorer: key press keycode %2.2X, modifiers %2.2X, ascii %2.2X\n", keycode, modifiers, ascii);

    switch(keycode) {
      case 0x4b: {
        pageUp();
        return false;
      }
      case 0x4e: {
        pageDown();
        return false;
      }
      default:
        break;      
    }

    switch(ascii) {
      // 1=DEL 2=REN 3=CPY 4=PST 5=REF
      case 0x31: { // 1 - Delete
        action(_i, onDeleteFile);
        return false;
      }
      case 0x32: { // 2 - Rename
        action(_i, onRenameFile);
        return false;
      }
      case 0x33: { // 3 - Copy
        action(_i, [&](FILINFO *finfo, int32_t i) { copy(finfo, i); });
        return false;
      }
      case 0x34: { // 4 - Paste
        onPasteFile(_copiedName.c_str());
        return false;
      }
      case 0x35: { // 5 - Refresh
        if (onRefresh) onRefresh(); else reload();
        return false;
      }
      case 27:  // Escape
      case 129: // Left
      {
        if (_path.size()) {
          _path.pop();
          load();
          return false;
        }
        break;
      }
      case 32:  // Space
      case 13:  // Enter
      case 128: // Right
      {
        if (_i >= 0 && _i < optionCount()) {
          toggleSelection(_i);
        }
        return false;
      }
      case 131: // Up
      {
        --_i;
        while (_i < 0) _i += optionCount();
        repaint();
        return false;
      }
      case 130: // Down
      {
        ++_i;
        while (_i >= optionCount()) _i -= optionCount();
        repaint();
        return false;
      }
      default:
        break;
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

void PicoExplorer::copy(FILINFO *finfo, int32_t i) {
  _copiedPath.clear();
  _copiedName = finfo->fname;
  FatFsFilePath path(&_path, finfo->fname);
  path.appendTo(_copiedPath);
}

bool PicoExplorer::pasteFile(const char *file) {
  std::string topath;
  FatFsFilePath path(&_path, file);
  path.appendTo(topath);
  FatFsSpiOutputStream os(_sdCard, topath.c_str());
  FatFsSpiInputStream is(_sdCard, _copiedPath.c_str());
  if (os.closed() || is.closed()) return false;
  uint8_t buf[64];
  int l = 0;
  while(true) {
    l = is.read(buf, sizeof(buf));
    if (l < 0) break;
    os.write(buf, l);
  }
  if (onRefresh) onRefresh(); else reload();
  return l == -1;
}

void PicoExplorer::pageDown() {
  _i += _r;
  while (_i >= optionCount()) _i -= optionCount();
  repaint();
}

void PicoExplorer::pageUp() {
  _i -= _r;
  while (_i < 0) _i += optionCount();
  repaint();
}

void PicoExplorer::paintRow(PicoPen *pen, bool focused, int32_t i) {
  FILINFO finfo;
  const char *fname = _cache.read(i, &finfo) ? finfo.fname : "????????.???";
  pen->printAtF(0, 0, false, "%c %c %s", (focused ? '>' : ' '), (finfo.fattrib & AM_DIR ? '*' : ' '), fname);
  for(int32_t i = strlen(fname) + 4; i < pen->cw(); ++i) pen->printAt(i, 0, false, " ");
}

void PicoExplorer::action(int32_t i, std::function<void(FILINFO *info, int32_t i)> func) {
  if (func) {
    FILINFO finfo;
    if (_cache.read(i, &finfo)) {
      func(&finfo, i);
    }
  }
}

void PicoExplorer::toggleSelection(int32_t i) {
  FILINFO finfo;
  if (_cache.read(i, &finfo)) {
    toggleSelection(i, &finfo);
  }
}

void PicoExplorer::toggleSelection(int32_t i, FILINFO* finfo) {
  if (finfo->fattrib & AM_DIR) {
    _path.push(finfo->fname);
    load();
  }
  else {
    std::string fullpath;
    FatFsFilePath path(&_path, finfo->fname);
    path.appendTo(fullpath);
    if (onToggle) onToggle(finfo, i, fullpath.c_str());
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

void PicoExplorer::next(std::function<bool(FILINFO *info)> filter, int d) {
  if (_i < 0 || optionCount() < 1) return;
  if (d > 1) d = 1;
  if (d < -1) d = -1;
  int32_t j = _i + d;
  do {
    if (j >= optionCount()) j -= optionCount();
    if (j < 0) j += optionCount();
    FILINFO finfo;
    if (!_cache.read(j, &finfo)) {
      DBG_PRINTF("PicoExplorer: failed to read directory entry for position %ld\n", j);
      return;
    }
    if (filter(&finfo)) {
      DBG_PRINTF("PicoExplorer: toggling next entry at position %ld, '%s'\n", j, finfo.fname);
      _i = j;
      toggleSelection(j, &finfo);
      repaint();
      return;
    }
    if (d == 0) {
      d = 1;
    }
    j += d;
  }
  while (j != _i);
}

void PicoExplorer::load() {
  _i = 0;
  _cache.attach(&_path);
  if (_cache.open()) {
    repaint();    
  }
  else {
    if (onRefresh) onRefresh(); else reload();
  }
}

void PicoExplorer::reload() {
  _i = 0;
  _cache.attach(&_path);
  _cache.reload();
  repaint();
}

bool PicoExplorer::checkExists(const char *file) {
  if (!_sdCard->mounted()) {
    if (!_sdCard->mount()) return false;
  }

  std::string fullpath;
  {
    FatFsFilePath path(&_path, file);
    path.appendTo(fullpath);
  }
  DBG_PRINTF("PicoExplorer: Checking '%s' exists\n", fullpath.c_str());

  FILINFO fno;
  FRESULT fr = f_stat(fullpath.c_str(), &fno);
  return fr == FR_OK;
}

bool PicoExplorer::deleteFile(const char *file) {
  if (!_sdCard->mounted()) {
    if (!_sdCard->mount()) return false;
  }
  
  std::string fullpath;
  {
    FatFsFilePath path(&_path, file);
    path.appendTo(fullpath);
  }
  DBG_PRINTF("PicoExplorer: Deleting '%s' exists\n", fullpath.c_str());

  bool r = f_unlink(fullpath.c_str()) == FR_OK;
  
  reload();
  
  return r;
}

bool PicoExplorer::renameFile(const char *fileo, const char *filen) {
  if (!_sdCard->mounted()) {
    if (!_sdCard->mount()) return false;
  }
  
  std::string fullpatho;
  {
    FatFsFilePath patho(&_path, fileo);
    patho.appendTo(fullpatho);
  }
  std::string fullpathn;
  {
    FatFsFilePath pathn(&_path, filen);
    pathn.appendTo(fullpathn);
  }
  
  DBG_PRINTF("PicoExplorer: Renaming '%s' to '%s'\n", fullpatho.c_str(), fullpathn.c_str());
  
  bool r = f_rename(fullpatho.c_str(), fullpathn.c_str()) == FR_OK;

  reload();
  
  return r;  
}

bool PicoExplorer::folderModified(const char *folder) {
  if (_path.equals(folder)) {
    DBG_PRINTF("PicoExplorer: Reloading modified folder '%s'\n", folder);
    reload();
    return true;
  }
  else {
    std::string cp(folder);
    cp.append("/.dcache");
    DBG_PRINTF("PicoExplorer: Removing cache from modified folder '%s'\n", cp.c_str());
    return FR_OK == f_unlink(cp.c_str());
  }
}


