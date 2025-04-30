#include "info-state.h"
#include "printer.h"
#include "application.h"
#include "util.h"

void InfoState::Prepare()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

// =============================================================================

void InfoState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case VK_CANCEL:
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;
  }
}

// =============================================================================

void InfoState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    int yPos = 2;

    std::string title = Util::StringFormat("%s the %s",
                                           _playerRef->Name.data(),
                                           _playerRef->GetClassName().data());
    Printer::Instance().PrintFB(1,
                                0,
                                title,
                                Printer::kAlignLeft,
                                Colors::WhiteColor,
                                Colors::BlackColor);

    int charToPrint = 0;

    #ifdef USE_SDL
    charToPrint = (int)NameCP437::HBAR_2;
    #else
    charToPrint = '=';
    #endif

    for (int i = 0; i < kMaxNameUnderscoreLength; i++)
    {
      Printer::Instance().PrintFB(i,
                                  1,
                                  charToPrint,
                                  Colors::WhiteColor,
                                  Colors::BlackColor);

      Printer::Instance().PrintFB(i,
                                  yPos + 12,
                                  charToPrint,
                                  Colors::WhiteColor,
                                  Colors::BlackColor);
    }

    #ifdef USE_SDL
    charToPrint = (int)NameCP437::VBAR_1;
    #else
    charToPrint = '|';
    #endif

    for (int y = 0; y < _th; y++)
    {
      Printer::Instance().PrintFB(kMaxNameUnderscoreLength,
                                  y,
                                  charToPrint,
                                  Colors::WhiteColor,
                                  Colors::BlackColor);
    }

    PrintAttribute(1, yPos, "LVL", _playerRef->Attrs.Lvl);
    PrintExp(1, yPos + 1);

    PrintAttribute(1, yPos + 3, "STR", _playerRef->Attrs.Str);
    PrintAttribute(1, yPos + 4, "DEF", _playerRef->Attrs.Def);
    PrintAttribute(1, yPos + 5, "MAG", _playerRef->Attrs.Mag);
    PrintAttribute(1, yPos + 6, "RES", _playerRef->Attrs.Res);
    PrintAttribute(1, yPos + 7, "SKL", _playerRef->Attrs.Skl);
    PrintAttribute(1, yPos + 8, "SPD", _playerRef->Attrs.Spd);

    PrintRangedAttribute(1, yPos + 10, "HP", _playerRef->Attrs.HP);
    PrintRangedAttribute(1, yPos + 11, "MP", _playerRef->Attrs.MP);

    //
    // Attributes are effectively right aligned in PrintAttribute(),
    // so no need for this hack anymore.
    //

    //int maxLength = FindAttrsMaxStringLength();

    PrintModifiers(9, yPos + 3);

    // Skills

    Printer::Instance().PrintFB(kMaxNameUnderscoreLength / 2,
                                yPos + 13,
                                "SKILLS",
                                Printer::kAlignCenter,
                                Colors::WhiteColor,
                                Colors::BlackColor);

    int yPrintOffset = 14;
    for (auto& kvp : _playerRef->SkillLevelBySkill)
    {
      std::string skillName = GlobalConstants::SkillNameByType.at(kvp.first);
      Printer::Instance().PrintFB(1,
                                  yPos + yPrintOffset,
                                  skillName,
                                  Printer::kAlignLeft,
                                  Colors::WhiteColor,
                                  Colors::BlackColor);
      yPrintOffset++;
    }

    yPrintOffset = 0;

    Printer::Instance().Render();
  }
}

// =============================================================================

void InfoState::PrintExp(int x, int y)
{
  size_t digits = std::to_string(_playerRef->Attrs.Exp.Max().Get()).length();

  std::string dots(digits, ' ');
  std::string placeholder = Util::StringFormat("EXP: %s / %s",
                                               dots.data(),
                                               dots.data());

  Printer::Instance().PrintFB(x,
                              y,
                              placeholder,
                              Printer::kAlignLeft,
                              Colors::WhiteColor,
                              Colors::BlackColor);

  std::string minVal = Util::StringFormat("%i",
                                          _playerRef->Attrs.Exp.Min().Get());

  std::string maxVal = Util::StringFormat("%i",
                                          _playerRef->Attrs.Exp.Max().Get());

  int xPos = x + placeholder.length() - digits - minVal.length() - 3;
  Printer::Instance().PrintFB(xPos,
                              y,
                              minVal,
                              Printer::kAlignLeft,
                              Colors::WhiteColor,
                              Colors::BlackColor);

  Printer::Instance().PrintFB(x + placeholder.length() - digits,
                              y,
                              maxVal,
                              Printer::kAlignLeft,
                              Colors::WhiteColor,
                              Colors::BlackColor);
}

// =============================================================================

void InfoState::PrintAttribute(int x,
                               int y,
                               const std::string& attrName,
                               Attribute& attr)
{
  uint32_t color = Colors::WhiteColor;

  int modifiers = attr.GetModifiers();
  if (modifiers > 0)
  {
    color = Colors::GreenColor;
  }
  else if (modifiers < 0)
  {
    color = Colors::RedColor;
  }

  std::string attrPlaceholder = Util::StringFormat("%s:   ", attrName.data());
  Printer::Instance().PrintFB(x,
                              y,
                              attrPlaceholder,
                              Printer::kAlignLeft,
                              Colors::ShadesOfGrey::Five,
                              Colors::BlackColor);

  std::string text = Util::StringFormat("%i", attr.Get());

  Printer::Instance().PrintFB(x + attrPlaceholder.length() - text.length(),
                              y,
                              text,
                              Printer::kAlignLeft,
                              color,
                              Colors::BlackColor);

  //text = Util::StringFormat("%s: %i", attrName.data(), attr.Get());
  //Printer::Instance().PrintFB(x, y, text, Printer::kAlignLeft, color);

  //
  // Replace stat name back with white color (kinda hack)
  //
  auto str = Util::StringFormat("%s:", attrName.data());
  Printer::Instance().PrintFB(x,
                              y,
                              str,
                              Printer::kAlignLeft,
                              Colors::WhiteColor,
                              Colors::BlackColor);
}

// =============================================================================

void InfoState::PrintRangedAttribute(int x,
                                     int y,
                                     const std::string& attrName,
                                     RangedAttribute& attr)
{
  uint32_t color = Colors::WhiteColor;

  int modifiers = attr.Max().GetModifiers();
  if (modifiers > 0)
  {
    color = Colors::GreenColor;
  }
  else if (modifiers < 0)
  {
    color = Colors::RedColor;
  }

  std::string placeholder =
      Util::StringFormat("%s:     /    ", attrName.data());

  Printer::Instance().PrintFB(x,
                              y,
                              placeholder,
                              Printer::kAlignLeft,
                              Colors::ShadesOfGrey::Five,
                              Colors::BlackColor);

  //std::string text = Util::StringFormat("%s: %i / %i",
  //                                      attrName.data(),
  //                                      attr.Min().Get(),
  //                                      attr.Max().Get());
  //Printer::Instance().PrintFB(x, y, text, Printer::kAlignLeft, color);

  std::string minVal = Util::StringFormat("%i", attr.Min().Get());
  std::string maxVal = Util::StringFormat("%i", attr.Max().Get());

  int xPos = x + placeholder.length() - 6 - minVal.length();
  Printer::Instance().PrintFB(xPos,
                              y,
                              minVal,
                              Printer::kAlignLeft,
                              color,
                              Colors::BlackColor);

  Printer::Instance().PrintFB(x + placeholder.length() - 3,
                              y,
                              maxVal,
                              Printer::kAlignLeft,
                              color,
                              Colors::BlackColor);

  Printer::Instance().PrintFB(x + placeholder.length() - 5,
                              y,
                              '/',
                              Colors::WhiteColor,
                              Colors::BlackColor);

  //
  // Replace stat name back with white color (kinda hack)
  //
  auto str = Util::StringFormat("%s:", attrName.data());
  Printer::Instance().PrintFB(x,
                              y,
                              str,
                              Printer::kAlignLeft,
                              Colors::WhiteColor,
                              Colors::BlackColor);
}

// =============================================================================

void InfoState::PrintModifiers(int x, int y)
{
  auto& playerRef = Application::Instance().PlayerInstance;

  int strMod = playerRef.Attrs.Str.GetModifiers();
  int defMod = playerRef.Attrs.Def.GetModifiers();
  int magMod = playerRef.Attrs.Mag.GetModifiers();
  int resMod = playerRef.Attrs.Res.GetModifiers();
  int sklMod = playerRef.Attrs.Skl.GetModifiers();
  int spdMod = playerRef.Attrs.Spd.GetModifiers();

  std::pair<uint32_t, std::string> res;

  res = GetModifierString(strMod);
  Printer::Instance().PrintFB(x,
                              y,
                              res.second,
                              Printer::kAlignLeft,
                              res.first,
                              Colors::BlackColor);

  res = GetModifierString(defMod);
  Printer::Instance().PrintFB(x,
                              y + 1,
                              res.second,
                              Printer::kAlignLeft,
                              res.first,
                              Colors::BlackColor);

  res = GetModifierString(magMod);
  Printer::Instance().PrintFB(x,
                              y + 2,
                              res.second,
                              Printer::kAlignLeft,
                              res.first,
                              Colors::BlackColor);

  res = GetModifierString(resMod);
  Printer::Instance().PrintFB(x,
                              y + 3,
                              res.second,
                              Printer::kAlignLeft,
                              res.first,
                              Colors::BlackColor);

  res = GetModifierString(sklMod);
  Printer::Instance().PrintFB(x,
                              y + 4,
                              res.second,
                              Printer::kAlignLeft,
                              res.first,
                              Colors::BlackColor);

  res = GetModifierString(spdMod);
  Printer::Instance().PrintFB(x,
                              y + 5,
                              res.second,
                              Printer::kAlignLeft,
                              res.first,
                              Colors::BlackColor);
}

// =============================================================================

std::pair<uint32_t, std::string> InfoState::GetModifierString(int value)
{
  std::pair<uint32_t, std::string> res;

  uint32_t color = Colors::WhiteColor;
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

// =============================================================================

int InfoState::FindAttrsMaxStringLength()
{
  auto& playerRef = Application::Instance().PlayerInstance;

  std::vector<int> lengths;

  lengths.push_back(
        std::to_string(playerRef.Attrs.Str.OriginalValue()).length()
  );

  lengths.push_back(
        std::to_string(playerRef.Attrs.Def.OriginalValue()).length()
  );

  lengths.push_back(
        std::to_string(playerRef.Attrs.Mag.OriginalValue()).length()
  );

  lengths.push_back(
        std::to_string(playerRef.Attrs.Res.OriginalValue()).length()
  );

  lengths.push_back(
        std::to_string(playerRef.Attrs.Skl.OriginalValue()).length()
  );

  lengths.push_back(
        std::to_string(playerRef.Attrs.Spd.OriginalValue()).length()
  );

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
