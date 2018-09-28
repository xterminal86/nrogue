#include "infostate.h"
#include "mainstate.h"
#include "printer.h"
#include "application.h"

void InfoState::HandleInput()
{
  char ch = getch();  
  
  switch (ch)
  {
    case 'q':
      Application::Instance().ChangeState(Application::GameStates::MAIN_STATE);      
      break;
  }  
}

void InfoState::Update()
{
  clear();
  
  Printer::Instance().Print(0, 0, "You check yourself out: you look good! :-)", Printer::kAlignLeft, "#FFFFFF");
  
  refresh();
}
