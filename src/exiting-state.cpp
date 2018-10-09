#include "exiting-state.h"

#include "printer.h"
#include "application.h"
#include "map.h"

void ExitingState::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

void ExitingState::HandleInput()
{
  _keyPressed = getch();

  switch (_keyPressed)
  {
    case 'q':
      Application::Instance().ChangeState(Application::GameStates::MAIN_STATE);
      break;

    case 'y':
      Application::Instance().ChangeState(Application::GameStates::EXIT_GAME);
      break;
  }
}

void ExitingState::Update()
{
  if (_keyPressed != -1)
  {
    clear();

    _playerRef->CheckVisibility();

    Map::Instance().Draw(_playerRef->PosX, _playerRef->PosY);

    _playerRef->Draw();

    Printer::Instance().Print(Printer::Instance().TerminalWidth - 1,
                              Printer::Instance().TerminalHeight - 1,
                              "Exit game? (y/q)",
                              Printer::kAlignRight,
                              "#FFFFFF");

    refresh();
  }
}
