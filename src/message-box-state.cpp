#include "message-box-state.h"
#include "printer.h"
#include "constants.h"
#include "application.h"
#include "util.h"

void MessageBoxState::HandleInput()
{
  _keyPressed = getch();

  switch (_keyPressed)
  {
    case VK_ENTER:
      Application::Instance().CloseMessageBox();
      break;
  }
}

void MessageBoxState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    int len = _message.length();

    int tw = Printer::Instance().TerminalWidth;
    int th = Printer::Instance().TerminalHeight;

    auto area = Util::GetRectAroundPoint(tw / 2, th / 2, len, 4);
    for (auto& p : area)
    {
      Printer::Instance().PrintFB(p.X, p.Y, ' ', "#000000", kBackgroundColor);
    }

    auto border = Util::GetPerimeterAroundPoint(tw / 2, th / 2, len, 4);
    for (auto& b : border)
    {
      Printer::Instance().PrintFB(b.X, b.Y, ' ', "#000000", "#FFFFFF");
    }

    Printer::Instance().PrintFB(tw / 2, th / 2, _message, Printer::kAlignCenter, "#FFFFFF", kBackgroundColor);

    Printer::Instance().Render();
  }
}
