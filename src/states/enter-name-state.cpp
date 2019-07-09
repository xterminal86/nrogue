#include "enter-name-state.h"

#include "application.h"
#include "util.h"

void EnterNameState::HandleInput()
{
  _keyPressed = GetKeyDown();

  if (_keyPressed != -1)
  {
    switch (_keyPressed)
    {
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

        Application::Instance().PlayerInstance.Name = _nameEntered;
        Application::Instance().PlayerInstance.ObjectName = _nameEntered;

        Application::Instance().ChangeState(GameStates::INTRO_STATE);
      }
      break;

      case VK_BACKSPACE:
        if (_nameEntered.length() > 0)
        {
          _nameEntered.pop_back();
        }
        break;

      default:
        if (_keyPressed >= 32 &&
            _keyPressed <= 126 &&
            _nameEntered.length() < GlobalConstants::MaxNameLength - 3)
        {
          _nameEntered += (char)_keyPressed;
        }
        break;
    }
  }
}

void EnterNameState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    int x = Printer::Instance().TerminalWidth / 2;
    int y = Printer::Instance().TerminalHeight / 2;

    #ifdef USE_SDL
    Printer::Instance().DrawWindow({ x - GlobalConstants::MaxNameLength / 2, y - 2 },
                                   { GlobalConstants::MaxNameLength + 1, 4 },
                                   kHeaderString,
                                   "#FFFFFF",
                                   GlobalConstants::MessageBoxHeaderBgColor);
    #else
    Printer::Instance().PrintFB(x, y - 2, kHeaderString, Printer::kAlignCenter, "#FFFFFF");

    auto border = Util::GetPerimeter(x - GlobalConstants::MaxNameLength / 2,
                                     y - 3,
                                     GlobalConstants::MaxNameLength,
                                     5, true);

    for (auto& i : border)
    {
      Printer::Instance().PrintFB(i.X, i.Y, '*', "#FFFFFF");
    }
    #endif

    if (_nameEntered.length() == 0)
    {
      Printer::Instance().PrintFB(x - GlobalConstants::MaxNameLength / 2 + 2, y, ' ', "#000000", "#FFFFFF");
    }
    else
    {
      Printer::Instance().PrintFB(x - GlobalConstants::MaxNameLength / 2 + 2, y, _nameEntered, Printer::kAlignLeft, "#FFFFFF");
      Printer::Instance().PrintFB(x - GlobalConstants::MaxNameLength / 2 + 2 + _nameEntered.length(), y, ' ', "#000000", "#FFFFFF");
    }

    Printer::Instance().Render();
  }
}
