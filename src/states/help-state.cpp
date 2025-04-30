#include "help-state.h"

#include "application.h"
#include "printer.h"
#include "util.h"

HelpState::HelpState()
{
  std::string s = Util::StringFormat("'%c' '%c' '%c'", ALT_K7, ALT_K8, ALT_K9);

  _keymap[0] = s;

  s = Util::StringFormat("'%c' '%c' '%c' or numpad for movement",
                         ALT_K4,
                         ALT_K5,
                         ALT_K6);
  _keymap[2] = s;

  s = Util::StringFormat("'%c' '%c' '%c'", ALT_K1, ALT_K2, ALT_K3);
  _keymap[4] = s;

  for (auto& line : _keymap)
  {
    _helpText.push_back(line);
  }
}

// =============================================================================

void HelpState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case '?':
    case 'h':
    case 'H':
    case VK_CANCEL:
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;

    default:
      break;
  }
}

// =============================================================================

void HelpState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    DrawHeader(" HELP ");

    int offsetY = 1;
    for (size_t i = 0; i < _helpText.size(); i++)
    {
      Printer::Instance().PrintFB(1,
                                  offsetY,
                                  _helpText[i],
                                  Printer::kAlignLeft,
                                  Colors::WhiteColor,
                                  Colors::BlackColor);
      offsetY++;
    }

    #ifdef USE_SDL
    Printer::Instance().PrintFB(Printer::TerminalWidth - 1,
                                1,
                                _specialText,
                                Printer::kAlignRight,
                                Colors::WhiteColor,
                                Colors::BlackColor);
    #endif

    Printer::Instance().Render();
  }
}
