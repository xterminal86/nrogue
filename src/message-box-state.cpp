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
    int len = 20; //_message[0].length();
    int rowsNum = _message.size();
    int rows = rowsNum; //(rowsNum % 2 == 0) ? rowsNum + 2 : rowsNum + 1;

    int tw = Printer::Instance().TerminalWidth;
    int th = Printer::Instance().TerminalHeight;

    auto area = Util::GetScreenRectAroundPoint(tw / 2, th / 2, len, rows);
    for (auto& p : area)
    {
      Printer::Instance().PrintFB(p.X, p.Y, ' ', "#000000", kBackgroundColor);
    }

    auto border = Util::GetPerimeterAroundPoint(tw / 2, th / 2, len, rows);
    for (auto& b : border)
    {
      Printer::Instance().PrintFB(b.X, b.Y, '*', "#FFFFFF");
    }

    if (_header.length() != 0)
    {
      Printer::Instance().PrintFB(tw / 2, th / 2 - rows, _header, Printer::kAlignCenter, "#FFFFFF");
    }

    int offset = 0;
    for (auto& s : _message)
    {
      Printer::Instance().PrintFB(tw / 2, th / 2 - rows + 1 + offset, s, Printer::kAlignCenter, "#FFFFFF", kBackgroundColor);
      offset++;
    }

    Printer::Instance().Render();
  }
}
