#include "infostate.h"
#include "mainstate.h"
#include "printer.h"
#include "application.h"
#include "util.h"

void InfoState::HandleInput()
{
  _keyPressed = GetKeyDown();
  
  switch (_keyPressed)
  {
    case 'q':
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;
  }  
}

void InfoState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    int yPos = 2;

    auto& playerRef = Application::Instance().PlayerInstance;

    std::string title = Util::StringFormat("%s the %s",
                                           playerRef.Name.data(),
                                           playerRef.GetClassName().data());
    Printer::Instance().PrintFB(0, 0, title, Printer::kAlignLeft, "#FFFFFF");

    int charToPrint = 0;

    #ifdef USE_SDL
    charToPrint = (int)NameCP437::HBAR_2;
    #else
    charToPrint = '=';
    #endif

    for (int i = 0; i < kMaxNameUnderscoreLength; i++)
    {      
      Printer::Instance().PrintFB(i, 1, charToPrint, "#FFFFFF");
      Printer::Instance().PrintFB(i, yPos + 12, charToPrint, "#FFFFFF");
    }

    #ifdef USE_SDL
    charToPrint = (int)NameCP437::VBAR_1;
    #else
    charToPrint = '|';
    #endif

    int th = Printer::Instance().TerminalHeight;
    for (int y = 0; y < th; y++)
    {
      Printer::Instance().PrintFB(kMaxNameUnderscoreLength, y, charToPrint, "#FFFFFF");
    }

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

    int maxLength = FindAttrsMaxStringLength();

    PrintModifiers(7 + maxLength, yPos + 3);

    // Skills

    Printer::Instance().PrintFB(kMaxNameUnderscoreLength / 2, yPos + 13, "SKILLS", Printer::kAlignCenter, "#FFFFFF");

    int i = 14;
    for (auto& kvp : playerRef.SkillLevelBySkill)
    {
      std::string skillName = GlobalConstants::SkillNameByType.at(kvp.first);
      Printer::Instance().PrintFB(0, yPos + i, skillName, Printer::kAlignLeft, "#FFFFFF");
      i++;
    }

    Printer::Instance().Render();
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
    if (attrName == "EXP" || attrName == "LVL")
    {
      text = Util::StringFormat("%s: %i", attrName.data(), attr.CurrentValue);
    }
    else
    {
      text = Util::StringFormat("%s: %i", attrName.data(), attr.OriginalValue);
    }
  }

  Printer::Instance().PrintFB(x, y, text, Printer::kAlignLeft, color);
}

void InfoState::PrintModifiers(int x, int y)
{
  auto& playerRef = Application::Instance().PlayerInstance;

  int strMod = playerRef.Attrs.Str.Modifier;
  int defMod = playerRef.Attrs.Def.Modifier;
  int magMod = playerRef.Attrs.Mag.Modifier;
  int resMod = playerRef.Attrs.Res.Modifier;
  int sklMod = playerRef.Attrs.Skl.Modifier;
  int spdMod = playerRef.Attrs.Spd.Modifier;

  std::pair<std::string, std::string> res;

  res = GetModifierString(strMod);
  Printer::Instance().PrintFB(x, y, res.second, Printer::kAlignLeft, res.first);

  res = GetModifierString(defMod);
  Printer::Instance().PrintFB(x, y + 1, res.second, Printer::kAlignLeft, res.first);

  res = GetModifierString(magMod);
  Printer::Instance().PrintFB(x, y + 2, res.second, Printer::kAlignLeft, res.first);

  res = GetModifierString(resMod);
  Printer::Instance().PrintFB(x, y + 3, res.second, Printer::kAlignLeft, res.first);

  res = GetModifierString(sklMod);
  Printer::Instance().PrintFB(x, y + 4, res.second, Printer::kAlignLeft, res.first);

  res = GetModifierString(spdMod);
  Printer::Instance().PrintFB(x, y + 5, res.second, Printer::kAlignLeft, res.first);
}

std::pair<std::string, std::string> InfoState::GetModifierString(int value)
{
  std::pair<std::string, std::string> res;

  std::string color = "#FFFFFF";
  std::string str;

  if (value < 0)
  {
    color = "#FF0000";
    str = Util::StringFormat("(%i)", value);
  }
  else if (value > 0)
  {
    color = "#00FF00";
    str = Util::StringFormat("(+%i)", value);
  }
  else if (value == 0)
  {
    str = Util::StringFormat("(+%i)", value);
  }

  res.first = color;
  res.second = str;

  return res;
}

int InfoState::FindAttrsMaxStringLength()
{
  auto& playerRef = Application::Instance().PlayerInstance;

  std::vector<int> lengths;

  lengths.push_back(std::to_string(playerRef.Attrs.Str.OriginalValue).length());
  lengths.push_back(std::to_string(playerRef.Attrs.Def.OriginalValue).length());
  lengths.push_back(std::to_string(playerRef.Attrs.Mag.OriginalValue).length());
  lengths.push_back(std::to_string(playerRef.Attrs.Res.OriginalValue).length());
  lengths.push_back(std::to_string(playerRef.Attrs.Skl.OriginalValue).length());
  lengths.push_back(std::to_string(playerRef.Attrs.Spd.OriginalValue).length());

  int max = 0;
  for (auto& l : lengths)
  {
    if (l > max)
    {
      max = l;
    }
  }

  return max;
}
