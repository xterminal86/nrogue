#include "enter-name-state.h"

#include "application.h"
#include "util.h"
#include "printer.h"

void EnterNameState::Prepare()
{
  _areEnteringName = true;

  _x = Printer::TerminalWidth / 2;
  _y = Printer::TerminalHeight / 2;

  _cursorPos = _y - 2;
}

void EnterNameState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case VK_TAB:
      _areEnteringName = !_areEnteringName;
      break;

    case VK_ENTER:
    {
      // Check if entered string is only spaces
      if (_nameEntered.find_first_not_of(' ') == std::string::npos)
      {
        _nameEntered.clear();
      }

      if (_nameEntered.length() == 0)
      {
        _nameEntered = Util::ChooseRandomName();
      }

      RNG::Instance().SetSeed(_seedEntered);

      Application::Instance().PlayerInstance.Name = _nameEntered;
      Application::Instance().PlayerInstance.ObjectName = _nameEntered;

      Application::Instance().ChangeState(GameStates::INTRO_STATE);
    }
    break;

    case VK_BACKSPACE:
    {
      if (_areEnteringName)
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
      }
    }
    break;

    default:
    {
      if (_keyPressed >= 32 && _keyPressed <= 126)
      {
        if (_areEnteringName && _nameEntered.length() < MaxNameLength - 3)
        {
          _nameEntered += (char)_keyPressed;
        }
        else if (!_areEnteringName && _seedEntered.length() < MaxSeedStringLength - 3)
        {
          _seedEntered += (char)_keyPressed;
        }
      }
    }
    break;
  }
}

void EnterNameState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    #ifdef USE_SDL
    Printer::Instance().DrawWindow({ _x - _maxNameHalf, _cursorPos - 2 },
                                   { MaxNameLength + 1, 4 },
                                   kEnterNameString,
                                   _areEnteringName ? "#FFFFFF" : "#000000",
                                   _areEnteringName ? GlobalConstants::MessageBoxHeaderBgColor : "#666666");

    Printer::Instance().DrawWindow({ _x - _maxSeedHalf, _cursorPos + 4 },
                                   { MaxSeedStringLength + 1, 4 },
                                   kEnterSeedString,
                                   !_areEnteringName ? "#FFFFFF" : "#000000",
                                   !_areEnteringName ? GlobalConstants::MessageBoxHeaderBgColor : "#666666");
    #else    
    auto border = Util::GetPerimeter(_x - _maxNameHalf,
                                      _cursorPos - 2,
                                      MaxNameLength,
                                      4,
                                      true);

    for (auto& i : border)
    {
      Printer::Instance().PrintFB(i.X, i.Y, '*', "#FFFFFF");
    }

    border = Util::GetPerimeter(_x - _maxSeedHalf,
                                 _cursorPos + 4,
                                 MaxSeedStringLength,
                                 4,
                                 true);

    for (auto& i : border)
    {
      Printer::Instance().PrintFB(i.X, i.Y, '*', "#FFFFFF");
    }

    Printer::Instance().PrintFB(_x, _cursorPos - 2, kEnterNameString, Printer::kAlignCenter, "#000000", "#FFFFFF");
    Printer::Instance().PrintFB(_x, _cursorPos + 4, kEnterSeedString, Printer::kAlignCenter, "#000000", "#FFFFFF");
    #endif

    Printer::Instance().PrintFB(_x - _maxNameHalf + 2, _cursorPos, _nameEntered, Printer::kAlignLeft, "#FFFFFF");
    Printer::Instance().PrintFB(_x - _maxSeedHalf + 2, _cursorPos + 6, _seedEntered, Printer::kAlignLeft, "#FFFFFF");

    if (_areEnteringName)
    {
      Printer::Instance().PrintFB(_x - _maxNameHalf + 2 + _nameEntered.length(), _cursorPos, ' ', "#000000", "#FFFFFF");
    }
    else
    {
      Printer::Instance().PrintFB(_x - _maxSeedHalf + 2 + _seedEntered.length(), _cursorPos + 6, ' ', "#000000", "#FFFFFF");
    }

    Printer::Instance().PrintFB(Printer::TerminalWidth / 2,
                                 Printer::TerminalHeight - 1,
                                 "'Tab' - change fields",
                                 Printer::kAlignCenter,
                                 "#FFFFFF");

    Printer::Instance().Render();
  }
}
