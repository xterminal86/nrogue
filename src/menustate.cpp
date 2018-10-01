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
    
    int titleX = halfW - _title[0].size() / 2;
    int titleY = 2; // halfH - 12 + offset;

    std::vector<std::vector<chtype>> acs;

    int offsetY = 0;
    for (auto& s : _title)
    {
      std::vector<chtype> newStr;

      for (auto& ch : s)
      {
        chtype acsCh = ' ';

        if (ch == '#')
        {
          acsCh = ACS_BLOCK;
        }

        newStr.push_back(acsCh);
      }

      acs.push_back(newStr);
    }

    int offsetX = 0;
    for (auto& v : acs)
    {
      for (auto& c : v)
      {
        Printer::Instance().Print(titleX + offsetX, titleY + offsetY, c, "#FFFFFF");
        offsetX++;
      }

      offsetX = 0;
      offsetY++;
    }
    
    titleY += offsetY;

    Printer::Instance().Print(halfW, titleY + 2, "(press 'Enter' to start, 'q' to exit)", Printer::kAlignCenter, "#FFFFFF");
    Printer::Instance().Print(tw, th - 1, "(c) 2018 by xterminal86", Printer::kAlignRight, "#FFFFFF");
    
    auto debugInfo = Util::StringFormat("terminal size: %ix%i", tw, th);
    Printer::Instance().Print(0, th - 1, debugInfo, Printer::kAlignLeft, "#FFFFFF");
  
    refresh();
  }
}
