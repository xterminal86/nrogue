#include "look-input-state.h"

#include "constants.h"
#include "application.h"
#include "printer.h"
#include "map.h"
#include "item-component.h"
#include "ai-component.h"
#include "ai-npc.h"
#include "ai-monster-basic.h"
#include "map-level-base.h"

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
  _keyPressed = GetKeyDown();

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
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
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

    int mapSizeX = Map::Instance().CurrentLevel->MapSize.X;
    int mapSizeY = Map::Instance().CurrentLevel->MapSize.Y;

    if (Util::CheckLimits(_cursorPosition, Position(mapSizeX, mapSizeY)))
    {
      auto tile = Map::Instance().CurrentLevel->MapArray[_cursorPosition.X][_cursorPosition.Y].get();

      bool foundGameObject = false;

      if (CheckPlayer())
      {
        lookStatus = "It's you!";
        foundGameObject = true;
      }
      else if (tile->Visible)
      {        
        auto actor = CheckActor();
        if (actor != nullptr)
        {          
          AIComponent* aic = actor->GetComponent<AIComponent>();
          if (aic != nullptr)
          {
            AIModelBase* model = dynamic_cast<AINPC*>(aic->CurrentModel);
            if (model != nullptr)
            {
              AINPC* ainpc = static_cast<AINPC*>(model);
              std::string name = ainpc->Data.Name;
              std::string title = ainpc->Data.Job;
              std::string unidStr = ainpc->Data.UnacquaintedDescription;
              auto idStr = Util::StringFormat("You see %s the %s", name.data(), title.data());
              lookStatus = (ainpc->Data.IsAquainted) ? idStr : unidStr;
              foundGameObject = true;
            }
            else
            {
              model = dynamic_cast<AIMonsterBasic*>(aic->CurrentModel);
              if (model != nullptr)
              {
                lookStatus = aic->OwnerGameObject->ObjectName;
                foundGameObject = true;
              }
            }
          }
        }
        else
        {
          auto gos = CheckGameObjects();
          if (gos.size() != 0)
          {
            std::string objName = gos.back()->ObjectName;

            ItemComponent* ic = gos.back()->GetComponent<ItemComponent>();
            if (ic != nullptr)
            {
              objName = ic->Data.IsIdentified ? ic->Data.IdentifiedName : ic->Data.UnidentifiedName;
            }

            lookStatus = objName;
            foundGameObject = true;
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

    Printer::Instance().PrintFB(Printer::Instance().TerminalWidth / 2, 0,
                                  "Press 'q' to exit look mode",
                                  Printer::kAlignCenter,
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
  Printer::Instance().PrintFB(_cursorPosition.X + Map::Instance().CurrentLevel->MapOffsetX + 1,
                              _cursorPosition.Y + Map::Instance().CurrentLevel->MapOffsetY,
                              ']', "#FFFFFF");

  Printer::Instance().PrintFB(_cursorPosition.X + Map::Instance().CurrentLevel->MapOffsetX - 1,
                              _cursorPosition.Y + Map::Instance().CurrentLevel->MapOffsetY,
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
  if (actor == nullptr && CheckPlayer())
  {
    actor = _playerRef;
  }

  if (actor != nullptr)
  {
    std::vector<std::string> msg;
    msg.push_back(Util::StringFormat("Rating: %i", actor->Attrs.Rating()));
    msg.push_back("");
    msg.push_back(Util::StringFormat("LVL: %i", actor->Attrs.Lvl.CurrentValue));
    msg.push_back(Util::StringFormat("EXP: %i", actor->Attrs.Exp.CurrentValue));    
    msg.push_back("");
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

    Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, actor->ObjectName, msg);
  }
}
