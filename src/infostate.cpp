#include "infostate.h"
#include "mainstate.h"
#include "printer.h"
#include "application.h"

void InfoState::HandleInput()
{
  _keyPressed = getch();  
  
  switch (_keyPressed)
  {
    case 'q':
      Application::Instance().ChangeState(Application::GameStates::MAIN_STATE);      
      break;
  }  
}

void InfoState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();
    
    Printer::Instance().PrintFB(0, 0, "You check yourself out: you look good! :-)", Printer::kAlignLeft, "#FFFFFF");
    
    Printer::Instance().Render();
  }
}
