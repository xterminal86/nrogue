#include "info-state.h"
#include "printer.h"
#include "application.h"
#include "util.h"

void InfoState::Prepare()
{
  _playerRef = &Game::gApp.PlayerInstance;
}

// =============================================================================

void InfoState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case VK_CANCEL:
      Game::gApp.ChangeState(GameStates::MAIN_STATE);
      break;
  }
}

// =============================================================================

void InfoState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Game::gPrnt.Clear();

    int yPos = 2;

    std::string title = Util::StringFormat("%s the %s",
                                           _playerRef->Name.data(),
                                           _playerRef->GetClassName().data());
    Game::gPrnt.PrintText(
      1,
      0,
      title,
      Printer::kAlignLeft,
      Colors::WhiteColor,
      Colors::BlackColor
    );

    int charToPrint = 0;

    #ifdef USE_SDL
    charToPrint = (int)NameCP437::HBAR_2;
    #else
    charToPrint = '=';
    #endif

    for (int i = 0; i < kMaxNameUnderscoreLength; i++)
    {
      Game::gPrnt.PrintChar(
        i,
        1,
        charToPrint,
        Colors::WhiteColor,
        Colors::BlackColor
      );

      Game::gPrnt.PrintChar(
        i,
        yPos + 12,
        charToPrint,
        Colors::WhiteColor,
        Colors::BlackColor
      );
    }

    #ifdef USE_SDL
    charToPrint = (int)NameCP437::VBAR_1;
    #else
    charToPrint = '|';
    #endif

    for (int y = 0; y < _th; y++)
    {
      Game::gPrnt.PrintChar(
        kMaxNameUnderscoreLength,
        y,
        charToPrint,
        Colors::WhiteColor,
        Colors::BlackColor
      );
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

    Game::gPrnt.PrintText(
      kMaxNameUnderscoreLength / 2,
      yPos + 13,
      "SKILLS",
      Printer::kAlignCenter,
      Colors::WhiteColor,
      Colors::BlackColor
    );

    int yPrintOffset = 14;
    for (auto& kvp : _playerRef->SkillLevelBySkill)
    {
      std::string skillName = GlobalConstants::SkillNameByType.at(kvp.first);
      Game::gPrnt.PrintText(
        1,
        yPos + yPrintOffset,
        skillName,
        Printer::kAlignLeft,
        Colors::WhiteColor,
        Colors::BlackColor
      );
      yPrintOffset++;
    }

    yPrintOffset = 0;

    Game::gPrnt.Render();
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

  Game::gPrnt.PrintText(
    x,
    y,
    placeholder,
    Printer::kAlignLeft,
    Colors::WhiteColor,
    Colors::BlackColor
  );

  std::string minVal = Util::StringFormat("%d",
                                          _playerRef->Attrs.Exp.Min().Get());

  std::string maxVal = Util::StringFormat("%d",
                                          _playerRef->Attrs.Exp.Max().Get());

  int xPos = x + placeholder.length() - digits - minVal.length() - 3;
  Game::gPrnt.PrintText(
    xPos,
    y,
    minVal,
    Printer::kAlignLeft,
    Colors::WhiteColor,
    Colors::BlackColor
  );

  Game::gPrnt.PrintText(
    x + placeholder.length() - digits,
    y,
    maxVal,
    Printer::kAlignLeft,
    Colors::WhiteColor,
    Colors::BlackColor
  );
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
  Game::gPrnt.PrintText(
    x,
    y,
    attrPlaceholder,
    Printer::kAlignLeft,
    Colors::ShadesOfGrey::Five,
    Colors::BlackColor
  );

  std::string text = Util::StringFormat("%d", attr.Get());

  Game::gPrnt.PrintText(
    x + attrPlaceholder.length() - text.length(),
    y,
    text,
    Printer::kAlignLeft,
    color,
    Colors::BlackColor
  );

  //text = Util::StringFormat("%s: %i", attrName.data(), attr.Get());
  //Game::gPrnt.PrintFB(x, y, text, Printer::kAlignLeft, color);

  //
  // Replace stat name back with white color (kinda hack)
  //
  std::string str = Util::StringFormat("%s:", attrName.data());
  Game::gPrnt.PrintText(
    x,
    y,
    str,
    Printer::kAlignLeft,
    Colors::WhiteColor,
    Colors::BlackColor
  );
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

  Game::gPrnt.PrintText(
    x,
    y,
    placeholder,
    Printer::kAlignLeft,
    Colors::ShadesOfGrey::Five,
    Colors::BlackColor
  );

  //std::string text = Util::StringFormat("%s: %i / %i",
  //                                      attrName.data(),
  //                                      attr.Min().Get(),
  //                                      attr.Max().Get());
  //Game::gPrnt.PrintFB(x, y, text, Printer::kAlignLeft, color);

  std::string minVal = Util::StringFormat("%d", attr.Min().Get());
  std::string maxVal = Util::StringFormat("%d", attr.Max().Get());

  int xPos = x + placeholder.length() - 6 - minVal.length();
  Game::gPrnt.PrintText(
    xPos,
    y,
    minVal,
    Printer::kAlignLeft,
    color,
    Colors::BlackColor
  );

  Game::gPrnt.PrintText(
    x + placeholder.length() - 3,
    y,
    maxVal,
    Printer::kAlignLeft,
    color,
    Colors::BlackColor
  );

  Game::gPrnt.PrintChar(
    x + placeholder.length() - 5,
    y,
    '/',
    Colors::WhiteColor,
    Colors::BlackColor
  );

  //
  // Replace stat name back with white color (kinda hack)
  //
  std::string str = Util::StringFormat("%s:", attrName.data());
  Game::gPrnt.PrintText(
    x,
    y,
    str,
    Printer::kAlignLeft,
    Colors::WhiteColor,
    Colors::BlackColor
  );
}

// =============================================================================

void InfoState::PrintModifiers(int x, int y)
{
  auto& playerRef = Game::gApp.PlayerInstance;

  int strMod = playerRef.Attrs.Str.GetModifiers();
  int defMod = playerRef.Attrs.Def.GetModifiers();
  int magMod = playerRef.Attrs.Mag.GetModifiers();
  int resMod = playerRef.Attrs.Res.GetModifiers();
  int sklMod = playerRef.Attrs.Skl.GetModifiers();
  int spdMod = playerRef.Attrs.Spd.GetModifiers();

  std::pair<uint32_t, std::string> res;

  res = GetModifierString(strMod);
  Game::gPrnt.PrintText(
    x,
    y,
    res.second,
    Printer::kAlignLeft,
    res.first,
    Colors::BlackColor
  );

  res = GetModifierString(defMod);
  Game::gPrnt.PrintText(
    x,
    y + 1,
    res.second,
    Printer::kAlignLeft,
    res.first,
    Colors::BlackColor
  );

  res = GetModifierString(magMod);
  Game::gPrnt.PrintText(
    x,
    y + 2,
    res.second,
    Printer::kAlignLeft,
    res.first,
    Colors::BlackColor
  );

  res = GetModifierString(resMod);
  Game::gPrnt.PrintText(
    x,
    y + 3,
    res.second,
    Printer::kAlignLeft,
    res.first,
    Colors::BlackColor
  );

  res = GetModifierString(sklMod);
  Game::gPrnt.PrintText(
    x,
    y + 4,
    res.second,
    Printer::kAlignLeft,
    res.first,
    Colors::BlackColor
  );

  res = GetModifierString(spdMod);
  Game::gPrnt.PrintText(
    x,
    y + 5,
    res.second,
    Printer::kAlignLeft,
    res.first,
    Colors::BlackColor
  );
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
    str = Util::StringFormat("(%d)", value);
  }
  else if (value > 0)
  {
    color = Colors::GreenColor;
    str = Util::StringFormat("(+%d)", value);
  }
  else if (value == 0)
  {
    str = Util::StringFormat("(+%d)", value);
  }

  res.first = color;
  res.second = str;

  return res;
}

// =============================================================================

int InfoState::FindAttrsMaxStringLength()
{
  auto& playerRef = Game::gApp.PlayerInstance;

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
