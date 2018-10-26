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

    case VK_ENTER:
      // FIXME: remove afterwards, implement as skill
      DisplayMonsterStats();
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

      bool foundGameObject = false;

      // Check actors and game objects in visible area

      if (tile->Visible)
      {
        if (CheckPlayer())
        {
          lookStatus = "It's you!";
          foundGameObject = true;
        }
        else
        {
          auto actor = CheckActor();
          if (actor != nullptr)
          {
            lookStatus = actor->ObjectName;
            foundGameObject = true;
          }
          else
          {
            auto gos = CheckGameObjects();
            if (gos.size() != 0)
            {
              lookStatus = gos.back()->ObjectName;
              foundGameObject = true;
            }
          }
        }
      }

      // If nothing is found or area is under fog of war,
      // check map tile instead
      if (!foundGameObject)
      {
        if (!tile->Revealed)
        {
          lookStatus = "???";
        }
        else if (tile->Revealed)
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
      // If cursor is outside map boundaries
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

bool LookInputState::CheckPlayer()
{
  return (_cursorPosition.X == _playerRef->PosX
       && _cursorPosition.Y == _playerRef->PosY);
}

GameObject* LookInputState::CheckActor()
{
  auto actor = Map::Instance().GetActorAtPosition(_cursorPosition.X, _cursorPosition.Y);
  return actor;
}

const std::vector<GameObject*> LookInputState::CheckGameObjects()
{
  return Map::Instance().GetGameObjectsAtPosition(_cursorPosition.X, _cursorPosition.Y);
}

void LookInputState::DisplayMonsterStats()
{
  auto actor = CheckActor();
  if (actor != nullptr)
  {
    std::vector<std::string> msg;
    msg.push_back(Util::StringFormat("STR: %i", actor->Attrs.Str.CurrentValue));
    msg.push_back(Util::StringFormat("DEF: %i", actor->Attrs.Def.CurrentValue));
    msg.push_back(Util::StringFormat("MAG: %i", actor->Attrs.Mag.CurrentValue));
    msg.push_back(Util::StringFormat("RES: %i", actor->Attrs.Res.CurrentValue));
    msg.push_back(Util::StringFormat("SKL: %i", actor->Attrs.Skl.CurrentValue));
    msg.push_back(Util::StringFormat("SPD: %i", actor->Attrs.Spd.CurrentValue));
    msg.push_back("");
    msg.push_back(Util::StringFormat("HP: %i/%i", actor->Attrs.HP.CurrentValue, actor->Attrs.HP.OriginalValue));
    msg.push_back(Util::StringFormat("MP: %i/%i", actor->Attrs.MP.CurrentValue, actor->Attrs.MP.OriginalValue));
    msg.push_back("");
    msg.push_back(Util::StringFormat("Action Meter: %i", actor->Attrs.ActionMeter));

    Application::Instance().ShowMessageBox(actor->ObjectName, msg);
  }
}
