#include "PicoOption.h"

PicoOption::PicoOption()
{
}

bool PicoOption::selected() { 
  return _selected && _selected();
}

PicoOption* PicoOption::addQuickKey(PicoQuickKey *quickKey) {
  _quickKeys.push_back(quickKey);
  return this;
}

PicoOption::~PicoOption() {
}

bool PicoOption::isQuickKey(uint8_t keycode, uint8_t modifiers, uint8_t ascii) {
  for (PicoQuickKey * quickKey : _quickKeys) {
    if (quickKey->isQuickKey(keycode, modifiers, ascii)) return true;
  }
  return false;
}

void PicoOption::toggle() {
  if (_toggle) {
    _toggle();
  }
}

void PicoOption::paintRow(PicoPen *pen, bool focused, int32_t i, int32_t ww) {
  pen->setAttr(selected() ? 1 : 0);

  if (_quickKeys.size()) {
    const char q = _quickKeys[0]->ascii();
    const char c = q > 32 ? q : '.';
    pen->printAtF(0, 0, false, "%c %c.", (focused ? '>' : ' '), c);
  }
  else {
    pen->printAtF(0, 0, false, "%c   ", (focused ? '>' : ' '));
  }
  
  if (_paint) {
    PicoPen rpen(pen, 5, 0, pen->cw() - 5, 1);
    _paint(&rpen);
  }
}


