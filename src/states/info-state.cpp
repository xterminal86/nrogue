#include "info-state.h"
#include "printer.h"
#include "application.h"
#include "util.h"

void InfoState::HandleInput()
{
  _keyPressed = GetKeyDown();
  
  switch (_keyPressed)
  {
    case VK_CANCEL:
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;

    default:
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
    Printer::Instance().PrintFB(1, 0, title, Printer::kAlignLeft, "#FFFFFF");

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

    int th = Printer::TerminalHeight;
    for (int y = 0; y < th; y++)
    {
      Printer::Instance().PrintFB(kMaxNameUnderscoreLength, y, charToPrint, "#FFFFFF");
    }

    PrintAttribute(1, yPos, "LVL", playerRef.Attrs.Lvl);
    //PrintRangedAttribute(0, yPos + 1, "EXP", playerRef.Attrs.Exp);
    PrintAttribute(1, yPos + 1, "EXP", playerRef.Attrs.Exp.Min());

    PrintAttribute(1, yPos + 3, "STR", playerRef.Attrs.Str);
    PrintAttribute(1, yPos + 4, "DEF", playerRef.Attrs.Def);
    PrintAttribute(1, yPos + 5, "MAG", playerRef.Attrs.Mag);
    PrintAttribute(1, yPos + 6, "RES", playerRef.Attrs.Res);
    PrintAttribute(1, yPos + 7, "SKL", playerRef.Attrs.Skl);
    PrintAttribute(1, yPos + 8, "SPD", playerRef.Attrs.Spd);

    PrintRangedAttribute(1, yPos + 10, "HP", playerRef.Attrs.HP);
    PrintRangedAttribute(1, yPos + 11, "MP", playerRef.Attrs.MP);

    int maxLength = FindAttrsMaxStringLength();

    PrintModifiers(8 + maxLength, yPos + 3);

    // Skills

    Printer::Instance().PrintFB(kMaxNameUnderscoreLength / 2, yPos + 13, "SKILLS", Printer::kAlignCenter, "#FFFFFF");

    int i = 14;
    for (auto& kvp : playerRef.SkillLevelBySkill)
    {
      std::string skillName = GlobalConstants::SkillNameByType.at(kvp.first);
      Printer::Instance().PrintFB(1, yPos + i, skillName, Printer::kAlignLeft, "#FFFFFF");
      i++;
    }

    Printer::Instance().Render();
  }
}

void InfoState::PrintAttribute(int x, int y, const std::string& attrName, Attribute& attr)
{
  std::string color = "#FFFFFF";

  int modifiers = attr.GetModifiers();
  if (modifiers > 0)
  {
    color = "#00FF00";
  }
  else if (modifiers < 0)
  {
    color = "#FF0000";
  }

  std::string attrPlaceholder = Util::StringFormat("%s:...", attrName.data());
  Printer::Instance().PrintFB(x, y, attrPlaceholder, Printer::kAlignLeft, "#555555");

  std::string text = Util::StringFormat("%i", attr.Get());

  Printer::Instance().PrintFB(x + attrPlaceholder.length() - text.length(), y, text, Printer::kAlignLeft, color);

  //text = Util::StringFormat("%s: %i", attrName.data(), attr.Get());
  //Printer::Instance().PrintFB(x, y, text, Printer::kAlignLeft, color);

  // Replace stat name back with white color (kinda hack)
  auto str = Util::StringFormat("%s:", attrName.data());
  Printer::Instance().PrintFB(x, y, str, Printer::kAlignLeft, "#FFFFFF");  
}

void InfoState::PrintRangedAttribute(int x, int y, const std::string& attrName, RangedAttribute& attr)
{
  std::string color = "#FFFFFF";

  int modifiers = attr.Max().GetModifiers();
  if (modifiers > 0)
  {
    color = "#00FF00";
  }
  else if (modifiers < 0)
  {
    color = "#FF0000";
  }

  std::string placeholder = Util::StringFormat("%s: ... / ...", attrName.data());
  Printer::Instance().PrintFB(x, y, placeholder, Printer::kAlignLeft, "#555555");

  //std::string text = Util::StringFormat("%s: %i / %i", attrName.data(), attr.Min().Get(), attr.Max().Get());
  //Printer::Instance().PrintFB(x, y, text, Printer::kAlignLeft, color);

  std::string minVal = Util::StringFormat("%i", attr.Min().Get());
  std::string maxVal = Util::StringFormat("%i", attr.Max().Get());

  Printer::Instance().PrintFB(x + placeholder.length() - 6 - minVal.length(), y, minVal, Printer::kAlignLeft, color);
  Printer::Instance().PrintFB(x + placeholder.length() - 3, y, maxVal, Printer::kAlignLeft, color);
  Printer::Instance().PrintFB(x + placeholder.length() - 5, y, '/', "#FFFFFF");

  // Replace stat name back with white color (kinda hack)
  auto str = Util::StringFormat("%s:", attrName.data());
  Printer::Instance().PrintFB(x, y, str, Printer::kAlignLeft, "#FFFFFF");
}

void InfoState::PrintModifiers(int x, int y)
{
  auto& playerRef = Application::Instance().PlayerInstance;

  int strMod = playerRef.Attrs.Str.GetModifiers();
  int defMod = playerRef.Attrs.Def.GetModifiers();
  int magMod = playerRef.Attrs.Mag.GetModifiers();
  int resMod = playerRef.Attrs.Res.GetModifiers();
  int sklMod = playerRef.Attrs.Skl.GetModifiers();
  int spdMod = playerRef.Attrs.Spd.GetModifiers();

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

  lengths.push_back(std::to_string(playerRef.Attrs.Str.OriginalValue()).length());
  lengths.push_back(std::to_string(playerRef.Attrs.Def.OriginalValue()).length());
  lengths.push_back(std::to_string(playerRef.Attrs.Mag.OriginalValue()).length());
  lengths.push_back(std::to_string(playerRef.Attrs.Res.OriginalValue()).length());
  lengths.push_back(std::to_string(playerRef.Attrs.Skl.OriginalValue()).length());
  lengths.push_back(std::to_string(playerRef.Attrs.Spd.OriginalValue()).length());

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
