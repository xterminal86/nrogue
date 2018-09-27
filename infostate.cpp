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
      Application::Instance().ChangeState(Application::Instance().kMainStateIndex);      
      break;
  }  
}

void InfoState::Update()
{
  clear();
  
  Printer::Instance().Print(40, 12, "InfoState::Update()", Printer::kAlignLeft, "#FFFFFF");
  
  refresh();
}
