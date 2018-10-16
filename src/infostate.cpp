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

    int yPos = 2;

    attr = Util::StringFormat("LVL: %i", playerRef.Actor.Attrs.Level);
    Printer::Instance().PrintFB(0, yPos, attr, Printer::kAlignLeft, "#FFFFFF");

    attr = Util::StringFormat("EXP: %i", playerRef.Actor.Attrs.Exp);
    Printer::Instance().PrintFB(0, yPos + 1, attr, Printer::kAlignLeft, "#FFFFFF");

    attr = Util::StringFormat("STR: %i", playerRef.Actor.Attrs.Str.Get());
    Printer::Instance().PrintFB(0, yPos + 3, attr, Printer::kAlignLeft, "#FFFFFF");

    attr = Util::StringFormat("DEF: %i", playerRef.Actor.Attrs.Def.Get());
    Printer::Instance().PrintFB(0, yPos + 4, attr, Printer::kAlignLeft, "#FFFFFF");

    attr = Util::StringFormat("MAG: %i", playerRef.Actor.Attrs.Mag.Get());
    Printer::Instance().PrintFB(0, yPos + 5, attr, Printer::kAlignLeft, "#FFFFFF");

    attr = Util::StringFormat("RES: %i", playerRef.Actor.Attrs.Res.Get());
    Printer::Instance().PrintFB(0, yPos + 6, attr, Printer::kAlignLeft, "#FFFFFF");

    attr = Util::StringFormat("SKL: %i", playerRef.Actor.Attrs.Skl.Get());
    Printer::Instance().PrintFB(0, yPos + 7, attr, Printer::kAlignLeft, "#FFFFFF");

    attr = Util::StringFormat("SPD: %i", playerRef.Actor.Attrs.Spd.Get());
    Printer::Instance().PrintFB(0, yPos + 8, attr, Printer::kAlignLeft, "#FFFFFF");

    attr = Util::StringFormat("HP: %i/%i", playerRef.Actor.Attrs.HP.Get(), playerRef.Actor.Attrs.HP.OriginalValue);
    Printer::Instance().PrintFB(0, yPos + 10, attr, Printer::kAlignLeft, "#FFFFFF");

    attr = Util::StringFormat("MP: %i/%i", playerRef.Actor.Attrs.MP.Get(), playerRef.Actor.Attrs.MP.OriginalValue);
    Printer::Instance().PrintFB(0, yPos + 11, attr, Printer::kAlignLeft, "#FFFFFF");

    Printer::Instance().Render();
  }
}

void InfoState::ShowInventory()
{
  int tw = Printer::Instance().TerminalWidth;

  Printer::Instance().PrintFB(tw - 1, 0, "INVENTORY", Printer::kAlignRight, "#FFFFFF");
  Printer::Instance().PrintFB(tw - 1, 1, "=========", Printer::kAlignRight, "#FFFFFF");

  auto& playerRef = Application::Instance().PlayerInstance;

  int yPos = 0;
  for (auto& item : playerRef.Inventory.Contents)
  {
    std::string attr = Util::StringFormat("%s", item.get()->ObjectName.data());
    Printer::Instance().PrintFB(tw - 1, 2 + yPos, attr, Printer::kAlignRight, "#FFFFFF");
    yPos++;
  }
}
