#include "menustate.h"
#include "application.h"
#include "printer.h"

void MenuState::HandleInput()
{
  _keyPressed = getch();

  switch (_keyPressed)
  {
    case 10:
      Application::Instance().ChangeState(Application::GameStates::SELECT_CLASS_STATE);
      break;

    case 'q':    
      Application::Instance().ChangeState(Application::GameStates::EXIT_GAME);      
      break;  
  }
}

void MenuState::Update()
{
  if (_keyPressed != -1)
  {
    Printer::Instance().ClearFrameBuffer();

    int tw = Printer::Instance().TerminalWidth;
    int th = Printer::Instance().TerminalHeight;

    int halfW = tw / 2;
    int halfH = th / 2;

    int titleX = halfW;
    int titleY = 2; // halfH - 12 + offset;

    int offset = 0;

    auto border = Util::GetPerimeter(0, 0, tw - 1, th - 1);
    for (auto& i : border)
    {
      chtype charToPrint = '=';

      if (i.Y > 0 && i.Y < th - 1)
      {
        charToPrint = '|';
      }

      Printer::Instance().PrintToFrameBuffer(i.X, i.Y, charToPrint, "#FFFFFF");
    }

    for (auto& s : _title)
    {
      Printer::Instance().PrintToFrameBuffer(titleX, titleY + offset, s, Printer::kAlignCenter, "#FFFFFF");

      offset++;
    }

    Printer::Instance().PrintToFrameBuffer(halfW, halfH + 2, "(press 'Enter' to start, 'q' to exit)", Printer::kAlignCenter, "#FFFFFF");
    Printer::Instance().PrintToFrameBuffer(tw - 1, th - 2, "(c) 2018 by xterminal86", Printer::kAlignRight, "#FFFFFF");

    auto debugInfo = Util::StringFormat("terminal size: %ix%i", tw, th);
    Printer::Instance().PrintToFrameBuffer(1, th - 2, debugInfo, Printer::kAlignLeft, "#FFFFFF");

    Printer::Instance().Render();
  }  
}
