#include "menu-state.h"
#include "application.h"
#include "printer.h"
#include "util.h"

void MenuState::Init()
{
  _titleX = _twHalf;
  _titleY = _thHalf / 2 - _title.size() / 2;

  _pictureX = _twHalf - _picture[0].length() / 2;
  _pictureY = _thHalf - _picture.size() / 4 + 1;

  _terminalSize = Util::StringFormat("terminal size: %ix%i", _tw, _th);

#ifdef USE_SDL
  _borderSize = { _tw - 1, _th - 1 };
#else
  _borderSize = { _tw, _th };
#endif

  PrepareGrassTiles();

  std::filesystem::path p = Strings::SaveFileName;
  _saveFileFound = std::filesystem::exists(p);
}

// =============================================================================

void MenuState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case VK_ENTER:
      Application::Instance().ChangeState(GameStates::SELECT_CLASS_STATE);
      break;

    case VK_CANCEL:
      Application::Instance().ChangeState(GameStates::EXIT_GAME);
      break;

    default:
      break;
  }
}

// =============================================================================

void MenuState::PrepareGrassTiles()
{
  int sx = _pictureX;
  int sy = _pictureY;

  int x = 0;
  int y = 0;

  uint32_t flowerColor = Colors::GrassDotColor;

  for (auto& line : _picture)
  {
    for (auto& c : line)
    {
      if (c == '.')
      {
        flowerColor = Colors::GrassDotColor;

        int colorChoice = RNG::Instance().RandomRange(0, 35);
        if      (colorChoice == 0) flowerColor = Colors::WhiteColor;
        else if (colorChoice == 1) flowerColor = Colors::DandelionYellowColor;
        else if (colorChoice == 2) flowerColor = Colors::RedPoppyColor;

        std::pair<int, int> key = { sx + x, sy + y };
        _grassColorByPosition.emplace(key, flowerColor);
      }

      x++;
    }

    x = 0;
    y++;
  }
}

// =============================================================================

void MenuState::DrawPicture()
{
  int sx = _pictureX;
  int sy = _pictureY;

  int x = 0;
  int y = 0;
  for (auto& line : _picture)
  {
    for (auto& c : line)
    {
      switch (c)
      {
        case '@':
        {
          char img = '@';

          #ifdef USE_SDL
          img = GlobalConstants::CP437IndexByType[NameCP437::FACE_2];
          #endif

          Printer::Instance().PrintFB(sx + x,
                                      sy + y,
                                      img,
                                      Colors::CyanColor,
                                      Colors::ShadesOfGrey::Eight);
        }
        break;

        case '#':
        {
          Printer::Instance().PrintFB(sx + x,
                                      sy + y,
                                      c,
                                      Colors::ShadesOfGrey::Four,
                                      Colors::ShadesOfGrey::Two);
        }
        break;

        case '.':
        {
          uint32_t& fgColor = _grassColorByPosition.at({ sx + x, sy + y });
          Printer::Instance().PrintFB(sx + x,
                                      sy + y,
                                      c,
                                      fgColor,
                                      Colors::GrassColor);
        }
        break;

        case 'T':
        {
          char img = c;

          #ifdef USE_SDL
          img = GlobalConstants::CP437IndexByType[NameCP437::CLUB];
          #endif

          Printer::Instance().PrintFB(sx + x,
                                      sy + y,
                                      img,
                                      Colors::GreenColor,
                                      Colors::BlackColor);
        }
        break;

        case '+':
        {
          Printer::Instance().PrintFB(sx + x,
                                      sy + y,
                                      c,
                                      Colors::WhiteColor,
                                      Colors::BlackColor);
        }
        break;

        case 'p':
        {
          Printer::Instance().PrintFB(sx + x,
                                      sy + y,
                                      '.',
                                      Colors::ShadesOfGrey::Ten,
                                      Colors::ShadesOfGrey::Eight);
        }
        break;
      }

      x++;
    }

    x = 0;
    y++;
  }
}

// =============================================================================

void MenuState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    DrawPicture();

    Printer::Instance().DrawWindow({ 0, 0 },
                                   _borderSize,
                                   "",
                                   Colors::BlackColor,
                                   Colors::BlackColor,
                                   Colors::WhiteColor,
                                   Colors::BlackColor,
                                   Colors::None);

    int yOffset = 0;
    for (auto& s : _title)
    {
      int xAlign = s.length() / 2;
      int xOffset = 0;
      for (auto& c : s)
      {
        if (c == '#')
        {
          Printer::Instance().PrintFB(_titleX - xAlign + xOffset,
                                      _titleY + yOffset,
                                      ' ',
                                      Colors::BlackColor,
                                      Colors::WhiteColor);
        }
        else if (c == 's')
        {
          Printer::Instance().PrintFB(_titleX - xAlign + xOffset,
                                      _titleY + yOffset,
                                      ' ',
                                      Colors::BlackColor,
                                      Colors::ShadesOfGrey::Three);
        }

        xOffset++;
      }

      yOffset++;
    }

    Printer::Instance().PrintFB(_twHalf,
                                _thHalf + _picture.size(),
                                _welcome,
                                Printer::kAlignCenter,
                                Colors::WhiteColor);

    if (_saveFileFound)
    {
      Printer::Instance().PrintFB(_twHalf,
                                  _thHalf + _picture.size() + 1,
                                  _savefilePresent,
                                  Printer::kAlignCenter,
                                  0x44FF44);
    }

    for (size_t i = 0; i < _signature.size(); i++)
    {
      Printer::Instance().PrintFB(_tw - 2,
                                  _th - 1 - (_signature.size() - i),
                                  _signature[i],
                                  Printer::kAlignRight,
                                  Colors::WhiteColor);
    }

    Printer::Instance().PrintFB(2,
                                _th -3,
                                _buildVersionText,
                                Printer::kAlignLeft,
                                Colors::WhiteColor);

    Printer::Instance().PrintFB(_twHalf,
                                _th - 2,
                                _builtWith,
                                Printer::kAlignCenter,
                                Colors::WhiteColor);

    Printer::Instance().PrintFB(2,
                                _th - 2,
                                _terminalSize,
                                Printer::kAlignLeft,
                                Colors::WhiteColor);

    Printer::Instance().Render();
  }
}
