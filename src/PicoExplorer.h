#pragma once

#include "PicoWin.h"
#include "PicoOption.h"
#include <vector>
#include <string>
#include <functional>
#include "FatFsDirCache.h"
#include "FatFsFilePath.h"

class PicoExplorer : public PicoWin {
protected:
  SdCardFatFsSpi* _sdCard;

private:
  FatFsDirCache _cache;
  FatFsFilePath _path; 
  int32_t _i, _r, _rh;
  
  std::string _copiedPath;
  std::string _copiedName;

  void paintRow(PicoPen *pen, bool focused, int32_t i);
  int32_t optionCount() { return (int32_t)_cache.size(); }
  void toggleSelection(int32_t i);
  void toggleSelection(int32_t i, FILINFO* finfo);
  void pageDown();
  void pageUp();
  void load();
  void action(int32_t i, std::function<void(FILINFO *info, int32_t i)> func);
  void copy(FILINFO *finfo, int32_t i);
  
public:

  PicoExplorer(SdCardFatFsSpi* sdCard, FatFsFilePath* root, int32_t x, int32_t y, int32_t w, int32_t r, int32_t rh);
  void focus(int32_t i);
  int32_t focus() { return _i; };
  void next(std::function<bool(FILINFO *info)> filter, int d);
  void reload();
  bool checkExists(const char *file);
  bool deleteFile(const char *file);
  bool renameFile(const char *fileo, const char *filen);
  bool pasteFile(const char *file);
  bool saveFile(const char *file, std::function<bool(FatFsSpiOutputStream *os)> write);
  bool folderModified(const char *folder);
  SdCardFatFsSpi* sdCard() { return _sdCard; }
  
  std::function<void(FILINFO *info, int32_t i, const char* path)> onToggle;
  std::function<void(FILINFO *info, int32_t i)> onRenameFile;
  std::function<void(FILINFO *info, int32_t i)> onDeleteFile;
  std::function<void()> onSaveFile;
  std::function<void(const char *name)> onPasteFile;
  std::function<void()> onRefresh;
};
