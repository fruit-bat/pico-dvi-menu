#include "PicoWizExplorer.h"

PicoWizExplorer::PicoWizExplorer(
  PicoWizUtils* utils,
  SdCardFatFsSpi* sdCard,
  FatFsFilePath* root,
  int32_t x,
  int32_t y,
  int32_t w,
  int32_t r,
  int32_t rh
) :
  PicoExplorer(sdCard, root, x, y, w, r, rh),
  _utils(utils),
  _fileName(0, 0, w, 64)
{
  onRenameFile = [&](FILINFO *finfo, int32_t i) {
    wizRenameFile(finfo, i);
  };
  
  onPasteFile = [&](const char* name) {
    wizPasteFile(name);
  };
  
  onDeleteFile = [&](FILINFO *finfo, int32_t i) {
    wizDeleteFile(finfo, i);
  };
  
  onRefresh = [&]() {
    wizRefreshFolder();
  };

  onSaveFile = [&]() {
    wizSaveFile();
  };
}

void PicoWizExplorer::wizSaveFile(const char* name) {
  if(saveFile(
    name, 
    [&](FatFsSpiOutputStream *os){
      return onWizSaveFile && onWizSaveFile(os);
    })
  ) {
    _utils->removeMessage();
  }
  else {
    _utils->showError([=](PicoPen *pen) {
      pen->printAtF(0, 0, false, "Failed to save '%s'", name);
    });
  }
}

void PicoWizExplorer::wizSaveFile() {
  _fileName.onenter([=](const char* name) {
    _tmpName = name;
    if (checkExists(name)) {
      _utils->confirm(
        [=](PicoPen *pen){
          pen->printAtF(0, 0, false, "Overwrite '%s'?", _tmpName.c_str());
        },
        [=]() {
          wizSaveFile(_tmpName.c_str());
        }
      );
    }
    else {
      wizSaveFile(_tmpName.c_str());
    }
  });
  if (onWizSaveFile) {
    _utils->wiz()->push(
      &_fileName, 
      [=](PicoPen *pen){ 
        pen->printAtF(0, 0, false, "Enter a save name"); 
      },
      true);
  }
}

void PicoWizExplorer::wizRenameFile(FILINFO *finfo, int32_t i) {
  _tmpName = finfo->fname;
  _fileName.set(finfo->fname);
  _fileName.onenter([=](const char* name) {
    if (checkExists(name)) {
      _utils->showError([=](PicoPen *pen) {
        pen->printAtF(0, 0, false, "Already exists '%s'", name);
      });
    }
    else {
      if(renameFile(_tmpName.c_str(), name)) {
        _utils->removeMessage();
      }
      else {
        _utils->showError([=](PicoPen *pen) {
          pen->printAtF(0, 0, false, "Failed to rename to '%s'", name);
        });
      }
    }
  });
  _utils->wiz()->push(
    &_fileName, 
    [=](PicoPen *pen){ 
      pen->printAtF(0, 0, false, "Enter new name for [ %s ]", _tmpName.c_str()); 
    },
    true);
}

void PicoWizExplorer::wizPasteFile(const char* origname) {
  _tmpName = origname;
  _fileName.set(origname);
  _fileName.onenter([=](const char* name) {
    if (checkExists(name)) {
      _utils->showError([=](PicoPen *pen) {
        pen->printAtF(0, 0, false, "Already exists '%s'", name);
      });
    }
    else {
      if(pasteFile(name)) {
        _utils->removeMessage();
      }
      else {
        _utils->showError([=](PicoPen *pen) {
          pen->printAtF(0, 0, false, "Failed to paste '%s'", name);
        });
      }
    }
  });
  _utils->wiz()->push(
    &_fileName, 
    [=](PicoPen *pen){ 
      pen->printAtF(0, 0, false, "Enter file name [ %s ]", _tmpName.c_str()); 
    },
    true);
}

void PicoWizExplorer::wizDeleteFile(FILINFO *finfo, int32_t i) {
  _tmpName = finfo->fname;
  _utils->confirm(
    [=](PicoPen *pen){
      pen->printAtF(0, 0, false, "Delete '%s'?", _tmpName.c_str());
    },
    [=]() {
      deleteFile(_tmpName.c_str());
    }
  );
}

void PicoWizExplorer::wizRefreshFolder() {
  _utils->showMessage([=](PicoPen *pen) {
    pen->printAtF(0, 0, false, "Scanning folder...");
  });
  if (onRefreshDisplay) onRefreshDisplay();
  reload();
  _utils->removeMessage();
}
