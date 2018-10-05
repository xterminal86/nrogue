#include "mainstate.h"

#include "application.h"
#include "map.h"
#include "printer.h"

void MainState::Init()
{
  _inputState = "move";

  _playerRef = &Application::Instance().PlayerInstance;
}

void MainState::HandleInput()
{
  if (_inputState == kInputMoveState)
  {
    ProcessMovement();
  }
  else if (_inputState == kInputLookState)
  {
    ProcessLook();
  }  
}

void MainState::Update()
{
  if (_inputState == kInputMoveState)
  {
    DrawMovementState();
  }
  else if (_inputState == kInputLookState)
  {
    DrawLookState();
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
      _inputState = kInputMoveState;      
      break;

    default:
      break;
  }
}

void MainState::ProcessMovement()
{
  _keyPressed = getch();  

  bool turnDone = false;

  switch (_keyPressed)
  {
    case NUMPAD_7:
      if (_playerRef->Move(-1, -1))
      {
        Map::Instance().MapOffsetY++;
        Map::Instance().MapOffsetX++;

        turnDone = true;
      }
      break;
      
    case NUMPAD_8:
      if (_playerRef->Move(0, -1))
      {
        Map::Instance().MapOffsetY++;

        turnDone = true;
      }
      break;

    case NUMPAD_9:
      if (_playerRef->Move(1, -1))
      {
        Map::Instance().MapOffsetY++;
        Map::Instance().MapOffsetX--;

        turnDone = true;
      }
      break;

    case NUMPAD_4:
      if (_playerRef->Move(-1, 0))
      {
        Map::Instance().MapOffsetX++;

        turnDone = true;
      }
      break;
    
    case NUMPAD_2:
      if (_playerRef->Move(0, 1))
      {
        Map::Instance().MapOffsetY--;

        turnDone = true;
      }
      break;
    
    case NUMPAD_6:
      if (_playerRef->Move(1, 0))
      {
        Map::Instance().MapOffsetX--;

        turnDone = true;
      }
      break;
    
    case NUMPAD_1:
      if (_playerRef->Move(-1, 1))
      {
        Map::Instance().MapOffsetY--;
        Map::Instance().MapOffsetX++;

        turnDone = true;
      }
      break;

    case NUMPAD_3:
      if (_playerRef->Move(1, 1))
      {
        Map::Instance().MapOffsetY--;
        Map::Instance().MapOffsetX--;

        turnDone = true;
      }
      break;

    // wait
    case NUMPAD_5:
      turnDone = true;
      break;

    case 'l':
      _cursorPosition.X = _playerRef->PosX;
      _cursorPosition.Y = _playerRef->PosY;
      _inputState = kInputLookState;
      break;

    case '@':
      Application::Instance().ChangeState(Application::GameStates::INFO_STATE);
      break;
      
    case 'q':
      Application::Instance().ChangeState(Application::GameStates::EXIT_GAME);
      break;
      
    default:
      break;
  }  

  // If player's turn finished, update all game objects' components
  if (turnDone)
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
        lookStatus += tile->Visible ? "wall" : "?wall?";
      }
      else
      {
        auto gameObjects = Map::Instance().GetGameObjectsAtPosition(_cursorPosition.X, _cursorPosition.Y);

        if (gameObjects.size() != 0)
        {
          lookStatus += "some game object";
        }
        else
        {
          lookStatus += tile->Visible ? "floor" : "?floor?";
        }
      }      
    }
    else
    {
      lookStatus += "???";
    }
    
    Printer::Instance().Print(0, Printer::Instance().TerminalHeight - 1, lookStatus, Printer::kAlignLeft, "#FFFFFF");        

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
