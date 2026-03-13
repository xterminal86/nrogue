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
  _playerRef = &Game::gApp.PlayerInstance;

  _actorStatsInfo.reserve(32);
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
      MoveCursor(_cursorPosition, -1, -1);
      break;

    case ALT_K8:
    case NUMPAD_8:
      MoveCursor(_cursorPosition, 0, -1);
      break;

    case ALT_K9:
    case NUMPAD_9:
      MoveCursor(_cursorPosition, 1, -1);
      break;

    case ALT_K4:
    case NUMPAD_4:
      MoveCursor(_cursorPosition, -1, 0);
      break;

    case ALT_K6:
    case NUMPAD_6:
      MoveCursor(_cursorPosition, 1, 0);
      break;

    case ALT_K1:
    case NUMPAD_1:
      MoveCursor(_cursorPosition, -1, 1);
      break;

    case ALT_K2:
    case NUMPAD_2:
      MoveCursor(_cursorPosition, 0, 1);
      break;

    case ALT_K3:
    case NUMPAD_3:
      MoveCursor(_cursorPosition, 1, 1);
      break;

    case VK_ENTER:
      DisplayActorStats();
      break;

    case VK_CANCEL:
      Game::gApp.ChangeState(GameStates::MAIN_STATE);
      break;

#ifdef DEBUG_BUILD
    case 'h':
    {
      _drawHint = !_drawHint;
    }
    break;

    case 'd':
    {
      GameObject* go =
          Game::gMap.GetStaticGameObjectAtPosition(_cursorPosition.X,
                                                   _cursorPosition.Y);
      if (go != nullptr)
      {
        go->Destroy();
        Game::gPrnt.AddMessage("Removed: " + go->ObjectName);
        Game::gMap.RemoveDestroyed();
        Game::gPrnt.DrawExplosion(_cursorPosition, 3);
      }
      else
      {
        Game::gPrnt.AddMessage(Strings::MsgNothingHere);
      }
    }
    break;

    case 'D':
    {
      auto gos = Game::gMap.GetGameObjectsAtPosition(_cursorPosition.X,
                                                       _cursorPosition.Y);
      if (!gos.empty())
      {
        GameObject* top = gos[gos.size() - 1];
        top->Destroy();
        Game::gPrnt.AddMessage("Removed: " + top->ObjectName);
        Game::gMap.RemoveDestroyed();
        Game::gPrnt.DrawExplosion(_cursorPosition, 3);
      }
      else
      {
        Game::gPrnt.AddMessage(Strings::MsgNothingHere);
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

      _distanceField = (c == nullptr) ?
                       "0x0" :
                       Util::StringFormat("%i %i [%i]",
                                          c->MapPos.X,
                                          c->MapPos.Y,
                                          c->Cost);
    }
    break;

    case 'M':
    {
      GameObject* mm =
          Game::gMI.CreateMonster(_cursorPosition.X,
                                   _cursorPosition.Y,
                                   GameObjectType::MAD_MINER);
      Game::gMap.PlaceActor(mm);
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
    Game::gPrnt.Clear();

    Game::gMap.Draw();

    _playerRef->Draw();

    DrawCursor(_cursorPosition);

    std::string lookStatus;

    int mapSizeX = Game::gMap.CurrentLevel->MapSize.X;
    int mapSizeY = Game::gMap.CurrentLevel->MapSize.Y;

    auto curLvl = Game::gMap.CurrentLevel;

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

    Game::gPrnt.PrintText(
      _twHalf,
      0,
      "Press 'q' to exit look mode",
      Printer::kAlignCenter,
      Colors::White,
      Colors::Black
    );

    std::string coords = Util::StringFormat("[%i;%i]",
                                            _cursorPosition.X,
                                            _cursorPosition.Y);

    Game::gPrnt.PrintText(
      Printer::TerminalWidth - 1,
      Printer::TerminalHeight - 2,
      coords,
      Printer::kAlignRight,
      Colors::White,
      Colors::Black
    );

    Game::gPrnt.PrintText(
      Printer::TerminalWidth - 1,
      Printer::TerminalHeight - 1,
      lookStatus,
      Printer::kAlignRight,
      Colors::White,
      Colors::Black
    );

    #ifdef DEBUG_BUILD
    PrintDebugInfo();

    if (_drawHint)
    {
      DrawHint();
    }
    #endif

    Game::gPrnt.Render();
  }
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
  auto actor = Game::gMap.GetActorAtPosition(_cursorPosition.X,
                                             _cursorPosition.Y);
  return actor;
}

// =============================================================================

const std::vector<GameObject*> LookInputState::CheckGameObjects()
{
  return Game::gMap.GetGameObjectsAtPosition(_cursorPosition.X,
                                             _cursorPosition.Y);
}

// =============================================================================

void LookInputState::DisplayActorStats()
{
  GameObject* actor = CheckActor();
  if (actor == nullptr && CheckPlayer())
  {
    actor = _playerRef;
  }

  if (actor != nullptr)
  {
    Game::gApp.ShowMessageBox(actor);
  }
}

// =============================================================================

#ifdef DEBUG_BUILD
void LookInputState::PrintDebugInfo()
{
  int yStart = 2;

  for (auto& line : _debugInfo)
  {
    Game::gPrnt.PrintText(
      0,
      yStart,
      line,
      Printer::kAlignLeft,
      Colors::White,
      Colors::Black
    );

    yStart++;
  }

  Game::gPrnt.PrintText(
    0,
    yStart + 1,
    _distanceField,
    Printer::kAlignLeft,
    Colors::White,
    Colors::Black
  );
}

void LookInputState::DrawHint()
{
  Position startPoint = _playerRef->GetPosition();

  int mox = Game::gMap.CurrentLevel->MapOffsetX;
  int moy = Game::gMap.CurrentLevel->MapOffsetY;

  _cellsToHighlight.clear();

  const PositionV& line = Util::BresenhamLineFast(startPoint, _cursorPosition);

  Position p;
  for (auto& i : line)
  {
    p.Set(startPoint.X + i.X, startPoint.Y + i.Y);

    if (p == startPoint)
    {
      continue;
    }

    _cellsToHighlight.insert(p);
  }

  for (auto& p : _cellsToHighlight)
  {
    Game::gPrnt.PrintChar(
      p.X + mox,
      p.Y + moy,
      '+',
      Colors::Yellow,
      Colors::Black
    );
  }
}
#endif
