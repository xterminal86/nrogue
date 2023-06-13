#include "exiting-state.h"

#include "printer.h"
#include "application.h"
#include "map.h"

void ExitingState::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

// =============================================================================

void ExitingState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case VK_CANCEL:
      Printer::Instance().ShowLastMessage = false;
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;

    case 'y':
      Application::Instance().WriteObituary(false);
      Application::Instance().ChangeState(GameStates::EXIT_GAME);
      break;

    default:
      break;
  }
}

// =============================================================================

void ExitingState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    _playerRef->CheckVisibility();

    Map::Instance().Draw();

    _playerRef->Draw();

    Printer::Instance().PrintFB(Printer::TerminalWidth - 1,
                                Printer::TerminalHeight - 1,
                                "Exit game? (y/q)",
                                Printer::kAlignRight,
                                Colors::WhiteColor);

    Printer::Instance().Render();
  }
}
