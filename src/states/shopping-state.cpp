#include "shopping-state.h"

#include "printer.h"

void ShoppingState::HandleInput()
{
  _keyPressed = getch();
}

void ShoppingState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    Printer::Instance().Render();
  }
}
