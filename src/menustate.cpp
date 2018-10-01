#include "menustate.h"
#include "application.h"
#include "printer.h"

void MenuState::HandleInput()
{
  _keyPressed = getch();

  switch (_keyPressed)
  {
    case 10:
      Application::Instance().ChangeState(Application::GameStates::MAIN_STATE);      
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
    clear();
  
    int tw = Printer::Instance().TerminalWidth;
    int th = Printer::Instance().TerminalHeight;
  
    int halfW = tw / 2;
    int halfH = th / 2;
    
    int titleY = 2; //halfH - 12 + offset;

    int offset = 0;
    for (auto& s : _title)
    {       
      Printer::Instance().Print(halfW, titleY + offset, s, Printer::kAlignCenter, "#FFFFFF");
      offset++;
    }
    
    titleY += offset;

    Printer::Instance().Print(halfW, titleY + 2, "(press 'Enter' to start, 'q' to exit)", Printer::kAlignCenter, "#FFFFFF");
    Printer::Instance().Print(tw, th - 1, "(c) 2018 by xterminal86", Printer::kAlignRight, "#FFFFFF");
    
    auto debugInfo = Util::StringFormat("terminal size: %ix%i", tw, th);
    Printer::Instance().Print(0, th - 1, debugInfo, Printer::kAlignLeft, "#FFFFFF");
  
    refresh();
  }
}
