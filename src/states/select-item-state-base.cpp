#include "select-item-state-base.h"

#include "printer.h"
#include "application.h"

void SelectItemStateBase::HandleInput()
{
  _keyPressed = GetKeyDown();

  ProcessInput();
}

// =============================================================================

void SelectItemStateBase::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Game::gPrnt.Clear();

    DrawHeader(_headerText);

    DrawSpecific();

    Game::gPrnt.PrintFB(_twHalf,
                        _th - 1,
                        "'q' - exit",
                        Printer::kAlignCenter,
                        Colors::WhiteColor,
                        Colors::BlackColor);

    Game::gPrnt.Render();
  }
}
