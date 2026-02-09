#include "enter-name-state.h"

#include "application.h"
#include "util.h"
#include "printer.h"

void EnterNameState::Prepare()
{
  _inputTypeIndex = 0;

  _x = _twHalf;
  _y = _thHalf;

  _cursorPos = _y - 2;

#ifdef USE_SDL
  _windowSize = { GlobalConstants::MaxNameLength + 1, 4 };
#else
  _windowSize = { GlobalConstants::MaxNameLength + 2, 4 };
#endif
}

// =============================================================================

void EnterNameState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case VK_TAB:
    {
      _inputTypeIndex++;
      _inputTypeIndex %= (size_t)(InputType::ENUM_END);

      _inputType = _inputTypeByIndex.at(_inputTypeIndex);

      _seedEntered.clear();
      _seedHex = 0;
      _seedConverted = false;
    }
    break;

    case VK_ENTER:
    {
      if (_nameEntered.find_first_not_of(' ') == std::string::npos)
      {
        _nameEntered.clear();
      }

      if (_nameEntered.length() == 0)
      {
        _nameEntered = Util::ChooseRandomName();
      }

      if (_seedConverted)
      {
        Game::gRng.SetSeed(_seedHex);
      }
      else
      {
        Game::gRng.SetSeed(_seedEntered);
      }

      Game::gApp.PlayerInstance.Name       = _nameEntered;
      Game::gApp.PlayerInstance.ObjectName = _nameEntered;

      Game::gApp.ChangeState(GameStates::INTRO_STATE);
    }
    break;

    case VK_BACKSPACE:
    {
      if (_inputType == InputType::NAME)
      {
        if (_nameEntered.length() > 0)
        {
          _nameEntered.pop_back();
        }
      }
      else
      {
        if (_seedEntered.length() > 0)
        {
          _seedEntered.pop_back();
        }

        UpdateSeedAsHex();
      }
    }
    break;

    default:
    {
      if (_keyPressed >= ' ' && _keyPressed <= '~')
      {
        switch(_inputType)
        {
          case InputType::NAME:
          {
            if (_nameEntered.length() <
                (size_t)GlobalConstants::MaxNameLength - 3)
            {
              _nameEntered += (char)_keyPressed;
            }
          }
          break;

          case InputType::SEED_STRING:
          {
            if (_seedEntered.length() <
                (size_t)GlobalConstants::MaxSeedStringLength - 2)
            {
              _seedEntered += (char)_keyPressed;

              UpdateSeedAsHex();
            }
          }
          break;

          case InputType::SEED_HEX:
          {
            if (_seedEntered.length() < sizeof(Game::gRng.Seed) * 2)
            {
              bool isNumber   = (_keyPressed >= '0' && _keyPressed <= '9');
              bool isHexUpper = (_keyPressed >= 'A' && _keyPressed <= 'F');
              bool isHexLower = (_keyPressed >= 'a' && _keyPressed <= 'f');

              bool isValid = isNumber || isHexUpper || isHexLower;

              if (isValid)
              {
                char c = (char)_keyPressed;

                if (isHexLower)
                {
                  c = std::toupper(c);
                }

                _seedEntered += c;

                _seedHex = std::stoull(_seedEntered, nullptr, 16);

                _seedConverted = true;
              }
            }
          }
          break;
        }
      }
    }
    break;
  }
}

// =============================================================================

void EnterNameState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Game::gPrnt.Clear();

    Game::gPrnt.DrawWindow(
      { _x - _maxNameHalf, _cursorPos - 2 },
      _windowSize,
      kEnterNameString,
      (_inputType == InputType::NAME)
      ? Colors::White
      : Colors::Black,
      (_inputType == InputType::NAME)
      ? Colors::MessageBoxHeaderBg
      : Colors::ShadesOfGrey::Six
    );

    Game::gPrnt.DrawWindow(
      { _x - _maxSeedHalf, _cursorPos + 4 },
      { GlobalConstants::MaxSeedStringLength + 2, 6 },
      kEnterSeedString,
      (_inputType != InputType::NAME)
      ? Colors::White
      : Colors::Black,
      (_inputType != InputType::NAME)
      ? Colors::MessageBoxHeaderBg
      : Colors::ShadesOfGrey::Six
    );

    //
    // Seed string hint.
    //
    for (int i = 0; i < GlobalConstants::MaxSeedStringLength - 2; i++)
    {
      Game::gPrnt.PrintChar(_x - _maxSeedHalf + 2 + i,
                            _cursorPos + 6,
                            '.',
                            Colors::ShadesOfGrey::Four,
                            Colors::Black);
    }

    //
    // Seed number hint.
    //
    for (size_t i = 0; i < sizeof(Game::gRng.Seed) * 2; i++)
    {
      Game::gPrnt.PrintChar(_x - _maxSeedHalf + 4 + i,
                            _cursorPos + 8,
                            '.',
                            Colors::ShadesOfGrey::Four,
                            Colors::Black);
    }

    Game::gPrnt.PrintText(_x - _maxNameHalf + 2,
                          _cursorPos,
                          _nameEntered,
                          Printer::kAlignLeft,
                          Colors::White,
                          Colors::Black);

    Game::gPrnt.PrintText(_x - _maxSeedHalf + 2,
                          _cursorPos + 8,
                          "0x",
                          Printer::kAlignLeft,
                          Colors::White,
                          Colors::Black);

    if (_inputType == InputType::NAME)
    {
      Game::gPrnt.PrintChar(_x - _maxNameHalf + 2 + _nameEntered.length(),
                            _cursorPos,
                            ' ',
                            Colors::Black,
                            Colors::White);
    }
    else if (_inputType == InputType::SEED_STRING)
    {
      Game::gPrnt.PrintChar(_x - _maxSeedHalf + 2 + _seedEntered.length(),
                            _cursorPos + 6,
                            ' ',
                            Colors::Black,
                            Colors::White);

      Game::gPrnt.PrintText(_x - _maxSeedHalf + 2,
                            _cursorPos + 6,
                            _seedEntered,
                            Printer::kAlignLeft,
                            Colors::White,
                            Colors::Black);

      if (!_seedEntered.empty())
      {
        Game::gPrnt.PrintText(_x - _maxSeedHalf + 4,
                              _cursorPos + 8,
                              _seedAsHex,
                              Printer::kAlignLeft,
                              Colors::White,
                              Colors::Black);
      }
    }
    else if (_inputType == InputType::SEED_HEX)
    {
      Game::gPrnt.PrintChar(_x - _maxSeedHalf + 4 + _seedEntered.length(),
                            _cursorPos + 8,
                            ' ',
                            Colors::Black,
                            Colors::White);

      Game::gPrnt.PrintText(_x - _maxSeedHalf + 4,
                            _cursorPos + 8,
                            _seedEntered,
                            Printer::kAlignLeft,
                            Colors::White,
                            Colors::Black);
    }

    Game::gPrnt.PrintText(_twHalf,
                          Printer::TerminalHeight - 1,
                          "'Tab' - change fields",
                          Printer::kAlignCenter,
                          Colors::White,
                          Colors::Black);

    Game::gPrnt.Render();
  }
}

// =============================================================================

void EnterNameState::UpdateSeedAsHex()
{
  Game::gRng.SetSeed(_seedEntered);
  _seedAsHex = Game::gRng.GetSeedAsHex();
}
