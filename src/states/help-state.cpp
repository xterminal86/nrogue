#include "help-state.h"

#include "application.h"
#include "printer.h"
#include "util.h"

HelpState::HelpState()
{
  // Kinda hack but fuck it

  std::string s = Util::StringFormat("'%c' '%c' '%c'", ALT_K7, ALT_K8, ALT_K9);
  _helpText[14] = s;

  s = Util::StringFormat("'%c' '%c' '%c' or numpad for movement", ALT_K4, ALT_K5, ALT_K6);
  _helpText[16] = s;

  s = Util::StringFormat("'%c' '%c' '%c'", ALT_K1, ALT_K2, ALT_K3);
  _helpText[18] = s;
}

void HelpState::Prepare()
{
  _scrollPosition = 0;
}

void HelpState::HandleInput()
{
  _keyPressed = GetKeyDown();

  int msgSize = _helpText.size();
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

void HelpState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    DrawHeader(" HELP ");

    DrawScrollBars();

    int offsetY = 1;
    for (size_t i = _scrollPosition; i < _helpText.size(); i++)
    {
      Printer::Instance().PrintFB(1, offsetY, _helpText[i], Printer::kAlignLeft, "#FFFFFF");
      offsetY++;
    }

    Printer::Instance().Render();
  }
}

void HelpState::DrawScrollBars()
{
  size_t tw = Printer::Instance().TerminalWidth;
  size_t th = Printer::Instance().TerminalHeight;

  // Since we draw messages from y = 1, compensate y pos with (th - 2)
  int scrollLimit = (_helpText.size() - 1) - (th - 2);

  if (_helpText.size() - 1 > th - 2)
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
