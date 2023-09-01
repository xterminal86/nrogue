#include "save-game-state.h"

#include "application.h"
#include "printer.h"
#include "map.h"

void SaveGameState::Prepare()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

// =============================================================================

void SaveGameState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case VK_CANCEL:
    case 'n':
    case 'N':
      Printer::Instance().ShowLastMessage = false;
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;

    case 'y':
      Application::Instance().SaveGame(true);
      Application::Instance().ChangeState(GameStates::EXIT_GAME);
      break;

    default:
      break;
  }
}

// =============================================================================

void SaveGameState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    _playerRef->CheckVisibility();

    Map::Instance().Draw();

    _playerRef->Draw();

    Printer::Instance().PrintFB(Printer::TerminalWidth - 1,
                                Printer::TerminalHeight - 1,
                                "Save and quit? (y/q)",
                                Printer::kAlignRight,
                                Colors::WhiteColor);

    Printer::Instance().Render();
  }
}
