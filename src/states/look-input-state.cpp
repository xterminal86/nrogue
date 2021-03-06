#include "look-input-state.h"

#include "application.h"
#include "printer.h"
#include "map.h"
#include "ai-component.h"
#include "ai-npc.h"
#include "map-level-base.h"

#ifdef DEBUG_BUILD
#include "game-objects-factory.h"
#endif

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
    case ALT_K7:
    case NUMPAD_7:
      MoveCursor(-1, -1);
      break;

    case ALT_K8:
    case NUMPAD_8:
      MoveCursor(0, -1);
      break;

    case ALT_K9:
    case NUMPAD_9:
      MoveCursor(1, -1);
      break;

    case ALT_K4:
    case NUMPAD_4:
      MoveCursor(-1, 0);
      break;

    case ALT_K6:
    case NUMPAD_6:
      MoveCursor(1, 0);
      break;

    case ALT_K1:
    case NUMPAD_1:
      MoveCursor(-1, 1);
      break;

    case ALT_K2:
    case NUMPAD_2:
      MoveCursor(0, 1);
      break;

    case ALT_K3:
    case NUMPAD_3:
      MoveCursor(1, 1);
      break;

    case VK_ENTER:
      // FIXME: remove afterwards, implement as skill
      DisplayMonsterStats();
      break;

    case VK_CANCEL:
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;

    #ifdef DEBUG_BUILD
    case 'b':
    {
      GameObject* bat = GameObjectsFactory::Instance().CreateMonster(_cursorPosition.X, _cursorPosition.Y, GameObjectType::BAT);
      Map::Instance().InsertActor(bat);
    }
    break;

    case 'B':
    {
      GameObject* bat = GameObjectsFactory::Instance().CreateMonster(_cursorPosition.X, _cursorPosition.Y, GameObjectType::VAMPIRE_BAT);
      Map::Instance().InsertActor(bat);
    }
    break;

    case 'r':
    {
      GameObject* rat = GameObjectsFactory::Instance().CreateMonster(_cursorPosition.X, _cursorPosition.Y, GameObjectType::RAT);
      Map::Instance().InsertActor(rat);
    }
    break;

    case 'S':
    {
      GameObject* spider = GameObjectsFactory::Instance().CreateMonster(_cursorPosition.X, _cursorPosition.Y, GameObjectType::SPIDER);
      Map::Instance().InsertActor(spider);
    }
    break;

    case 'T':
    {
      GameObject* troll = GameObjectsFactory::Instance().CreateMonster(_cursorPosition.X, _cursorPosition.Y, GameObjectType::TROLL);
      Map::Instance().InsertActor(troll);
    }
    break;
    #endif

    default:
      break;
  }
}

void LookInputState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    Map::Instance().Draw();

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
              model = dynamic_cast<AIModelBase*>(aic->CurrentModel);
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

      // If nothing is found or area is under fog of war
      if (!foundGameObject)
      {
        if (!tile->Revealed)
        {
          lookStatus = "???";
        }
        else if (tile->Revealed)
        {
          auto& staticObj = Map::Instance().CurrentLevel->StaticMapObjects[_cursorPosition.X][_cursorPosition.Y];

          if (staticObj != nullptr)
          {
            auto nameHidden = (staticObj->FogOfWarName.length() == 0) ?
                              "?" + staticObj->ObjectName + "?" :
                              staticObj->FogOfWarName;

            lookStatus = tile->Visible ? staticObj->ObjectName : nameHidden;
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
    }
    else
    {
      // If cursor is outside map boundaries
      lookStatus = "???";
    }

    Printer::Instance().PrintFB(Printer::TerminalWidth / 2, 0,
                                  "Press 'q' to exit look mode",
                                  Printer::kAlignCenter,
                                  "#FFFFFF");

    Printer::Instance().PrintFB(Printer::TerminalWidth - 1,
                                  Printer::TerminalHeight - 1,
                                  lookStatus,
                                  Printer::kAlignRight,
                                  "#FFFFFF");

    #ifdef DEBUG_BUILD
    PrintDebugInfo();
    #endif

    Printer::Instance().Render();
  }
}

void LookInputState::MoveCursor(int dx, int dy)
{
  int nx = _cursorPosition.X + dx;
  int ny = _cursorPosition.Y + dy;

  int hw = Printer::TerminalWidth / 2;
  int hh = Printer::TerminalHeight / 2;

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
    std::string name = Util::StringFormat("%s_%lu", actor->ObjectName.data(), actor->ObjectId());

    msg.push_back(Util::StringFormat("Rating: %i", actor->Attrs.Rating()));
    msg.push_back("");
    msg.push_back(Util::StringFormat("LVL: %i", actor->Attrs.Lvl.Get()));
    msg.push_back(Util::StringFormat("EXP: %i", actor->Attrs.Exp.Min().Get()));
    msg.push_back("");
    msg.push_back(Util::StringFormat("STR: %i", actor->Attrs.Str.Get()));
    msg.push_back(Util::StringFormat("DEF: %i", actor->Attrs.Def.Get()));
    msg.push_back(Util::StringFormat("MAG: %i", actor->Attrs.Mag.Get()));
    msg.push_back(Util::StringFormat("RES: %i", actor->Attrs.Res.Get()));
    msg.push_back(Util::StringFormat("SKL: %i", actor->Attrs.Skl.Get()));
    msg.push_back(Util::StringFormat("SPD: %i", actor->Attrs.Spd.Get()));
    msg.push_back("");
    msg.push_back(Util::StringFormat("HP: %i/%i", actor->Attrs.HP.Min().Get(), actor->Attrs.HP.Max().Get()));
    msg.push_back(Util::StringFormat("MP: %i/%i", actor->Attrs.MP.Min().Get(), actor->Attrs.MP.Max().Get()));
    msg.push_back("");
    msg.push_back(Util::StringFormat("Action Meter: %i", actor->Attrs.ActionMeter));

    Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, name, msg);
  }
}

#ifdef DEBUG_BUILD
void LookInputState::PrintDebugInfo()
{
  int yStart = 2;

  for (auto& line : _debugInfo)
  {
    Printer::Instance().PrintFB(0, yStart, line, Printer::kAlignLeft, "#FFFFFF");
    yStart++;
  }
}
#endif
