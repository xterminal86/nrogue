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
    
    auto& playerRef = Application::Instance().PlayerInstance;

    std::string title = Util::StringFormat("%s the %s", playerRef.Name.data(), playerRef.GetClassName().data());
    Printer::Instance().PrintFB(0, 0, title, Printer::kAlignLeft, "#FFFFFF");

    std::string border;
    for (int i = 0; i < title.length(); i++)
    {
      border += '=';
    }

    Printer::Instance().PrintFB(0, 1, border, Printer::kAlignLeft, "#FFFFFF");

    std::string attr;

    attr = Util::StringFormat("LV: %i", playerRef.Actor.Attrs.Level);
    Printer::Instance().PrintFB(0, 2, attr, Printer::kAlignLeft, "#FFFFFF");

    attr = Util::StringFormat("EXP: %i", playerRef.Actor.Attrs.Exp);
    Printer::Instance().PrintFB(border.length() - 1, 2, attr, Printer::kAlignRight, "#FFFFFF");

    attr = Util::StringFormat("STR: %i", playerRef.Actor.Attrs.Str.Get());
    Printer::Instance().PrintFB(0, 4, attr, Printer::kAlignLeft, "#FFFFFF");

    attr = Util::StringFormat("DEF: %i", playerRef.Actor.Attrs.Def.Get());
    Printer::Instance().PrintFB(0, 5, attr, Printer::kAlignLeft, "#FFFFFF");

    attr = Util::StringFormat("MAG: %i", playerRef.Actor.Attrs.Mag.Get());
    Printer::Instance().PrintFB(0, 6, attr, Printer::kAlignLeft, "#FFFFFF");

    attr = Util::StringFormat("RES: %i", playerRef.Actor.Attrs.Res.Get());
    Printer::Instance().PrintFB(0, 7, attr, Printer::kAlignLeft, "#FFFFFF");

    attr = Util::StringFormat("SKL: %i", playerRef.Actor.Attrs.Skl.Get());
    Printer::Instance().PrintFB(0, 8, attr, Printer::kAlignLeft, "#FFFFFF");

    attr = Util::StringFormat("SPD: %i", playerRef.Actor.Attrs.Spd.Get());
    Printer::Instance().PrintFB(0, 9, attr, Printer::kAlignLeft, "#FFFFFF");

    attr = Util::StringFormat("HP: %i/%i", playerRef.Actor.Attrs.HP.Get(), playerRef.Actor.Attrs.HP.OriginalValue);
    Printer::Instance().PrintFB(0, 11, attr, Printer::kAlignLeft, "#FFFFFF");

    attr = Util::StringFormat("MP: %i/%i", playerRef.Actor.Attrs.MP.Get(), playerRef.Actor.Attrs.MP.OriginalValue);
    Printer::Instance().PrintFB(0, 12, attr, Printer::kAlignLeft, "#FFFFFF");

    Printer::Instance().Render();
  }
}
