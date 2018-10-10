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
    Printer::Instance().Clear();

    Printer::Instance().PrintFB(Printer::Instance().TerminalWidth / 2, 0, "==== HELP ====", Printer::kAlignCenter, "#FFFFFF");

    int offset = 1;
    for (auto& item : _helpText)
    {
      Printer::Instance().PrintFB(0, offset, item, Printer::kAlignLeft, "#FFFFFF");
      offset++;
    }

    Printer::Instance().Render();
  }
}
