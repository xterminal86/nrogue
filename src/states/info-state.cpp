#include "info-state.h"
#include "printer.h"
#include "application.h"
#include "util.h"

void InfoState::Prepare()
{
  _playerRef = &Application::Instance().PlayerInstance;

  _scrollIndex = 0;
  _scrollLimitReached = false;

  PrepareUseIdentifiedListToPrint();
}

void InfoState::PrepareUseIdentifiedListToPrint()
{
  if (_playerRef->_useIdentifiedItemsByIndex.empty())
  {
    return;
  }

  _useIdentifiedMapCopy.clear();

  std::vector<std::string> list;
  for (auto& kvp : _playerRef->_useIdentifiedItemsByIndex)
  {
    list.push_back(kvp.second.first);
  }

  size_t longestLen = Util::FindLongestStringLength(list);
  for (auto& kvp : _playerRef->_useIdentifiedItemsByIndex)
  {
    std::string key = kvp.second.first;
    key.append(longestLen - key.length(), ' ');
    _useIdentifiedMapCopy[kvp.first] = { key, kvp.second.second };
  }
}

void InfoState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case ALT_K2:
    case NUMPAD_2:
      if (!_scrollLimitReached)
      {
        _scrollIndex++;
      }
      break;

    case ALT_K8:
    case NUMPAD_8:
      if (_scrollIndex > 0)
      {
        _scrollIndex--;
      }
      break;

    case VK_CANCEL:
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;

    default:
      break;
  }

  _scrollLimitReached = ((_scrollIndex + _th) >= _playerRef->_useIdentifiedItemsByIndex.size());
}

void InfoState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    int yPos = 2;

    std::string title = Util::StringFormat("%s the %s",
                                           _playerRef->Name.data(),
                                           _playerRef->GetClassName().data());
    Printer::Instance().PrintFB(1, 0, title, Printer::kAlignLeft, Colors::WhiteColor);

    int charToPrint = 0;

    #ifdef USE_SDL
    charToPrint = (int)NameCP437::HBAR_2;
    #else
    charToPrint = '=';
    #endif

    for (int i = 0; i < kMaxNameUnderscoreLength; i++)
    {
      Printer::Instance().PrintFB(i, 1, charToPrint, Colors::WhiteColor);
      Printer::Instance().PrintFB(i, yPos + 12, charToPrint, Colors::WhiteColor);
    }

    #ifdef USE_SDL
    charToPrint = (int)NameCP437::VBAR_1;
    #else
    charToPrint = '|';
    #endif

    for (int y = 0; y < _th; y++)
    {
      Printer::Instance().PrintFB(kMaxNameUnderscoreLength, y, charToPrint, Colors::WhiteColor);
    }

    PrintAttribute(1, yPos, "LVL", _playerRef->Attrs.Lvl);
    //PrintRangedAttribute(0, yPos + 1, "EXP", playerRef.Attrs.Exp);
    PrintAttribute(1, yPos + 1, "EXP", _playerRef->Attrs.Exp.Min());

    PrintAttribute(1, yPos + 3, "STR", _playerRef->Attrs.Str);
    PrintAttribute(1, yPos + 4, "DEF", _playerRef->Attrs.Def);
    PrintAttribute(1, yPos + 5, "MAG", _playerRef->Attrs.Mag);
    PrintAttribute(1, yPos + 6, "RES", _playerRef->Attrs.Res);
    PrintAttribute(1, yPos + 7, "SKL", _playerRef->Attrs.Skl);
    PrintAttribute(1, yPos + 8, "SPD", _playerRef->Attrs.Spd);

    PrintRangedAttribute(1, yPos + 10, "HP", _playerRef->Attrs.HP);
    PrintRangedAttribute(1, yPos + 11, "MP", _playerRef->Attrs.MP);

    // Attributes are effectively right aligned in PrintAttribute(),
    // so no need for this hack anymore.

    //int maxLength = FindAttrsMaxStringLength();

    PrintModifiers(9, yPos + 3);

    // Skills

    Printer::Instance().PrintFB(kMaxNameUnderscoreLength / 2,
                                yPos + 13,
                                "SKILLS",
                                Printer::kAlignCenter,
                                Colors::WhiteColor);

    int yPrintOffset = 14;
    for (auto& kvp : _playerRef->SkillLevelBySkill)
    {
      std::string skillName = GlobalConstants::SkillNameByType.at(kvp.first);
      Printer::Instance().PrintFB(1,
                                  yPos + yPrintOffset,
                                  skillName,
                                  Printer::kAlignLeft,
                                  Colors::WhiteColor);
      yPrintOffset++;
    }

    yPrintOffset = 0;

    // Display some filler text just to give player some info
    if (_playerRef->_useIdentifiedItemsByIndex.empty())
    {
      Printer::Instance().PrintFB(_twHalf + _twQuarter,
                                  _thHalf,
                                  "No items to recall",
                                  Printer::kAlignCenter,
                                  Colors::WhiteColor);
    }

    // Use-identified items
    auto& items = _useIdentifiedMapCopy;
    for (size_t ind = _scrollIndex; ind < items.size(); ind++)
    {
      // Outside the screen
      if (yPrintOffset > _th)
      {
        break;
      }

      auto it = items.begin();
      std::advance(it, ind);
      std::string str = Util::StringFormat("%s - %s", it->second.first.data(), it->second.second.data());
      Printer::Instance().PrintFB(kMaxNameUnderscoreLength + 1,
                                  yPrintOffset,
                                  str,
                                  Printer::kAlignLeft,
                                  Colors::WhiteColor);
      yPrintOffset++;
    }

    DrawScrollBars();

    Printer::Instance().Render();
  }
}

void InfoState::DrawScrollBars()
{
  if (_useIdentifiedMapCopy.size() > (size_t)_th)
  {
    if (_scrollIndex == 0)
    {
      #ifdef USE_SDL
      Printer::Instance().PrintFB(_tw - 1, _th - 1, (int)NameCP437::DARROW_2, Colors::WhiteColor);
      #else
      Printer::Instance().PrintFB(_tw - 1, _th - 1, "\\/", Printer::kAlignRight, Colors::WhiteColor);
      #endif
    }
    else if (_scrollLimitReached)
    {
      #ifdef USE_SDL
      Printer::Instance().PrintFB(_tw - 1, 0, (int)NameCP437::UARROW_2, Colors::WhiteColor);
      #else
      Printer::Instance().PrintFB(_tw - 1, 0, "/\\", Printer::kAlignRight, Colors::WhiteColor);
      #endif
    }
    else
    {
      #ifdef USE_SDL
      Printer::Instance().PrintFB(_tw - 1, 0, (int)NameCP437::UARROW_2, Colors::WhiteColor);
      Printer::Instance().PrintFB(_tw - 1, _th - 1, (int)NameCP437::DARROW_2, Colors::WhiteColor);
      #else
      Printer::Instance().PrintFB(_tw - 1, 0, "/\\", Printer::kAlignRight, Colors::WhiteColor);
      Printer::Instance().PrintFB(_tw - 1, _th - 1, "\\/", Printer::kAlignRight, Colors::WhiteColor);
      #endif
    }
  }
}

void InfoState::PrintAttribute(int x, int y, const std::string& attrName, Attribute& attr)
{
  std::string color = Colors::WhiteColor;

  int modifiers = attr.GetModifiers();
  if (modifiers > 0)
  {
    color = Colors::GreenColor;
  }
  else if (modifiers < 0)
  {
    color = Colors::RedColor;
  }

  std::string attrPlaceholder = Util::StringFormat("%s:...", attrName.data());
  Printer::Instance().PrintFB(x, y, attrPlaceholder, Printer::kAlignLeft, Colors::ShadesOfGrey::Five);

  std::string text = Util::StringFormat("%i", attr.Get());

  Printer::Instance().PrintFB(x + attrPlaceholder.length() - text.length(), y, text, Printer::kAlignLeft, color);

  //text = Util::StringFormat("%s: %i", attrName.data(), attr.Get());
  //Printer::Instance().PrintFB(x, y, text, Printer::kAlignLeft, color);

  // Replace stat name back with white color (kinda hack)
  auto str = Util::StringFormat("%s:", attrName.data());
  Printer::Instance().PrintFB(x, y, str, Printer::kAlignLeft, Colors::WhiteColor);
}

void InfoState::PrintRangedAttribute(int x, int y, const std::string& attrName, RangedAttribute& attr)
{
  std::string color = Colors::WhiteColor;

  int modifiers = attr.Max().GetModifiers();
  if (modifiers > 0)
  {
    color = Colors::GreenColor;
  }
  else if (modifiers < 0)
  {
    color = Colors::RedColor;
  }

  std::string placeholder = Util::StringFormat("%s: ... / ...", attrName.data());
  Printer::Instance().PrintFB(x, y, placeholder, Printer::kAlignLeft, Colors::ShadesOfGrey::Five);

  //std::string text = Util::StringFormat("%s: %i / %i", attrName.data(), attr.Min().Get(), attr.Max().Get());
  //Printer::Instance().PrintFB(x, y, text, Printer::kAlignLeft, color);

  std::string minVal = Util::StringFormat("%i", attr.Min().Get());
  std::string maxVal = Util::StringFormat("%i", attr.Max().Get());

  Printer::Instance().PrintFB(x + placeholder.length() - 6 - minVal.length(), y, minVal, Printer::kAlignLeft, color);
  Printer::Instance().PrintFB(x + placeholder.length() - 3, y, maxVal, Printer::kAlignLeft, color);
  Printer::Instance().PrintFB(x + placeholder.length() - 5, y, '/', Colors::WhiteColor);

  // Replace stat name back with white color (kinda hack)
  auto str = Util::StringFormat("%s:", attrName.data());
  Printer::Instance().PrintFB(x, y, str, Printer::kAlignLeft, Colors::WhiteColor);
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

  std::string color = Colors::WhiteColor;
  std::string str;

  if (value < 0)
  {
    color = Colors::RedColor;
    str = Util::StringFormat("(%i)", value);
  }
  else if (value > 0)
  {
    color = Colors::GreenColor;
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
