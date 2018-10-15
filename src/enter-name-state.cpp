#include "enter-name-state.h"

#include "application.h"

void EnterNameState::HandleInput()
{
  _keyPressed = getch();

  if (_keyPressed != -1)
  {
    switch (_keyPressed)
    {
      case 10:
        if (_nameEntered.length() == 0)
        {
          _nameEntered = Util::ChooseRandomName();
        }

        Application::Instance().PlayerInstance.Name = _nameEntered;
        Application::Instance().ChangeState(Application::GameStates::INTRO_STATE);
        break;

      // KEY_BACKSPACE doesn't work
      case 127:
        if (_nameEntered.length() > 0)
        {
          _nameEntered.pop_back();
        }
        break;

      default:
        if (_nameEntered.length() < kMaxNameLength - 3)
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

    Printer::Instance().PrintFB(x, y - 2, "What is your name?", Printer::kAlignCenter, "#FFFFFF");

    auto border = Util::GetPerimeter(x - kMaxNameLength / 2, y - 3, kMaxNameLength, 5, true);

    for (auto& i : border)
    {
      Printer::Instance().PrintFB(i.X, i.Y, '*', "#FFFFFF");
    }

    if (_nameEntered.length() == 0)
    {
      Printer::Instance().PrintFB(x - kMaxNameLength / 2 + 2, y, ' ', "#000000", "#FFFFFF");
    }
    else
    {
      Printer::Instance().PrintFB(x - kMaxNameLength / 2 + 2, y, _nameEntered, Printer::kAlignLeft, "#FFFFFF");
      Printer::Instance().PrintFB(x - kMaxNameLength / 2 + 2 + _nameEntered.length(), y, ' ', "#000000", "#FFFFFF");
    }

    Printer::Instance().Render();
  }
}
