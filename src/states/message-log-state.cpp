#include "message-log-state.h"

#include "application.h"
#include "printer.h"
#include "util.h"

void MessageLogState::Prepare()
{
  _scrollPosition = 0;
}

void MessageLogState::HandleInput()
{
  _keyPressed = GetKeyDown();

  int msgSize = Printer::Instance().Messages().size();
  int th = Printer::Instance().TerminalHeight;

  // Since we draw messages from y = 1, compensate with (th - 2)
  int scrollLimit = (msgSize - 1) - (th - 2);

  switch (_keyPressed)
  {
    case ALT_K2:
    case NUMPAD_2:
      if (msgSize > th - 2)
      {
        _scrollPosition++;
      }
      break;

    case ALT_K8:
    case NUMPAD_8:
      if (msgSize > th - 2)
      {
        _scrollPosition--;
      }
      break;

    case VK_CANCEL:
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;

    default:
      break;
  }

  _scrollPosition = Util::Clamp(_scrollPosition, 0, scrollLimit);
}

void MessageLogState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    DrawHeader(" GAME LOG ");

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
      #ifdef USE_SDL
      Printer::Instance().PrintFB(tw - 1, th - 1, (int)NameCP437::DARROW_2, "#FFFFFF");
      #else
      Printer::Instance().PrintFB(tw - 1, th - 1, "\\/", Printer::kAlignRight, "#FFFFFF");
      #endif
    }
    else if (_scrollPosition == scrollLimit)
    {
      #ifdef USE_SDL
      Printer::Instance().PrintFB(tw - 1, 1, (int)NameCP437::UARROW_2, "#FFFFFF");
      #else
      Printer::Instance().PrintFB(tw - 1, 1, "/\\", Printer::kAlignRight, "#FFFFFF");
      #endif
    }
    else if (_scrollPosition > 0 && _scrollPosition != scrollLimit)
    {
      #ifdef USE_SDL
      Printer::Instance().PrintFB(tw - 1, 1, (int)NameCP437::UARROW_2, "#FFFFFF");
      Printer::Instance().PrintFB(tw - 1, th - 1, (int)NameCP437::DARROW_2, "#FFFFFF");
      #else
      Printer::Instance().PrintFB(tw - 1, 1, "/\\", Printer::kAlignRight, "#FFFFFF");
      Printer::Instance().PrintFB(tw - 1, th - 1, "\\/", Printer::kAlignRight, "#FFFFFF");
      #endif
    }
  }
}
