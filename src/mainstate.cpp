#include "mainstate.h"

#include "application.h"
#include "map.h"
#include "printer.h"

void MainState::Init()
{
  _inputState = InputStateEnum::MOVE;

  _playerRef = &Application::Instance().PlayerInstance;
}

void MainState::HandleInput()
{
  if (_inputState == InputStateEnum::MOVE)
  {
    ProcessMovement();
  }
  else if (_inputState == InputStateEnum::LOOK)
  {
    ProcessLook();
  }  
  else if (_inputState == InputStateEnum::INTERACT)
  {
    ProcessInteraction();
  }
  else if (_inputState == InputStateEnum::EXIT_GAME)
  {
    ProcessExitGame();
  }
}

void MainState::Update()
{
  if (_inputState == InputStateEnum::MOVE)
  {
    DrawMovementState();
  }
  else if (_inputState == InputStateEnum::LOOK)
  {
    DrawLookState();
  }
  else if (_inputState == InputStateEnum::INTERACT)
  {
    DrawInteractionState();
  }  
  else if (_inputState == InputStateEnum::EXIT_GAME)
  {
    DrawExitGameState();
  }
}

void MainState::ProcessInteraction()
{
  _keyPressed = getch();

  bool dirSet = false;

  switch (_keyPressed)
  {
    case NUMPAD_7:
      dirSet = true;
      _cursorPosition.X -= 1;
      _cursorPosition.Y -= 1;
      break;

    case NUMPAD_8:
      dirSet = true;
      _cursorPosition.Y -= 1;
      break;

    case NUMPAD_9:
      dirSet = true;
      _cursorPosition.X += 1;
      _cursorPosition.Y -= 1;
      break;

    case NUMPAD_4:
      dirSet = true;
      _cursorPosition.X -= 1;
      break;

    case NUMPAD_6:
      dirSet = true;
      _cursorPosition.X += 1;
      break;

    case NUMPAD_1:
      dirSet = true;
      _cursorPosition.X -= 1;
      _cursorPosition.Y += 1;
      break;

    case NUMPAD_2:
      dirSet = true;
      _cursorPosition.Y += 1;
      break;

    case NUMPAD_3:
      dirSet = true;
      _cursorPosition.X += 1;
      _cursorPosition.Y += 1;
      break;

    case 'q':
      Printer::Instance().AddMessage("Cancelled");
      _inputState = InputStateEnum::MOVE;
      break;

    default:
      break;
  }

  if (dirSet)
  {
    auto res = Map::Instance().GetGameObjectsAtPosition(_cursorPosition.X, _cursorPosition.Y);
    if (res.size() != 0)
    {
      TryToInteractWithObject(res.back());
    }
    else
    {
      auto* cell = &Map::Instance().MapArray[_cursorPosition.X][_cursorPosition.Y];
      TryToInteractWithObject(cell);
    }
  }
}

void MainState::ProcessLook()
{
  _keyPressed = getch();  

  switch (_keyPressed)
  {
    case NUMPAD_7:
      MoveCursor(-1, -1);
      break;
    
    case NUMPAD_8:
      MoveCursor(0, -1);
      break;

    case NUMPAD_9:
      MoveCursor(1, -1);
      break;

    case NUMPAD_4:
      MoveCursor(-1, 0);
      break;

    case NUMPAD_6:
      MoveCursor(1, 0);
      break;

    case NUMPAD_1:
      MoveCursor(-1, 1);
      break;

    case NUMPAD_2:
      MoveCursor(0, 1);
      break;

    case NUMPAD_3:
      MoveCursor(1, 1);
      break;

    case 'q':
      _inputState = InputStateEnum::MOVE;
      break;

    default:
      break;
  }
}

void MainState::ProcessMovement()
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
      Printer::Instance().AddMessage("Waiting...");
      _playerTurnDone = true;
      break;

    case 'm':
      Application::Instance().ChangeState(Application::GameStates::SHOW_MESSAGES_STATE);
      break;

    case 'l':
      _cursorPosition.X = _playerRef->PosX;
      _cursorPosition.Y = _playerRef->PosY;
      _inputState = InputStateEnum::LOOK;
      break;

    case 'i':
      Printer::Instance().AddMessage("Interact in which direction?");
      _cursorPosition.X = _playerRef->PosX;
      _cursorPosition.Y = _playerRef->PosY;
      _inputState = InputStateEnum::INTERACT;
      break;

    case '@':
      Application::Instance().ChangeState(Application::GameStates::INFO_STATE);
      break;
      
    case '?':
      Application::Instance().ChangeState(Application::GameStates::SHOW_HELP_STATE);
      break;

    case 'q':
      _inputState = InputStateEnum::EXIT_GAME;
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

void MainState::DrawCursor()
{
  Printer::Instance().Print(_cursorPosition.X + Map::Instance().MapOffsetX + 1, 
                            _cursorPosition.Y + Map::Instance().MapOffsetY,
                            ']', "#FFFFFF");    

  Printer::Instance().Print(_cursorPosition.X + Map::Instance().MapOffsetX - 1, 
                            _cursorPosition.Y + Map::Instance().MapOffsetY,
                            '[', "#FFFFFF");      
}

void MainState::DrawLookState()
{
  if (_keyPressed != -1)
  {
    clear();

    Map::Instance().Draw(_playerRef->PosX, _playerRef->PosY);

    _playerRef->Draw();
    
    DrawCursor();   
    
    std::string lookStatus = "You see: ";

    if (Util::CheckLimits(_cursorPosition, Position(GlobalConstants::MapX, GlobalConstants::MapY)))
    {      
      auto* tile = &Map::Instance().MapArray[_cursorPosition.X][_cursorPosition.Y];
      if (_cursorPosition.X == _playerRef->PosX && _cursorPosition.Y == _playerRef->PosY)
      {
        lookStatus += "it's you!";
      }
      else if (!tile->Revealed)
      {
        lookStatus += "???";
      }
      else if (tile->Blocking)
      {
        auto nameHidden = "?" + tile->ObjectName + "?";
        lookStatus += tile->Visible ? tile->ObjectName : nameHidden;
      }
      else
      {
        auto gameObjects = Map::Instance().GetGameObjectsAtPosition(_cursorPosition.X, _cursorPosition.Y);

        if (tile->Visible && gameObjects.size() != 0)
        {
          lookStatus += gameObjects.back()->ObjectName;
        }
        else
        {
          auto nameHidden = "?" + tile->ObjectName + "?";
          lookStatus += tile->Visible ? tile->ObjectName : nameHidden;
        }
      }      
    }
    else
    {
      lookStatus += "???";
    }

    Printer::Instance().Print(Printer::Instance().TerminalWidth - 1,
                                Printer::Instance().TerminalHeight - 1,
                                lookStatus, Printer::kAlignRight, "#FFFFFF");

    refresh();  
  }
}

void MainState::DrawMovementState()
{
  if (_keyPressed != -1)
  {
    clear();
        
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

    Printer::Instance().Print(0, Printer::Instance().TerminalHeight - 1, _debugInfo, Printer::kAlignLeft, "#FFFFFF");

    DisplayGameLog();

    refresh();  
  }
}

void MainState::DrawInteractionState()
{
  if (_keyPressed != -1)
  {
    clear();

    _playerRef->CheckVisibility();

    Map::Instance().Draw(_playerRef->PosX, _playerRef->PosY);

    _playerRef->Draw();

    DisplayGameLog();

    refresh();
  }
}

void MainState::MoveCursor(int dx, int dy)
{
  int nx = _cursorPosition.X + dx;
  int ny = _cursorPosition.Y + dy;

  int hw = Printer::Instance().TerminalWidth / 2;
  int hh = Printer::Instance().TerminalHeight / 2;

  nx = Util::Clamp(nx, _playerRef->PosX - hw + 1,
                       _playerRef->PosX + hw - 2);

  ny = Util::Clamp(ny, _playerRef->PosY - hh + 1,
                       _playerRef->PosY + hh - 2);
  
  _cursorPosition.X = nx;
  _cursorPosition.Y = ny;
}

void MainState::DisplayGameLog()
{
  int x = Printer::Instance().TerminalWidth - 1;
  int y = Printer::Instance().TerminalHeight - 1;

  Printer::Instance().Print(x, y, Printer::Instance().GetLastMessage(), Printer::kAlignRight, "#FFFFFF");
}

void MainState::TryToInteractWithObject(GameObject* go)
{
  if (go->Interact())
  {
    _playerTurnDone = true;
    _inputState = InputStateEnum::MOVE;
    Printer::Instance().AddMessage("You interact with " + go->ObjectName);
  }
  else
  {
    Printer::Instance().AddMessage("Can't interact with " + go->ObjectName);
    _inputState = InputStateEnum::MOVE;
  }
}

void MainState::ProcessExitGame()
{
  _keyPressed = getch();

  switch (_keyPressed)
  {
    case 'y':
      Application::Instance().ChangeState(Application::GameStates::EXIT_GAME);
      break;

    case 'q':
      _inputState = InputStateEnum::MOVE;
      break;
  }
}

void MainState::DrawExitGameState()
{
  if (_keyPressed != -1)
  {
    clear();

    Map::Instance().Draw(_playerRef->PosX, _playerRef->PosY);

    _playerRef->Draw();

    int tw = Printer::Instance().TerminalWidth;
    int th = Printer::Instance().TerminalHeight;

    Printer::Instance().Print(tw - 1, th - 1, "Exit game? (y/q)", Printer::kAlignRight, "#FFFFFF");

    refresh();
  }
}
