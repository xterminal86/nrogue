#include "message-box-state.h"
#include "printer.h"
#include "constants.h"
#include "application.h"
#include "util.h"

void MessageBoxState::HandleInput()
{
  _keyPressed = getch();

  if (_keyPressed != -1)
  {
    if (_waitForInput && (_keyPressed == VK_ENTER || _keyPressed == 'q'))
    {
      Application::Instance().CloseMessageBox();
    }
    else if (!_waitForInput)
    {
      Application::Instance().CloseMessageBox();
    }
  }
}

void MessageBoxState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    int len = 0;
    for (auto& s : _message)
    {
      if (s.length() > len)
      {
        len = s.length();
      }
    }

    // Taking into account message size (usually > 0, so as is),
    // and that it's not actually a total count of number of rows,
    // but addition that we must add to the y1,
    // so 5 rows means we must add 4 if message.size = 1, 5 if 2 and so on
    int rows = 3 + _message.size();

    int tw = Printer::Instance().TerminalWidth;
    int th = Printer::Instance().TerminalHeight;

    // x padding is randomly chosen
    int x1 = tw / 2 - len / 2 - 3;
    int y1 = th / 2 - _message.size() / 2 - 2;
    int x2 = x1 + len + 5;
    int y2 = y1 + rows;

    auto area = Util::GetScreenRect(x1, y1, x2, y2);
    for (auto& p : area)
    {
      Printer::Instance().PrintFB(p.X, p.Y, ' ', "#000000", _bgColor);
    }

    auto border = Util::GetScreenRectPerimeter(x1, y1, x2, y2);
    for (auto& b : border)
    {
      Printer::Instance().PrintFB(b.X, b.Y, ' ', "#000000", _borderColor);
    }

    std::string header = _header;
    if (header.length() != 0)
    {
      header.insert(0, " ");
      header.append(" ");

      Printer::Instance().PrintFB(tw / 2, y1, header, Printer::kAlignCenter, "#FFFFFF");      
    }

    int offset = 0;
    for (auto& s : _message)
    {
      Printer::Instance().PrintFB(tw / 2, th / 2 - _message.size() / 2 + offset, s, Printer::kAlignCenter, "#FFFFFF", _bgColor);
      offset++;
    }

    Printer::Instance().Render();
  }
}
