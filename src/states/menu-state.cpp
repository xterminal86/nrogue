#include "menu-state.h"
#include "application.h"
#include "printer.h"
#include "rng.h"
#include "util.h"

void MenuState::Init()
{
  _titleX = _twHalf;
  _titleY = _thHalf / 2 - _title.size() / 2;

  _pictureX = _twHalf - _picture[0].length() / 2;
  _pictureY = _thHalf - _picture.size() / 4 + 1;

  _terminalSize = Util::StringFormat("terminal size: %dx%d", _tw, _th);

#ifdef USE_SDL
  _borderSize = { _tw - 1, _th - 1 };
  _builtWith = Util::StringFormat(_builtWith.data(),
                                  SDL_MAJOR_VERSION,
                                  SDL_MINOR_VERSION,
                                  SDL_PATCHLEVEL);
#else
  _borderSize = { _tw, _th };
#endif

  PrepareGrassTiles();

  using Path = std::filesystem::path;

  Path p = Strings::SaveFileName;
  _saveFileFound = std::filesystem::exists(p);
}

// =============================================================================

void MenuState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case VK_ENTER:
      Game::gApp.ChangeState(GameStates::SELECT_CLASS_STATE);
      break;

    case 'L':
      DebugLog("FIXME: implement proper save/load");
      break;

    case VK_CANCEL:
      Game::gApp.ChangeState(GameStates::EXIT_GAME);
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

  uint32_t flowerColor = Colors::GrassDot;

  for (auto& line : _picture)
  {
    for (auto& c : line)
    {
      if (c == '.')
      {
        flowerColor = Colors::GrassDot;

        int colorChoice = Game::gRng.RandomRange(0, 35);
        if      (colorChoice == 0) flowerColor = Colors::White;
        else if (colorChoice == 1) flowerColor = Colors::DandelionYellow;
        else if (colorChoice == 2) flowerColor = Colors::RedPoppy;

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

          Game::gPrnt.PrintChar(
            sx + x,
            sy + y,
            img,
            Colors::Cyan,
            Colors::ShadesOfGrey::Eight
          );
        }
        break;

        case '#':
        {
          Game::gPrnt.PrintChar(
            sx + x,
            sy + y,
            c,
            Colors::ShadesOfGrey::Four,
            Colors::ShadesOfGrey::Two
          );
        }
        break;

        case '.':
        {
          uint32_t& fgColor = _grassColorByPosition.at({ sx + x, sy + y });
          Game::gPrnt.PrintChar(
            sx + x,
            sy + y,
            c,
            fgColor,
            Colors::Grass
          );
        }
        break;

        case 'T':
        {
          char img = c;

          #ifdef USE_SDL
          img = GlobalConstants::CP437IndexByType[NameCP437::CLUB];
          #endif

          Game::gPrnt.PrintChar(
            sx + x,
            sy + y,
            img,
            Colors::Green,
            Colors::Black
          );
        }
        break;

        case '+':
        {
          Game::gPrnt.PrintChar(
            sx + x,
            sy + y,
            c,
            Colors::White,
            Colors::Black
          );
        }
        break;

        case 'p':
        {
          Game::gPrnt.PrintChar(
            sx + x,
            sy + y,
            '.',
            Colors::ShadesOfGrey::Ten,
            Colors::ShadesOfGrey::Eight
          );
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
    Game::gPrnt.Clear();

    DrawPicture();

    Game::gPrnt.DrawWindow({ 0, 0 },
                            _borderSize,
                            "",
                            Colors::Black,
                            Colors::Black,
                            Colors::White,
                            Colors::Black,
                            Colors::None);

#ifdef USE_SDL
    Application::ApplicationData& ad = Game::gApp.AppData;

    Game::gPrnt.PrintTextExt(ad.WindowWidth / 2,
                             ad.GlyphHeightScaled,
                             "NROGUE",
                             Printer::kAlignCenter,
                             Colors::White,
                             Colors::None,
                             10.0,
                             12,
                             12);
#else
    int yOffset = 0;
    for (auto& s : _title)
    {
      int xAlign = s.length() / 2;
      int xOffset = 0;
      for (auto& c : s)
      {
        if (c == '#')
        {
          Game::gPrnt.PrintChar(
            _titleX - xAlign + xOffset,
            _titleY + yOffset,
            ' ',
            Colors::BlackColor,
            Colors::WhiteColor
          );
        }
        else if (c == 's')
        {
          Game::gPrnt.PrintChar(
            _titleX - xAlign + xOffset,
            _titleY + yOffset,
            ' ',
            Colors::BlackColor,
            Colors::ShadesOfGrey::Three
          );
        }

        xOffset++;
      }

      yOffset++;
    }
#endif

    Game::gPrnt.PrintText(
      _twHalf,
      _thHalf + _picture.size(),
      _welcome,
      Printer::kAlignCenter,
      Colors::White,
      Colors::Black
    );

    if (_saveFileFound)
    {
      Game::gPrnt.PrintText(
        _twHalf,
        _thHalf + _picture.size() + 1,
        _savefilePresent,
        Printer::kAlignCenter,
        0x44FF44,
        Colors::Black
      );
    }

    for (size_t i = 0; i < _signature.size(); i++)
    {
      Game::gPrnt.PrintText(
        _tw - 2,
        _th - 1 - (_signature.size() - i),
        _signature[i],
        Printer::kAlignRight,
        Colors::White,
        Colors::Black
      );
    }

    Game::gPrnt.PrintText(
      2,
      _th -3,
      _buildVersionText,
      Printer::kAlignLeft,
      Colors::White,
      Colors::Black
    );

    Game::gPrnt.PrintText(
      _twHalf,
      _th - 2,
      _builtWith,
      Printer::kAlignCenter,
      Colors::White,
      Colors::Black
    );

    Game::gPrnt.PrintText(
      2,
      _th - 2,
      _terminalSize,
      Printer::kAlignLeft,
      Colors::White,
      Colors::Black
    );

    Game::gPrnt.Render();
  }
}
