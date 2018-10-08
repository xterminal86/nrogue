#include "message-log-state.h"

#include "application.h"
#include "printer.h"

void MessageLogState::HandleInput()
{
  _keyPressed = getch();

  switch (_keyPressed)
  {
    case NUMPAD_2:
      _scrollPosition--;
      break;

    case NUMPAD_8:
      _scrollPosition++;
      break;

    case 'q':
      Application::Instance().ChangeState(Application::GameStates::MAIN_STATE);
      break;
  }

  _scrollPosition = Util::Clamp(_scrollPosition, 0, Printer::Instance().Messages().size() - 1);
}

void MessageLogState::Update()
{
  if (_keyPressed != -1)
  {
    clear();

    int tw = Printer::Instance().TerminalWidth;

    Printer::Instance().Print(tw / 2, 0, "==== MESSAGES ====", Printer::kAlignCenter, "#FFFFFF");

    int offsetY = 1;
    auto messages = Printer::Instance().Messages();
    for (int i = _scrollPosition; i < messages.size(); i++)
    {
      Printer::Instance().Print(0, offsetY, messages[i], Printer::kAlignLeft, "#FFFFFF");
      offsetY++;
    }

    refresh();
  }
}
