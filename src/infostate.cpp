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

    PrintAttribute(0, yPos, "LVL", playerRef.Attrs.Lvl);
    PrintAttribute(0, yPos + 1, "EXP", playerRef.Attrs.Exp);

    PrintAttribute(0, yPos + 3, "STR", playerRef.Attrs.Str);
    PrintAttribute(0, yPos + 4, "DEF", playerRef.Attrs.Def);
    PrintAttribute(0, yPos + 5, "MAG", playerRef.Attrs.Mag);
    PrintAttribute(0, yPos + 6, "RES", playerRef.Attrs.Res);
    PrintAttribute(0, yPos + 7, "SKL", playerRef.Attrs.Skl);
    PrintAttribute(0, yPos + 8, "SPD", playerRef.Attrs.Spd);

    PrintAttribute(0, yPos + 10, "HP", playerRef.Attrs.HP, true);
    PrintAttribute(0, yPos + 11, "MP", playerRef.Attrs.MP, true);

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

void InfoState::PrintAttribute(int x, int y, std::string attrName, Attribute& attr, bool displayMaxValue)
{
  std::string color = "#FFFFFF";

  std::string text;
  if (displayMaxValue)
  {
    text = Util::StringFormat("%s: %i / %i", attrName.data(), attr.CurrentValue, attr.OriginalValue);
  }
  else
  {    
    if (attr.Modifier < 0)
    {
      color = "#FF0000";
    }
    else if (attr.Modifier > 0)
    {
      color = "#4444FF";
    }

    if (attrName == "EXP" || attrName == "LVL")
    {
      text = Util::StringFormat("%s: %i", attrName.data(), attr.Get(false));
    }
    else
    {
      text = Util::StringFormat("%s: %i", attrName.data(), attr.Get());
    }
  }

  Printer::Instance().PrintFB(x, y, text, Printer::kAlignLeft, color);
}

