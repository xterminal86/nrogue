#include "mainstate.h"

#include "application.h"
#include "map.h"
#include "printer.h"
#include "rng.h"

void MainState::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;  
}

void MainState::HandleInput()
{
  _keyPressed = getch();

  _playerTurnDone = false;

  switch (_keyPressed)
  {
    case NUMPAD_7:
      if (_playerRef->Move(-1, -1))
      {
        Map::Instance().MapOffsetY++;
        Map::Instance().MapOffsetX++;

        _playerTurnDone = true;
      }
      break;

    case NUMPAD_8:
      if (_playerRef->Move(0, -1))
      {
        Map::Instance().MapOffsetY++;

        _playerTurnDone = true;
      }
      break;

    case NUMPAD_9:
      if (_playerRef->Move(1, -1))
      {
        Map::Instance().MapOffsetY++;
        Map::Instance().MapOffsetX--;

        _playerTurnDone = true;
      }
      break;

    case NUMPAD_4:
      if (_playerRef->Move(-1, 0))
      {
        Map::Instance().MapOffsetX++;

        _playerTurnDone = true;
      }
      break;

    case NUMPAD_2:
      if (_playerRef->Move(0, 1))
      {
        Map::Instance().MapOffsetY--;

        _playerTurnDone = true;
      }
      break;

    case NUMPAD_6:
      if (_playerRef->Move(1, 0))
      {
        Map::Instance().MapOffsetX--;

        _playerTurnDone = true;
      }
      break;

    case NUMPAD_1:
      if (_playerRef->Move(-1, 1))
      {
        Map::Instance().MapOffsetY--;
        Map::Instance().MapOffsetX++;

        _playerTurnDone = true;
      }
      break;

    case NUMPAD_3:
      if (_playerRef->Move(1, 1))
      {
        Map::Instance().MapOffsetY--;
        Map::Instance().MapOffsetX--;

        _playerTurnDone = true;
      }
      break;

    // wait
    case NUMPAD_5:
      Printer::Instance().AddMessage("You waited...");
      _playerTurnDone = true;
      break;

    case 'm':
      Application::Instance().ChangeState(Application::GameStates::SHOW_MESSAGES_STATE);
      break;

    case 'l':
      Application::Instance().ChangeState(Application::GameStates::LOOK_INPUT_STATE);
      break;

    case 'i':
      Application::Instance().ChangeState(Application::GameStates::INTERACT_INPUT_STATE);
      break;

    case '@':
      Application::Instance().ChangeState(Application::GameStates::INFO_STATE);
      break;

    case '?':
      Application::Instance().ChangeState(Application::GameStates::SHOW_HELP_STATE);
      break;

    case 'q':
      Application::Instance().ChangeState(Application::GameStates::EXITING_STATE);
      break;

    default:
      break;
  }

  // If player's turn finished, update all game objects' components
  if (_playerTurnDone)
  {
    Map::Instance().UpdateGameObjects();
  }
}

void MainState::Update()
{
  if (_keyPressed != -1)
  {
    Printer::Instance().Clear();

    _playerRef->CheckVisibility();

    Map::Instance().Draw(_playerRef->PosX, _playerRef->PosY);

    _playerRef->Draw();

    // Some debug info    
    _debugInfo = Util::StringFormat("Ofst: %i %i: Plr: [%i;%i] Key: %i",
                                    Map::Instance().MapOffsetX,
                                    Map::Instance().MapOffsetY,
                                    _playerRef->PosX,
                                    _playerRef->PosY,
                                    _keyPressed);

    Printer::Instance().PrintFB(0, Printer::Instance().TerminalHeight - 1, _debugInfo, Printer::kAlignLeft, "#FFFFFF");

    _debugInfo = Util::StringFormat("World seed: %lu", RNG::Instance().Seed);
    Printer::Instance().PrintFB(0, 0, _debugInfo, Printer::kAlignLeft, "#FFFFFF");

    DisplayGameLog();

    Printer::Instance().Render();
  }
}

void MainState::DisplayGameLog()
{
  int x = Printer::Instance().TerminalWidth - 1;
  int y = Printer::Instance().TerminalHeight - 1;

  Printer::Instance().PrintFB(x, y, Printer::Instance().GetLastMessage(), Printer::kAlignRight, "#FFFFFF");
}
