#include "menustate.h"
#include "application.h"
#include "printer.h"

void MenuState::HandleInput()
{
  char ch = getch();

  switch (ch)
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
  clear();

  int tw = Printer::Instance().TerminalWidth;
  int th = Printer::Instance().TerminalHeight;

  //Printer::Instance().Print(tw / 2, th / 2 - 4, "NROGUE", Printer::kAlignCenter, "#FFFFFF");
  Printer::Instance().Print(0, th / 2 - 12, _title, Printer::kAlignLeft, "#FFFFFF");
  Printer::Instance().Print(tw / 2, th / 2 + 4, "(press 'Enter' to start, 'q' to exit)", Printer::kAlignCenter, "#FFFFFF");
  Printer::Instance().Print(tw, th - 1, "(c) 2018 by xterminal86", Printer::kAlignRight, "#FFFFFF");

  refresh();
}