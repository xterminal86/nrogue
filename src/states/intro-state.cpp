#include "intro-state.h"
#include "printer.h"

#include "application.h"
#include "map.h"

void IntroState::Prepare()
{
  Printer::Instance().Clear();

  _textPositionCursor = 0;
  _textPositionX = _twHalf;

  int textIndex = Application::Instance().PlayerInstance.SelectedClass;
  _textPositionY = _thHalf - _introStrings[textIndex].size() / 2;

  Util::WaitForMs(0, true);
}

// =============================================================================

void IntroState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case VK_ENTER:
      Application::Instance().ChangeState(GameStates::START_GAME_STATE);
      break;

    default:
      break;
  }
}

// =============================================================================

void IntroState::Update(bool forceUpdate)
{
  int pci = Application::Instance().PlayerInstance.SelectedClass;
  PlayerClass pc = Application::Instance().PlayerInstance.GetClass();

#ifdef USE_SDL
  Printer::Instance().PrintFB(
        _twHalf,
        (Printer::TerminalHeight - _introStrings[pci].size()) / 4,
        _scenarioNameByClass.at(pc),
        2,
        Printer::kAlignCenter,
        Colors::WhiteColor,
        Colors::BlackColor
  );
#else
  Printer::Instance().PrintFB(
        _twHalf,
        (Printer::TerminalHeight - _introStrings[pci].size()) / 4,
        _scenarioNameByClass.at(pc),
        Printer::kAlignCenter,
        Colors::WhiteColor,
        Colors::BlackColor
  );
#endif

  if (Util::WaitForMs(10))
  {
    int textIndex = Application::Instance().PlayerInstance.SelectedClass;
    if (_stringIndex != _introStrings[textIndex].size())
    {
      const std::string& s = _introStrings[textIndex][_stringIndex];
      int len = s.length();

      if ((int)_textPositionCursor != len)
      {
        Printer::Instance().PrintFB(_textPositionX - len / 2,
                                    _textPositionY,
                                    s[_textPositionCursor],
                                    Colors::WhiteColor,
                                    Colors::BlackColor);

        Printer::Instance().PrintFB(_textPositionX - len / 2 + 1,
                                    _textPositionY,
                                    ' ',
                                    Colors::BlackColor,
                                    Colors::WhiteColor);

        _textPositionX++;
        _textPositionCursor++;
      }
      else
      {
        Printer::Instance().PrintFB(_textPositionX - len / 2,
                                    _textPositionY,
                                    ' ',
                                    Colors::BlackColor,
                                    Colors::BlackColor);

        _textPositionCursor = 0;

        _textPositionX = _twHalf;
        _textPositionY++;

        _stringIndex++;
      }
    }
    else
    {
      Printer::Instance().PrintFB(_twHalf,
                                  Printer::TerminalHeight - 1,
                                  "Press 'Enter' to continue",
                                  Printer::kAlignCenter,
                                  Colors::WhiteColor,
                                  Colors::BlackColor);
    }

    Printer::Instance().Render();
  }
}
