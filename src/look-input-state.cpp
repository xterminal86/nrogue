#include "look-input-state.h"

#include "constants.h"
#include "application.h"
#include "printer.h"
#include "map.h"

void LookInputState::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

void LookInputState::Prepare()
{
  _cursorPosition.X = _playerRef->PosX;
  _cursorPosition.Y = _playerRef->PosY;
}

void LookInputState::HandleInput()
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
      Application::Instance().ChangeState(Application::GameStates::MAIN_STATE);
      break;

    default:
      break;
  }
}

void LookInputState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    Map::Instance().Draw(_playerRef->PosX, _playerRef->PosY);

    _playerRef->Draw();

    DrawCursor();

    std::string lookStatus;

    if (Util::CheckLimits(_cursorPosition, Position(GlobalConstants::MapX, GlobalConstants::MapY)))
    {
      auto* tile = &Map::Instance().MapArray[_cursorPosition.X][_cursorPosition.Y];
      if (_cursorPosition.X == _playerRef->PosX && _cursorPosition.Y == _playerRef->PosY)
      {
        lookStatus = "It's you!";
      }
      else if (!tile->Revealed)
      {
        lookStatus = "???";
      }
      else if (tile->Blocking)
      {        
        auto nameHidden = (tile->FogOfWarName.length() == 0) ?
                          "?" + tile->ObjectName + "?" :
                          tile->FogOfWarName;
        lookStatus = tile->Visible ? tile->ObjectName : nameHidden;
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
          auto nameHidden = (tile->FogOfWarName.length() == 0) ?
                            "?" + tile->ObjectName + "?" :
                            tile->FogOfWarName;
          lookStatus = tile->Visible ? tile->ObjectName : nameHidden;
        }
      }
    }
    else
    {
      lookStatus = "???";
    }

    Printer::Instance().PrintFB(0, Printer::Instance().TerminalHeight - 1,
                                  "Press 'q' to exit look mode",
                                  Printer::kAlignLeft,
                                  "#FFFFFF");

    Printer::Instance().PrintFB(Printer::Instance().TerminalWidth - 1,
                                  Printer::Instance().TerminalHeight - 1,
                                  lookStatus,
                                  Printer::kAlignRight,
                                  "#FFFFFF");

    Printer::Instance().Render();
  }
}

void LookInputState::MoveCursor(int dx, int dy)
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

void LookInputState::DrawCursor()
{
  Printer::Instance().PrintFB(_cursorPosition.X + Map::Instance().MapOffsetX + 1,
                              _cursorPosition.Y + Map::Instance().MapOffsetY,
                              ']', "#FFFFFF");

  Printer::Instance().PrintFB(_cursorPosition.X + Map::Instance().MapOffsetX - 1,
                              _cursorPosition.Y + Map::Instance().MapOffsetY,
                              '[', "#FFFFFF");
}
