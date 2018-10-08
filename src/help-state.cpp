#include "help-state.h"

#include "application.h"
#include "printer.h"

void HelpState::HandleInput()
{
  _keyPressed = getch();

  switch(_keyPressed)
  {
    case 'q':
      Application::Instance().ChangeState(Application::GameStates::MAIN_STATE);
      break;
  }
}

void HelpState::Update()
{
  if (_keyPressed != -1)
  {
    clear();

    int offset = 0;
    for (auto& item : _helpText)
    {
      Printer::Instance().Print(0, offset, item, Printer::kAlignLeft, "#FFFFFF");
      offset++;
    }

    refresh();
  }
}
