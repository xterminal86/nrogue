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
  _windowSize = { MaxNameLength + 1, 4 };
#else
  _windowSize = { MaxNameLength + 2, 4 };
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
      _inputTypeIndex %= static_cast<size_t>(InputType::ENUM_END);

      _inputType = _inputTypeByIndex.at(_inputTypeIndex);

      _seedEntered.clear();
      _seedHex = 0;
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

      if (_seedHex != 0)
      {
        RNG::Instance().SetSeed(_seedHex);
      }
      else
      {
        RNG::Instance().SetSeed(_seedEntered);
      }

      Application::Instance().PlayerInstance.Name       = _nameEntered;
      Application::Instance().PlayerInstance.ObjectName = _nameEntered;

      Application::Instance().ChangeState(GameStates::INTRO_STATE);
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
      // Don't include special characters
      if (_keyPressed >= 32 && _keyPressed <= 126)
      {
        if (_inputType == InputType::NAME && _nameEntered.length() < (size_t)MaxNameLength - 3)
        {
          _nameEntered += (char)_keyPressed;
        }
        else if (_inputType == InputType::SEED_STRING && _seedEntered.length() < (size_t)MaxSeedStringLength - 2)
        {
          _seedEntered += (char)_keyPressed;

          UpdateSeedAsHex();
        }
        else if (_inputType == InputType::SEED_HEX && _seedEntered.length() < sizeof(RNG::Instance().Seed) * 2)
        {
          bool isNumber   = (_keyPressed >= 48 && _keyPressed <= 57);
          bool isHexUpper = (_keyPressed >= 65 && _keyPressed <= 70);
          bool isHexLower = (_keyPressed >= 97 && _keyPressed <= 102);

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

            DebugLog("Seed entered: 0x%lX", _seedHex);
          }
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
    Printer::Instance().Clear();

    Printer::Instance().DrawWindow({ _x - _maxNameHalf, _cursorPos - 2 },
                                   _windowSize,
                                   kEnterNameString,
                                   (_inputType == InputType::NAME) ? Colors::WhiteColor : Colors::BlackColor,
                                   (_inputType == InputType::NAME) ? Colors::MessageBoxHeaderBgColor : Colors::ShadesOfGrey::Six);

    Printer::Instance().DrawWindow({ _x - _maxSeedHalf, _cursorPos + 4 },
                                   { MaxSeedStringLength + 2, 6 },
                                   kEnterSeedString,
                                   (_inputType != InputType::NAME) ? Colors::WhiteColor : Colors::BlackColor,
                                   (_inputType != InputType::NAME) ? Colors::MessageBoxHeaderBgColor : Colors::ShadesOfGrey::Six);

    // Seed string hint
    for (int i = 0; i < MaxSeedStringLength - 2; i++)
    {
      Printer::Instance().PrintFB(_x - _maxSeedHalf + 2 + i,
                                  _cursorPos + 6,
                                  '.',
                                  Colors::ShadesOfGrey::Four);
    }

    // Seed number hint
    for (size_t i = 0; i < sizeof(RNG::Instance().Seed) * 2; i++)
    {
      Printer::Instance().PrintFB(_x - _maxSeedHalf + 4 + i,
                                  _cursorPos + 8,
                                  '.',
                                  Colors::ShadesOfGrey::Four);
    }

    Printer::Instance().PrintFB(_x - _maxNameHalf + 2,
                                _cursorPos,
                                _nameEntered,
                                Printer::kAlignLeft,
                                Colors::WhiteColor);

    Printer::Instance().PrintFB(_x - _maxSeedHalf + 2,
                                _cursorPos + 8,
                                "0x",
                                Printer::kAlignLeft,
                                Colors::WhiteColor);

    if (_inputType == InputType::NAME)
    {
      Printer::Instance().PrintFB(_x - _maxNameHalf + 2 + _nameEntered.length(),
                                  _cursorPos,
                                  ' ',
                                  Colors::BlackColor,
                                  Colors::WhiteColor);
    }
    else if (_inputType == InputType::SEED_STRING)
    {
      Printer::Instance().PrintFB(_x - _maxSeedHalf + 2 + _seedEntered.length(),
                                  _cursorPos + 6,
                                  ' ',
                                  Colors::BlackColor,
                                  Colors::WhiteColor);

      Printer::Instance().PrintFB(_x - _maxSeedHalf + 2,
                                  _cursorPos + 6,
                                  _seedEntered,
                                  Printer::kAlignLeft,
                                  Colors::WhiteColor);

      if (!_seedEntered.empty())
      {
        Printer::Instance().PrintFB(_x - _maxSeedHalf + 4,
                                    _cursorPos + 8,
                                    _seedAsHex,
                                    Printer::kAlignLeft,
                                    Colors::WhiteColor);
      }
    }
    else if (_inputType == InputType::SEED_HEX)
    {
      Printer::Instance().PrintFB(_x - _maxSeedHalf + 4 + _seedEntered.length(),
                                  _cursorPos + 8,
                                  ' ',
                                  Colors::BlackColor,
                                  Colors::WhiteColor);

      Printer::Instance().PrintFB(_x - _maxSeedHalf + 4,
                                  _cursorPos + 8,
                                  _seedEntered,
                                  Printer::kAlignLeft,
                                  Colors::WhiteColor);
    }

    Printer::Instance().PrintFB(_twHalf,
                                Printer::TerminalHeight - 1,
                                "'Tab' - change fields",
                                Printer::kAlignCenter,
                                Colors::WhiteColor);

    Printer::Instance().Render();
  }
}

// =============================================================================

void EnterNameState::UpdateSeedAsHex()
{
  RNG::Instance().SetSeed(_seedEntered);
  _seedAsHex = RNG::Instance().GetSeedAsHex();
}
