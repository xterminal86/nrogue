#include "intro-state.h"
#include "printer.h"

#include "application.h"

void IntroState::Prepare()
{
  Printer::Instance().Clear();

  _lastTime = std::chrono::high_resolution_clock::now();

  _textPositionCursor = 0;
  _textPositionX = Printer::Instance().TerminalWidth / 2;

  int textIndex = Application::Instance().PlayerInstance.SelectedClass;
  _textPositionY = Printer::Instance().TerminalHeight / 2 - _introStrings[textIndex].size() / 2;
}

void IntroState::HandleInput()
{
  _keyPressed = getch();

  switch (_keyPressed)
  {
    case 10:
      Application::Instance().ChangeState(Application::GameStates::MAIN_STATE);
      break;
  }
}

void IntroState::Update(bool forceUpdate)
{
  auto elapsed = std::chrono::high_resolution_clock::now() - _lastTime;
  auto msPassed = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);

  if (msPassed > kTextPrintDelay)
  {
    _lastTime = std::chrono::high_resolution_clock::now();

    int textIndex = Application::Instance().PlayerInstance.SelectedClass;
    if (_stringIndex != _introStrings[textIndex].size())
    {
      if (_textPositionCursor != _introStrings[textIndex][_stringIndex].length())
      {
        int len = _introStrings[textIndex][_stringIndex].length();

        Printer::Instance().PrintFB(_textPositionX - len / 2, _textPositionY, _introStrings[textIndex][_stringIndex][_textPositionCursor], "#FFFFFF");

        _textPositionX++;

        _textPositionCursor++;
      }
      else
      {
        _textPositionCursor = 0;

        _textPositionX = Printer::Instance().TerminalWidth / 2;
        _textPositionY++;

        _stringIndex++;
      }
    }
    else
    {
      Printer::Instance().PrintFB(Printer::Instance().TerminalWidth / 2, Printer::Instance().TerminalHeight - 1, "Press 'Enter' to continue", Printer::kAlignCenter, "#FFFFFF");
    }
  }

  Printer::Instance().Render();
}
