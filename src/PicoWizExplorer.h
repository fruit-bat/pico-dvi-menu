#pragma once

#include "PicoExplorer.h"
#include "PicoWizUtils.h"
#include "PicoTextField.h"
#include <string>

class PicoWizExplorer : public PicoExplorer {
private:
  PicoWizUtils* _utils;
  std::string _tmpName;
  PicoTextField _fileName;

  void wizRenameFile(FILINFO *finfo, int32_t i);
  void wizDeleteFile(FILINFO *finfo, int32_t i);
  void wizPasteFile(const char* name);
  void wizRefreshFolder();

public:
  PicoWizExplorer(
    PicoWizUtils* utils,
    SdCardFatFsSpi* sdCard,
    FatFsFilePath* root,
    int32_t x,
    int32_t y,
    int32_t w,
    int32_t r,
    int32_t rh
  );

  ~PicoWizExplorer() {}
  
  std::function<void()> onRefreshDisplay;  
};
