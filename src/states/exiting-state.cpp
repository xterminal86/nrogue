#include "exiting-state.h"

#include "printer.h"
#include "application.h"
#include "map.h"

void ExitingState::Init()
{
  _playerRef = &Game::gApp.PlayerInstance;
}

// =============================================================================

void ExitingState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case VK_CANCEL:
    case 'n':
    case 'N':
      Game::gPrnt.ShowLastMessage = false;
      Game::gApp.ChangeState(GameStates::MAIN_STATE);
      break;

    case 'y':
      Game::gApp.WriteObituary(false);
      Game::gApp.ChangeState(GameStates::EXIT_GAME);
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
    Game::gPrnt.Clear();

    _playerRef->CheckVisibility();

    Game::gMap.Draw();

    _playerRef->Draw();

    Game::gPrnt.PrintText(
      Printer::TerminalWidth - 1,
      Printer::TerminalHeight - 1,
      "Exit game? (y/n)",
      Printer::kAlignRight,
      Colors::WhiteColor,
      Colors::BlackColor
    );

    Game::gPrnt.Render();
  }
}
