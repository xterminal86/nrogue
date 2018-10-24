#include "message-log-state.h"

#include "application.h"
#include "printer.h"
#include "constants.h"
#include "util.h"

void MessageLogState::HandleInput()
{
  _keyPressed = getch();

  int msgSize = Printer::Instance().Messages().size();
  int th = Printer::Instance().TerminalHeight;

  // Since we draw messages from y = 1, compensate with (th - 2)
  int scrollLimit = (msgSize - 1) - (th - 2);

  switch (_keyPressed)
  {
    case NUMPAD_2:
      if (msgSize > th - 2)
      {
        _scrollPosition++;
      }
      break;

    case NUMPAD_8:
      if (msgSize > th - 2)
      {
        _scrollPosition--;
      }
      break;

    case 'q':
      Application::Instance().ChangeState(Application::GameStates::MAIN_STATE);
      break;
  }

  _scrollPosition = Util::Clamp(_scrollPosition, 0, scrollLimit);
}

void MessageLogState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    int tw = Printer::Instance().TerminalWidth;
    int th = Printer::Instance().TerminalHeight;

    Printer::Instance().PrintFB(tw / 2, 0, "========== GAME LOG ==========", Printer::kAlignCenter, "#FFFFFF");

    DrawScrollBars();

    auto messages = Printer::Instance().Messages();

    int offsetY = 1;
    for (int i = _scrollPosition; i < messages.size(); i++)
    {
      if (_scrollPosition == 0)
      {
        Printer::Instance().PrintFB(0, 1, '*', "#FFFFFF");
      }

      Printer::Instance().PrintFB(1, offsetY, messages[i], Printer::kAlignLeft, "#FFFFFF");
      offsetY++;
    }

    Printer::Instance().Render();
  }
}

void MessageLogState::DrawScrollBars()
{
  auto messages = Printer::Instance().Messages();

  int tw = Printer::Instance().TerminalWidth;
  int th = Printer::Instance().TerminalHeight;

  // Since we draw messages from y = 1, compensate y pos with (th - 2)
  int scrollLimit = (messages.size() - 1) - (th - 2);

  if (messages.size() - 1 > th - 2)
  {
    if (_scrollPosition == 0)
    {
      Printer::Instance().PrintFB(tw - 1, th - 1, "\\/", Printer::kAlignRight, "#FFFFFF");
    }
    else if (_scrollPosition == scrollLimit)
    {
      Printer::Instance().PrintFB(tw - 1, 1, "/\\", Printer::kAlignRight, "#FFFFFF");
    }
    else if (_scrollPosition > 0 && _scrollPosition != scrollLimit)
    {
      Printer::Instance().PrintFB(tw - 1, 1, "/\\", Printer::kAlignRight, "#FFFFFF");
      Printer::Instance().PrintFB(tw - 1, th - 1, "\\/", Printer::kAlignRight, "#FFFFFF");
    }
  }
}
