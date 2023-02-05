#pragma once

#include "PicoWiz.h"
#include "PicoWin.h"
#include "PicoSelect.h"
#include "PicoOptionText.h"
#include "PicoQuickKeyAscii.h"

class PicoWizUtils {
private:
  PicoWiz* _wiz;
  PicoWin _message;
  PicoSelect _confirm;
  PicoOptionText _confirmNo;
  PicoOptionText _confirmYes;
  
public:
  PicoWizUtils (
    PicoWiz* wiz,
    uint32_t sep,
    PicoQuickKeyAscii* qkNo,
    PicoQuickKeyAscii* qkYes
  );
  
  ~PicoWizUtils() {};
  
  PicoWiz* wiz() { return _wiz; }
  
  void showError(
    std::function<void(PicoPen *pen)> message
  );

  void confirm(
    std::function<void(PicoPen *pen)> message,
    std::function<void()> no,
    std::function<void()> yes
  );
  
  void confirm(
    std::function<void(PicoPen *pen)> message,
    std::function<void()> yes
  ); 
  
  void showMessage(
    std::function<void(PicoPen *pen)> message
  );
  
  void removeMessage();  
};
