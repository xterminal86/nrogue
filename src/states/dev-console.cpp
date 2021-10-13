#include "dev-console.h"

#include "application.h"
#include "printer.h"

void DevConsole::Init()
{
}

void DevConsole::Prepare()
{
  _toEnter = "> ";
}

void DevConsole::Cleanup()
{
  _toEnter.clear();
}

void DevConsole::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case '`':
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;

    case VK_ENTER:
    {
      _entered.insert(_entered.begin(), _toEnter);
      _toEnter = "> ";
      _cursorX = 1;
      _cursorY++;

      if (_entered.size() > 15)
      {
        _entered.pop_back();
      }
    }
    break;

    case VK_BACKSPACE:
    {
      if (_toEnter.length() > 2)
      {
        _toEnter.pop_back();
      }
    }
    break;

    default:
    {
      if (_keyPressed >= 32 && _keyPressed <= 126)
      {
        _toEnter += (char)_keyPressed;
      }
    }
    break;
  }
}

void DevConsole::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    Printer::Instance().PrintFB(_twHalf, 0, "DEVELOPER'S CONSOLE", Printer::kAlignCenter, "#FFFFFF");

    int lineCount = 0;
    for (int i = _entered.size() - 1; i >= 0; i--)
    {
      Printer::Instance().PrintFB(1, 2 + lineCount, _entered[i], Printer::kAlignLeft, "#FFFFFF");
      lineCount++;
    }

    Printer::Instance().PrintFB(1, 2 + lineCount, _toEnter, Printer::kAlignLeft, "#FFFFFF");
    Printer::Instance().PrintFB(1 + _toEnter.length(), 2 + lineCount, ' ', "#000000", "#FFFFFF");

    Printer::Instance().Render();
  }
}

