#include "intro-state.h"
#include "printer.h"

#include "application.h"

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

void IntroState::Update()
{
  auto elapsed = std::chrono::high_resolution_clock::now() - _lastTime;
  auto msPassed = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);

  if (msPassed > kTextPrintDelay)
  {
    _lastTime = std::chrono::high_resolution_clock::now();

    if (_textPositionCursor != _introStrings[0].length())
    {
      Printer::Instance().Print(_textPositionX, _textPositionY, _introStrings[0][_textPositionCursor], "#FFFFFF");

      _textPositionX++;

      if (_textPositionX > Printer::Instance().TerminalWidth)
      {
        _textPositionX = 0;
        _textPositionY++;
      }

      _textPositionCursor++;      
    }
    else
    {
      Printer::Instance().Print(Printer::Instance().TerminalWidth / 2, Printer::Instance().TerminalHeight - 1, "Press 'Enter' to continue", Printer::kAlignCenter, "#FFFFFF");
    }
  }

  refresh();
}
