#include "select-item-state-base.h"

#include "printer.h"

void SelectItemStateBase::HandleInput()
{
  _keyPressed = GetKeyDown();

  ProcessInput();
}

void SelectItemStateBase::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    DrawHeader(_headerText);

    DrawSpecific();

    Printer::Instance().PrintFB(_twHalf, _th - 1, "'q' - cancel", Printer::kAlignCenter, Colors::WhiteColor);
    Printer::Instance().Render();
  }
}
