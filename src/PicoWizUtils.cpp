
#include "PicoWizUtils.h"

PicoWizUtils::  PicoWizUtils (
  PicoWiz* wiz,
  uint32_t sep,
  PicoQuickKeyAscii* qkNo,
  PicoQuickKeyAscii* qkYes
) : 
  _wiz(wiz),
  _message(0, 0, wiz->ww(), 12),
  _confirm(0, 0, wiz->ww(), 6, sep),
  _confirmNo("No"),
  _confirmYes("Yes")
{
  _confirm.addOption(_confirmNo.addQuickKey(qkNo));
  _confirm.addOption(_confirmYes.addQuickKey(qkYes));
  _confirm.enableQuickKeys();  
}

void PicoWizUtils::showError(std::function<void(PicoPen *pen)> message) {
  _wiz->push(
    &_message, 
    [](PicoPen *pen){ pen->printAt(0, 0, false, "Error:"); },
    true);
  _message.onPaint(message);
}

void PicoWizUtils::showMessage(std::function<void(PicoPen *pen)> message) {
  _wiz->push(
    &_message, 
    [](PicoPen *pen){ pen->printAt(0, 0, false, ""); },
    true);
  _message.onPaint(message);
}

void PicoWizUtils::removeMessage() {
  _wiz->pop(true);
}

void PicoWizUtils::confirm(
  std::function<void(PicoPen *pen)> message,
  std::function<void()> yes
) {
  _confirm.focus(0);
  _wiz->push(
    &_confirm, 
    message,
    true);
  _confirmNo.toggle([=]() {
    _wiz->pop(true);
  });
  _confirmYes.toggle([=]() {
    _wiz->pop(true);
    if (yes) yes();
  });
}

void PicoWizUtils::confirm(
  std::function<void(PicoPen *pen)> message,
  std::function<void()> no,
  std::function<void()> yes
) {
  _confirm.focus(0);
  _wiz->push(
    &_confirm, 
    message,
    true);
  _confirmNo.toggle([=]() {
    _wiz->pop(true);
    if (no) no();
  });
  _confirmYes.toggle([=]() {
    _wiz->pop(true);
    if (yes) yes();
  });
}
