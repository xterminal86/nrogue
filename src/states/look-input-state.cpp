#include "look-input-state.h"

#include "application.h"
#include "printer.h"
#include "map.h"
#include "ai-component.h"
#include "ai-npc.h"
#include "map-level-base.h"

#ifdef DEBUG_BUILD
#include "game-objects-factory.h"
#include "monsters-inc.h"
#endif

void LookInputState::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;

  _monsterStatsInfo.reserve(32);
}

// =============================================================================

void LookInputState::Prepare()
{
  _cursorPosition.X = _playerRef->PosX;
  _cursorPosition.Y = _playerRef->PosY;
}

// =============================================================================

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
      // FIXME: remove afterwards, implement as skill or whatever
      DisplayMonsterStats();
      break;

    case VK_CANCEL:
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;

#ifdef DEBUG_BUILD
    case 'd':
    {
      GameObject* go =
          Map::Instance().GetStaticGameObjectAtPosition(_cursorPosition.X,
                                                        _cursorPosition.Y);
      if (go != nullptr)
      {
        go->IsDestroyed = true;
        Printer::Instance().AddMessage("Removed: " + go->ObjectName);
        Map::Instance().RemoveDestroyed();
        Printer::Instance().DrawExplosion(_cursorPosition, 3);
      }
      else
      {
        Printer::Instance().AddMessage(Strings::MsgNothingHere);
      }
    }
    break;

    case 'D':
    {
      auto gos = Map::Instance().GetGameObjectsAtPosition(_cursorPosition.X,
                                                          _cursorPosition.Y);
      if (!gos.empty())
      {
        GameObject* top = gos[gos.size() - 1];
        top->IsDestroyed = true;
        Printer::Instance().AddMessage("Removed: " + top->ObjectName);
        Map::Instance().RemoveDestroyed();
        Printer::Instance().DrawExplosion(_cursorPosition, 3);
      }
      else
      {
        Printer::Instance().AddMessage(Strings::MsgNothingHere);
      }
    }
    break;

    case 'f':
    {
      if (_playerRef->DistanceField.IsDirty())
      {
        _playerRef->DistanceField.Emanate();
      }

      PotentialField::Cell* c =
          _playerRef->DistanceField.GetCell(_cursorPosition.X,
                                            _cursorPosition.Y);

      _distanceField = (c == nullptr)
                       ? "0x0"
                       : Util::StringFormat("%i %i [%i]",
                                            c->MapPos.X,
                                            c->MapPos.Y,
                                            c->Cost);
    }
    break;

    case 'M':
    {
      GameObject* mm =
          MonstersInc::Instance().CreateMonster(_cursorPosition.X,
                                                _cursorPosition.Y,
                                                GameObjectType::MAD_MINER);
      Map::Instance().PlaceActor(mm);
    }
    break;
#endif

    default:
      break;
  }
}

// =============================================================================

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

    auto curLvl = Map::Instance().CurrentLevel;

    if (Util::CheckLimits(_cursorPosition, { mapSizeX, mapSizeY }))
    {
      auto tile = curLvl->MapArray[_cursorPosition.X][_cursorPosition.Y].get();

      bool foundGameObject = false;

      if (CheckPlayer())
      {
        lookStatus = "It's you!";
        foundGameObject = true;
      }
      else
      {
        //
        // If tile is visible, check if game objects are present on it:
        // actors or items.
        //
        if (tile->Visible)
        {
          auto actor = CheckActor();
          if (actor != nullptr)
          {
            AIComponent* aic = actor->GetComponent<AIComponent>();
            if (aic != nullptr)
            {
              if (aic->CurrentModel != nullptr)
              {
                AINPC* model = dynamic_cast<AINPC*>(aic->CurrentModel);
                if (model != nullptr)
                {
                  std::string name = model->Data.Name;
                  std::string title = model->Data.Job;
                  std::string unidStr = model->Data.UnacquaintedDescription;
                  auto idStr = Util::StringFormat("You see %s the %s",
                                                  name.data(),
                                                  title.data());
                  lookStatus = (model->Data.IsAquainted) ? idStr : unidStr;
                  foundGameObject = true;
                }
                else
                {
                  std::string objName = aic->OwnerGameObject->ObjectName;

                  bool hasInvisibility =
                      aic->OwnerGameObject->HasEffect(
                        ItemBonusType::INVISIBILITY
                  );

                  if (hasInvisibility)
                  {
                    bool hasTele =
                        _playerRef->HasEffect(ItemBonusType::TELEPATHY);

                    bool hasTS =
                        _playerRef->HasEffect(ItemBonusType::TRUE_SEEING);

                    bool objIsLiving = aic->OwnerGameObject->IsLiving;

                    bool detectLiving = ((hasTele || hasTS) && objIsLiving);
                    bool detectHidden = (hasTS && !objIsLiving);

                    objName = (detectHidden || detectLiving)
                            ? aic->OwnerGameObject->ObjectName
                            : "?";
                  }

                  lookStatus = objName;
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
                objName = ic->Data.IsIdentified
                          ? ic->Data.IdentifiedName
                          : ic->Data.UnidentifiedName;
              }

              lookStatus = objName;
              foundGameObject = true;
            }
          }

          //
          // No objects found on this tile,
          // get static object or map array object name as name to display.
          //
          if (!foundGameObject)
          {
            auto& staticObj =
                curLvl->StaticMapObjects[_cursorPosition.X][_cursorPosition.Y];

            lookStatus = (staticObj != nullptr)
                         ? staticObj->ObjectName
                         : tile->ObjectName;
          }
        }
        else
        {
          //
          // Tile is not visible,
          // so get its last known name if it was revealed earlier.
          //
          lookStatus =
              tile->Revealed
              ? curLvl->FowLayer[_cursorPosition.X][_cursorPosition.Y].FowName
              : Strings::TripleQuestionMarks;
        }
      }
    }
    else
    {
      //
      // If cursor is outside map boundaries.
      //
      lookStatus = Strings::TripleQuestionMarks;
    }

    Printer::Instance().PrintFB(_twHalf, 0,
                                "Press 'q' to exit look mode",
                                Printer::kAlignCenter,
                                Colors::WhiteColor,
                                Colors::BlackColor);

    std::string coords = Util::StringFormat("[%i;%i]",
                                            _cursorPosition.X,
                                            _cursorPosition.Y);

    Printer::Instance().PrintFB(Printer::TerminalWidth - 1,
                                Printer::TerminalHeight - 2,
                                coords,
                                Printer::kAlignRight,
                                Colors::WhiteColor,
                                Colors::BlackColor);

    Printer::Instance().PrintFB(Printer::TerminalWidth - 1,
                                Printer::TerminalHeight - 1,
                                lookStatus,
                                Printer::kAlignRight,
                                Colors::WhiteColor,
                                Colors::BlackColor);

    #ifdef DEBUG_BUILD
    PrintDebugInfo();
    #endif

    Printer::Instance().Render();
  }
}

// =============================================================================

void LookInputState::MoveCursor(int dx, int dy)
{
  int nx = _cursorPosition.X + dx;
  int ny = _cursorPosition.Y + dy;

  int hw = _twHalf;
  int hh = _thHalf;

  //
  // To compensate for cursor image.
  //
  nx = Util::Clamp(nx, _playerRef->PosX - hw + 1,
                       _playerRef->PosX + hw - 2);

  ny = Util::Clamp(ny, _playerRef->PosY - hh,
                       _playerRef->PosY + hh);

  _cursorPosition.X = nx;
  _cursorPosition.Y = ny;
}

// =============================================================================

void LookInputState::DrawCursor()
{
  Printer::Instance().PrintFB(_cursorPosition.X +
                              Map::Instance().CurrentLevel->MapOffsetX + 1,
                              _cursorPosition.Y +
                              Map::Instance().CurrentLevel->MapOffsetY,
                              ']',
                              Colors::WhiteColor,
                              Colors::BlackColor);

  Printer::Instance().PrintFB(_cursorPosition.X +
                              Map::Instance().CurrentLevel->MapOffsetX - 1,
                              _cursorPosition.Y +
                              Map::Instance().CurrentLevel->MapOffsetY,
                              '[',
                              Colors::WhiteColor,
                              Colors::BlackColor);
}

// =============================================================================

bool LookInputState::CheckPlayer()
{
  return (_cursorPosition.X == _playerRef->PosX
       && _cursorPosition.Y == _playerRef->PosY);
}

// =============================================================================

GameObject* LookInputState::CheckActor()
{
  auto actor = Map::Instance().GetActorAtPosition(_cursorPosition.X,
                                                  _cursorPosition.Y);
  return actor;
}

// =============================================================================

const std::vector<GameObject*> LookInputState::CheckGameObjects()
{
  return Map::Instance().GetGameObjectsAtPosition(_cursorPosition.X,
                                                  _cursorPosition.Y);
}

// =============================================================================

void LookInputState::DisplayMonsterStats()
{
  auto GetPrettyPrint =
  [this](Attribute& attr, const std::string& attrName)
  {
    int val = attr.Get();
    int mod = attr.GetModifiers();

    std::string txt = (mod <= 0)
                      ? Util::StringFormat("(%i)", mod)
                      : Util::StringFormat("(+%i)", mod);

    std::string total = Util::StringFormat("%s: %i %s",
                                           attrName.data(),
                                           val,
                                           txt.data());

    return total;
  };

  auto actor = CheckActor();
  if (actor == nullptr && CheckPlayer())
  {
    actor = _playerRef;
  }

  if (actor != nullptr)
  {
    _monsterStatsInfo.clear();

    std::string name = Util::StringFormat("%s_%llu",
                                          actor->ObjectName.data(),
                                          actor->ObjectId());

    _monsterStatsInfo.push_back(
          Util::StringFormat("LVL: %i", actor->Attrs.Lvl.Get())
    );

    _monsterStatsInfo.push_back(
          Util::StringFormat("EXP: %i", actor->Attrs.Exp.Min().Get())
    );

    _monsterStatsInfo.push_back(std::string());
    _monsterStatsInfo.push_back(GetPrettyPrint(actor->Attrs.Str, "STR"));
    _monsterStatsInfo.push_back(GetPrettyPrint(actor->Attrs.Def, "DEF"));
    _monsterStatsInfo.push_back(GetPrettyPrint(actor->Attrs.Mag, "MAG"));
    _monsterStatsInfo.push_back(GetPrettyPrint(actor->Attrs.Res, "RES"));
    _monsterStatsInfo.push_back(GetPrettyPrint(actor->Attrs.Skl, "SKL"));
    _monsterStatsInfo.push_back(GetPrettyPrint(actor->Attrs.Spd, "SPD"));
    _monsterStatsInfo.push_back(std::string());

    _monsterStatsInfo.push_back(
          Util::StringFormat("Rating: %i", actor->Attrs.Rating())
    );

    _monsterStatsInfo.push_back(
          Util::StringFormat("(CR: %i)", actor->Attrs.ChallengeRating)
    );

    _monsterStatsInfo.push_back(std::string());

    _monsterStatsInfo.push_back(
          Util::StringFormat("HP: %i/%i",
                             actor->Attrs.HP.Min().Get(),
                             actor->Attrs.HP.Max().Get())
    );

    _monsterStatsInfo.push_back(
          Util::StringFormat("MP: %i/%i",
                             actor->Attrs.MP.Min().Get(),
                             actor->Attrs.MP.Max().Get())
    );

    _monsterStatsInfo.push_back(std::string());
    _monsterStatsInfo.push_back(Util::StringFormat("Action Meter: %i",
                                                   actor->Attrs.ActionMeter));

    Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                           name,
                                           _monsterStatsInfo);
  }
}

// =============================================================================

#ifdef DEBUG_BUILD
void LookInputState::PrintDebugInfo()
{
  int yStart = 2;

  for (auto& line : _debugInfo)
  {
    Printer::Instance().PrintFB(0,
                                yStart,
                                line,
                                Printer::kAlignLeft,
                                Colors::WhiteColor,
                                Colors::BlackColor);
    yStart++;
  }

  Printer::Instance().PrintFB(0,
                              yStart + 1,
                              _distanceField,
                              Printer::kAlignLeft,
                              Colors::WhiteColor,
                              Colors::BlackColor);
}
#endif
