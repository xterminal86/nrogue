#include "main-state.h"

#include "application.h"
#include "map.h"
#include "printer.h"
#include "stairs-component.h"
#include "target-state.h"
#include "spells-processor.h"
#include "pickup-item-state.h"
#include "timer.h"

#include "equipment-component.h"

void MainState::Init()
{
  _playerRef = &Game::gApp.PlayerInstance;
}

// =============================================================================

void MainState::HandleInput()
{
  //
  // Otherwise we could still time-in some action
  // even after we received fatal damage.
  //
  // E.g. we wait a turn, spider approaches and deals fatal damage,
  // this results in Player.IsAlive = false, but if we check IsAlive()
  // at the end of HandleInput(), we could still issue some other command
  // if we are quick enough before condition is checked
  // and current state changes to ENDGAME_STATE.
  //
  if (!_playerRef->HasNonZeroHP())
  {
    Game::gApp.ChangeState(GameStates::GAMEOVER_STATE);
    return;
  }

  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    // -------------------------------------------------------------------------
    case ALT_K7:
    case NUMPAD_7:
      ProcessMovement({ -1, -1 });
      break;
    // -------------------------------------------------------------------------
    case ALT_K8:
    case NUMPAD_8:
      ProcessMovement({ 0, -1 });
      break;
    // -------------------------------------------------------------------------
    case ALT_K9:
    case NUMPAD_9:
      ProcessMovement({ 1, -1 });
      break;
    // -------------------------------------------------------------------------
    case ALT_K4:
    case NUMPAD_4:
      ProcessMovement({ -1, 0 });
      break;
    // -------------------------------------------------------------------------
    case ALT_K2:
    case NUMPAD_2:
      ProcessMovement({ 0, 1 });
      break;
    // -------------------------------------------------------------------------
    case ALT_K6:
    case NUMPAD_6:
      ProcessMovement({ 1, 0 });
      break;
    // -------------------------------------------------------------------------
    case ALT_K1:
    case NUMPAD_1:
      ProcessMovement({ -1, 1 });
      break;
    // -------------------------------------------------------------------------
    case ALT_K3:
    case NUMPAD_3:
      ProcessMovement({ 1, 1 });
      break;
    // -------------------------------------------------------------------------
    case ALT_K5:
    case NUMPAD_5:
      Game::gPrnt.AddMessage(Strings::MsgWait);
      _playerRef->FinishTurn();
      break;
    // -------------------------------------------------------------------------
    case 'a':
    {
      if (Game::gMap.CurrentLevel->Peaceful)
      {
        PrintNoAttackInTown();
      }
      else if (_playerRef->IsSwimming())
      {
        Game::gPrnt.AddMessage(Strings::MsgNotInWater);
      }
      else
      {
        Game::gApp.ChangeState(GameStates::ATTACK_STATE);
      }
    }
    break;
    // -------------------------------------------------------------------------
    case '$':
    {
      auto str = Util::StringFormat("You have %i %s",
                                    _playerRef->Money,
                                    Strings::MoneyName.data());
      Game::gPrnt.AddMessage(str);
    }
    break;
    // -------------------------------------------------------------------------
    case 'e':
      Game::gApp.ChangeState(GameStates::INVENTORY_STATE);
      break;
    // -------------------------------------------------------------------------
    case 'm':
      Game::gApp.ChangeState(GameStates::SHOW_MESSAGES_STATE);
      break;
    // -------------------------------------------------------------------------
    case 'l':
      Game::gApp.ChangeState(GameStates::LOOK_INPUT_STATE);
      break;
    // -------------------------------------------------------------------------
    case 'i':
      Game::gApp.ChangeState(GameStates::INTERACT_INPUT_STATE);
      break;
    // -------------------------------------------------------------------------
    case 'I':
      DisplayScenarioInformation();
      break;
    // -------------------------------------------------------------------------
    case 'g':
      TryToPickupItems();
      break;
    // -------------------------------------------------------------------------
    case '@':
      Game::gApp.ChangeState(GameStates::INFO_STATE);
      break;
    // -------------------------------------------------------------------------
    case 'H':
    case 'h':
    case '?':
      Game::gApp.ChangeState(GameStates::HELP_STATE);
      break;
    // -------------------------------------------------------------------------
    case 'Q':
      Game::gApp.ChangeState(GameStates::EXITING_STATE);
      break;
    // -------------------------------------------------------------------------
    case 'f':
      ProcessRangedWeapon();
      break;
    // -------------------------------------------------------------------------
    case '>':
    {
      auto res = CheckStairs('>');
      if (res.first != nullptr)
      {
        ClimbStairs(res);
      }
    }
    break;
    // -------------------------------------------------------------------------
    case '<':
    {
      auto res = CheckStairs('<');
      if (res.first != nullptr)
      {
        ClimbStairs(res);
      }
    }
    break;
    // -------------------------------------------------------------------------
    case 'S':
      Game::gApp.ChangeState(GameStates::SAVE_GAME_STATE);
      break;
    // -------------------------------------------------------------------------
#ifdef DEBUG_BUILD
    case '`':
      Game::gApp.ChangeState(GameStates::DEV_CONSOLE);
      break;

    case 'T':
    {
      int exitX = Game::gMap.CurrentLevel->LevelExit.X;
      int exitY = Game::gMap.CurrentLevel->LevelExit.Y;

      if (exitX < 0 || exitY < 0)
      {
        Game::gPrnt.AddMessage("No exit defined on this level!");
      }
      else
      {
        if (_playerRef->MoveTo(exitX, exitY))
        {
          Game::gMap.CurrentLevel->AdjustCamera();
          Update(true);
          _playerRef->FinishTurn();
        }
        else
        {
          auto str = Util::StringFormat("[%i;%i] is occupied!", exitX, exitY);
          Game::gPrnt.AddMessage(str);
          DebugLog("%s\n", str.data());
        }
      }
    }
    break;

    case 's':
      GetActorsAround();
      break;
#endif
    // -------------------------------------------------------------------------
    default:
      break;
  }
}

// =============================================================================

void MainState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Game::gPrnt.Clear();

    _playerRef->CheckVisibility();

    Game::gMap.Draw();

    _playerRef->Draw();

    DisplayStartHint();
    DisplayExitHint();
    DisplayStatusIcons();
    DrawHPMP();

    if (Game::gPrnt.ShowLastMessage)
    {
      DisplayGameLog();
    }
    else
    {
      Game::gPrnt.ResetMessagesToDisplay();
    }

    Game::gPrnt.PrintFB(Printer::TerminalWidth - 1,
                        0,
                        Game::gMap.CurrentLevel->LevelName,
                        Printer::kAlignRight,
                        Colors::WhiteColor,
                        Colors::BlackColor);

    #ifdef DEBUG_BUILD
    Game::gTimer.StartProfiling("  PrintDebugInfo()");
    PrintDebugInfo();
    Game::gTimer.FinishProfiling("  PrintDebugInfo()");
    #endif

    Game::gPrnt.Render();
  }
}

// =============================================================================

void MainState::ProcessMovement(const Position& dirOffsets)
{
  if (_playerRef->TryToMeleeAttack(dirOffsets.X, dirOffsets.Y))
  {
    _playerRef->FinishTurn();
  }
  else if (_playerRef->Move(dirOffsets.X, dirOffsets.Y))
  {
    //
    // This line must be the first in order to
    // allow potential messages to show in FinishTurn()
    // (e.g. starvation damage message) after player moved.
    //
    Game::gPrnt.ShowLastMessage = false;

    CheckItemsOnGround();

    Game::gMap.CurrentLevel->MapOffsetX -= dirOffsets.X;
    Game::gMap.CurrentLevel->MapOffsetY -= dirOffsets.Y;

    _playerRef->FinishTurn();

    //
    // Sometimes loot can drop on top of stairs which can obscure them.
    // Also, it is possible for stairs to become camouflaged
    // or somehow else become inactive (via level map design, for example).
    //
    // Stairs are part of floor map tiles, so they are not updated.
    // That's why we need to check for stairs in main state.
    //

    auto& px = _playerRef->PosX;
    auto& py = _playerRef->PosY;

    GameObject* tile = Game::gMap.CurrentLevel->MapArray[px][py].get();

    bool stairsHere = (tile->Image == '>' || tile->Image == '<');
    if (stairsHere)
    {
      StairsComponent* sc = tile->GetComponent<StairsComponent>();
      if (sc->IsEnabled)
      {
        Game::gPrnt.AddMessage((tile->Image == '>') ?
                                 Strings::MsgStairsDown :
                                 Strings::MsgStairsUp);
      }
    }
  }
  else
  {
    bool actorInFront = (Game::gMap.GetActorAtPosition(
                           _playerRef->PosX + dirOffsets.X,
                           _playerRef->PosY + dirOffsets.Y
                        ) != nullptr);

    if (_playerRef->IsSwimming() && actorInFront)
    {
      Game::gPrnt.AddMessage("You can't attack while swimming!");
    }
  }
}

// =============================================================================

void MainState::CheckItemsOnGround()
{
  auto items = Game::gMap.GetGameObjectsToPickup(_playerRef->PosX,
                                                   _playerRef->PosY);
  if (items.size() > 1)
  {
    Game::gPrnt.AddMessage(Strings::MsgItemsLyingHere);
  }
}

// =============================================================================

void MainState::DisplayGameLog()
{
  int x = Printer::TerminalWidth - 1;
  int y = Printer::TerminalHeight - 1;

  // FIXME: always prints > 1 messages if available.
  int count = 0;
  auto msgs = Game::gPrnt.GetLastMessages();
  for (GameLogMessageData* m : msgs)
  {
    if (m == nullptr)
    {
      break;
    }

    Game::gPrnt.PrintFB(x,
                        y - count,
                        m->Message,
                        Printer::kAlignRight,
                        m->FgColor,
                        m->BgColor);
    count++;
  }
}

// =============================================================================

void MainState::TryToPickupItems()
{
  auto items = Game::gMap.GetGameObjectsToPickup(_playerRef->PosX,
                                                   _playerRef->PosY);
  if (!items.empty())
  {
    if (items.size() == 1)
    {
      PickupSingleItem(items[0]);
    }
    else
    {
      GameStates s = GameStates::PICKUP_ITEM_STATE;
      auto gs = Game::gApp.GetGameStateRefByName(s);
      PickupItemState* pis = static_cast<PickupItemState*>(gs);
      pis->Setup(items);
      Game::gApp.ChangeState(GameStates::PICKUP_ITEM_STATE);
    }
  }
  else
  {
    Game::gPrnt.AddMessage(Strings::MsgNothingHere);
  }
}

// =============================================================================

void MainState::PickupSingleItem(std::pair<int, GameObject *>& item)
{
  if (ProcessMoneyPickup(item))
  {
    return;
  }

  if (_playerRef->Inventory->IsFull())
  {
    Game::gApp.ShowMessageBox(
      MessageBoxType::ANY_KEY,
      Strings::MessageBoxEpicFailHeaderText,
      { Strings::MsgInventoryFull },
      Colors::MessageBoxRedBorderColor
    );

    return;
  }

  ProcessItemPickup(item);
}

// =============================================================================

void MainState::DrawHPMP()
{
  int curHp = _playerRef->Attrs.HP.Min().Get();
  int maxHp = _playerRef->Attrs.HP.Max().Get();
  int curMp = _playerRef->Attrs.MP.Min().Get();
  int maxMp = _playerRef->Attrs.MP.Max().Get();

  int th = Printer::TerminalHeight;

  UpdateBar(1, th - 2, _playerRef->Attrs.HP);

  auto str = Util::StringFormat("%i/%i", curHp, maxHp);
  Game::gPrnt.PrintFB(GlobalConstants::HPMPBarLength / 2,
                      th - 2,
                      str,
                      Printer::kAlignCenter,
                      Colors::WhiteColor,
                      0x880000);

  UpdateBar(1, th - 1, _playerRef->Attrs.MP);

  str = Util::StringFormat("%i/%i", curMp, maxMp);
  Game::gPrnt.PrintFB(GlobalConstants::HPMPBarLength / 2,
                      th - 1,
                      str,
                      Printer::kAlignCenter,
                      Colors::WhiteColor,
                      0x000088);

#ifdef USE_SDL
  int expCur = _playerRef->Attrs.Exp.Min().Get();
  int expMax = _playerRef->Attrs.Exp.Max().Get();

  auto tws = Game::gPrnt.GetTileWHScaled();

  int xPos1 = tws.first * 2;
  int xPos2 = GlobalConstants::HPMPBarLength * tws.first;
  xPos2 = xPos1 + (int)(((double)expCur / (double)expMax) * (double)xPos2);
  int yPos = (th - 1) * tws.second;

  Game::gPrnt.DrawRect(xPos1, yPos - 1, xPos2, yPos + 1, 0xFFFF00);
#endif
}

// =============================================================================

void MainState::UpdateBar(int x, int y, RangedAttribute& attr)
{
  double ratio = ((double)attr.Min().Get() / (double)attr.Max().Get());
  int len = ratio * GlobalConstants::HPMPBarLength;

  std::string bar = "[";
  for (int i = 0; i < GlobalConstants::HPMPBarLength; i++)
  {
    bar += (i < len) ? "=" : " ";
  }

  bar += "]";

  Game::gPrnt.PrintFB(x,
                      y,
                      bar,
                      Printer::kAlignLeft,
                      Colors::WhiteColor,
                      Colors::BlackColor);
}

// =============================================================================

std::pair<GameObject*, bool> MainState::CheckStairs(int stairsSymbol)
{
  auto& ma = Game::gMap.CurrentLevel->MapArray;
  GameObject* stairsTile = ma[_playerRef->PosX][_playerRef->PosY].get();

  //
  // We're relying here on stairsSymbol to be exactly '>' or '<'
  //
  if (stairsTile->Image != stairsSymbol)
  {
    Game::gPrnt.AddMessage((stairsSymbol == '>') ?
                             Strings::MsgNoStairsDown :
                             Strings::MsgNoStairsUp);
    _stairsTileInfo.first = nullptr;
  }
  else
  {
    StairsComponent* sc = stairsTile->GetComponent<StairsComponent>();
    if (!sc->IsEnabled)
    {
      Game::gPrnt.AddMessage((stairsSymbol == '>') ?
                               Strings::MsgNoStairsDown :
                               Strings::MsgNoStairsUp);
      _stairsTileInfo.first = nullptr;
    }
    else
    {
      _stairsTileInfo.first  = stairsTile;
      _stairsTileInfo.second = (stairsSymbol == '>');
    }
  }

  return _stairsTileInfo;
}

// =============================================================================

void MainState::ClimbStairs(const std::pair<GameObject*, bool>& stairsTileInfo)
{
  bool shouldGoDown = stairsTileInfo.second;

  StairsComponent* stairs =
      stairsTileInfo.first->GetComponent<StairsComponent>();

  if (stairs->IsEnabled)
  {
    Game::gMap.ChangeLevel(stairs->LeadsTo, shouldGoDown);
  }
}

// =============================================================================

#ifdef DEBUG_BUILD
void MainState::PrintDebugInfo()
{
  MapLevelBase* curLvl = Game::gMap.CurrentLevel;

  _debugInfo = Util::StringFormat("Act: %i Ofst: %i %i Pos: [%i;%i] Hngr: %i",
                                  _playerRef->Attrs.ActionMeter,
                                  curLvl->MapOffsetX,
                                  curLvl->MapOffsetY,
                                  _playerRef->PosX,
                                  _playerRef->PosY,
                                  _playerRef->Attrs.Hunger);

  Game::gPrnt.PrintFB(1,
                      0,
                      _debugInfo,
                      Printer::kAlignLeft,
                      Colors::WhiteColor,
                      Colors::BlackColor);

  _debugInfo = Util::StringFormat("GO = %lu Actors = %lu",
                                  curLvl->GameObjects.size(),
                                  curLvl->ActorGameObjects.size());

  Game::gPrnt.PrintFB(1,
                      1,
                      _debugInfo,
                      Printer::kAlignLeft,
                      Colors::WhiteColor,
                      Colors::BlackColor);

  _debugInfo = Util::StringFormat("Key: %i", _keyPressed);

  Game::gPrnt.PrintFB(1,
                      2,
                      _debugInfo,
                      Printer::kAlignLeft,
                      Colors::WhiteColor,
                      Colors::BlackColor);

  _debugInfo = Util::StringFormat("Start: [%i;%i]",
                                  curLvl->LevelStart.X,
                                  curLvl->LevelStart.Y);

  Game::gPrnt.PrintFB(1,
                      3,
                      _debugInfo,
                      Printer::kAlignLeft,
                      Colors::WhiteColor,
                      Colors::BlackColor);

  _debugInfo = Util::StringFormat("Exit: [%i;%i]",
                                  curLvl->LevelExit.X,
                                  curLvl->LevelExit.Y);

  Game::gPrnt.PrintFB(1,
                      4,
                      _debugInfo,
                      Printer::kAlignLeft,
                      Colors::WhiteColor,
                      Colors::BlackColor);

  _debugInfo = Util::StringFormat("Colors: %i",
                                  Game::gPrnt.ColorsUsed());

  Game::gPrnt.PrintFB(1,
                      5,
                      _debugInfo,
                      Printer::kAlignLeft,
                      Colors::WhiteColor,
                      Colors::BlackColor);

  _debugInfo =
      Util::StringFormat("PT: %llu MU: %llu",
                         Game::gApp.PlayerTurnsPassed,
                         Game::gApp.MapUpdateCyclesPassed);

  Game::gPrnt.PrintFB(1,
                      6,
                      _debugInfo,
                      Printer::kAlignLeft,
                      Colors::WhiteColor,
                      Colors::BlackColor);

  Game::gPrnt.PrintFB(1,
                      7,
                      "Actors watched:",
                      Printer::kAlignLeft,
                      Colors::WhiteColor,
                      Colors::BlackColor);

  int yOffset = 0;
  bool found = false;
  for (auto& id : _actorsForDebugDisplay)
  {
    for (auto& a : curLvl->ActorGameObjects)
    {
      if (a->ObjectId() == id)
      {
        _debugInfo = Util::StringFormat("%s_%llu (%i)",
                                        a->ObjectName.data(),
                                        id,
                                        a->Attrs.ActionMeter);

        Game::gPrnt.PrintFB(1,
                            8 + yOffset,
                            _debugInfo,
                            Printer::kAlignLeft,
                            Colors::WhiteColor,
                            Colors::BlackColor);
        yOffset++;
        found = true;
      }
    }
  }

  if (!found)
  {
    Game::gPrnt.PrintFB(1,
                        8,
                        "NONE",
                        Printer::kAlignLeft,
                        Colors::WhiteColor,
                        Colors::BlackColor);
  }
}
#endif

// =============================================================================

void MainState::ProcessRangedWeapon()
{
  if (Game::gMap.CurrentLevel->Peaceful)
  {
    // NOTE: comment out all lines for debug if needed
    PrintNoAttackInTown();
    return;
  }

  if (_playerRef->IsSwimming())
  {
    Game::gPrnt.AddMessage(Strings::MsgNotInWater);
    return;
  }

  // TODO: wands in both hands?

  ItemComponent* weapon =
      _playerRef->Equipment->EquipmentByCategory[EquipmentCategory::WEAPON][0];

  if (weapon != nullptr)
  {
    if (weapon->Data.ItemType_ == ItemType::WAND)
    {
      ProcessWand(weapon);
    }
    else if (weapon->Data.ItemType_ == ItemType::RANGED_WEAPON)
    {
      ProcessWeapon(weapon);
    }
    else
    {
      Game::gPrnt.AddMessage(Strings::MsgNotRangedWeapon);
    }
  }
  else
  {
    Game::gPrnt.AddMessage(Strings::MsgEquipWeapon);
  }
}

// =============================================================================

void MainState::ProcessWeapon(ItemComponent* weapon)
{
  ItemComponent* arrows =
      _playerRef->Equipment->EquipmentByCategory[EquipmentCategory::SHIELD][0];

  RangedWeaponType wt = weapon->Data.RangedWeaponType_;

  if (arrows != nullptr)
  {
    bool isBow = (wt == RangedWeaponType::SHORT_BOW
               || wt == RangedWeaponType::LONGBOW
               || wt == RangedWeaponType::WAR_BOW);

    bool isXBow = (wt == RangedWeaponType::LIGHT_XBOW
                || wt == RangedWeaponType::XBOW
                || wt == RangedWeaponType::HEAVY_XBOW);

    if (arrows->Data.ItemType_ != ItemType::ARROWS)
    {
      Game::gPrnt.AddMessage(Strings::MsgWhatToShoot);
    }
    else
    {
      if ( (isBow && arrows->Data.AmmoType == ArrowType::BOLTS)
        || (isXBow && arrows->Data.AmmoType == ArrowType::ARROWS) )
      {
        Game::gPrnt.AddMessage(Strings::MsgWrongAmmo);
        return;
      }

      if (arrows->Data.Amount == 0)
      {
        Game::gPrnt.AddMessage(Strings::MsgNoAmmo);
      }
      else
      {
        auto s = Game::gApp.GetGameStateRefByName(
                   GameStates::TARGET_STATE
        );

        TargetState* ts = static_cast<TargetState*>(s);
        ts->Setup(weapon);
        Game::gApp.ChangeState(GameStates::TARGET_STATE);
      }
    }
  }
  else
  {
    Game::gPrnt.AddMessage(Strings::MsgWhatToShoot);
  }
}

// =============================================================================

void MainState::ProcessWand(ItemComponent* wand)
{
  //
  // NOTE: amount of charges should be subtracted
  // separately in corresponding methods
  // (i.e. EffectsProcessor or inside TargetState),
  // because combat wands require targeting,
  // which is checked against out of bounds,
  // and only after it's OK and player hits "fire",
  // the actual firing takes place.
  //
  if (wand->Data.Amount == 0)
  {
    Game::gPrnt.AddMessage(Strings::MsgNoCharges);
  }
  else
  {
    switch (wand->Data.SpellHeld.SpellType_)
    {
      //
      // TODO: finish wands effects and attack
      // (e.g. wand of heal others etc.)
      //
      case SpellType::LIGHT:
        Game::gSP.ProcessWand(wand);
        break;

      case SpellType::STRIKE:
      case SpellType::FROST:
      case SpellType::TELEPORT:
      case SpellType::FIREBALL:
      case SpellType::LASER:
      case SpellType::LIGHTNING:
      case SpellType::MAGIC_MISSILE:
      case SpellType::NONE:
      {
        auto s = Game::gApp.GetGameStateRefByName(
          GameStates::TARGET_STATE
        );

        TargetState* ts = static_cast<TargetState*>(s);
        ts->Setup(wand);
        Game::gApp.ChangeState(GameStates::TARGET_STATE);
      }
      break;

      default:
        break;
    }
  }
}

// =============================================================================

bool MainState::ProcessMoneyPickup(std::pair<int, GameObject*>& pair)
{
  ItemComponent* ic = pair.second->GetComponent<ItemComponent>();
  if (ic->Data.ItemType_ == ItemType::COINS)
  {
    auto message = Util::StringFormat(Strings::FmtPickedUpIS,
                                      ic->Data.Amount,
                                      ic->OwnerGameObject->ObjectName.data());
    Game::gPrnt.AddMessage(message);

    _playerRef->Money += ic->Data.Amount;
    auto it = Game::gMap.CurrentLevel->GameObjects.begin();
    Game::gMap.CurrentLevel->GameObjects.erase(it + pair.first);
    return true;
  }

  return false;
}

// =============================================================================

void MainState::ProcessItemPickup(std::pair<int, GameObject*>& pair)
{
  ItemComponent* ic = pair.second->GetComponent<ItemComponent>();

  auto go = Game::gMap.CurrentLevel->GameObjects[pair.first].release();

  _playerRef->Inventory->Add(go);

  std::string objName = ic->Data.IsIdentified
                      ? go->ObjectName
                      : ic->Data.UnidentifiedName;

  std::string message;
  if (ic->Data.IsStackable)
  {
    message = Util::StringFormat(Strings::FmtPickedUpIS,
                                 ic->Data.Amount,
                                 objName.data());
  }
  else
  {
    message = Util::StringFormat(Strings::FmtPickedUpS, objName.data());
  }

  Game::gPrnt.AddMessage(message);

  auto it = Game::gMap.CurrentLevel->GameObjects.begin();
  Game::gMap.CurrentLevel->GameObjects.erase(it + pair.first);
}

// =============================================================================

void MainState::DisplayStartHint()
{
  int th = Printer::TerminalHeight;

  Game::gPrnt.PrintFB(1,
                      th - 4,
                      '<',
                      Colors::WhiteColor,
                      Colors::DoorHighlightColor);

  auto curLvl = Game::gMap.CurrentLevel;
  int dx = curLvl->LevelStart.X - _playerRef->PosX;
  int dy = curLvl->LevelStart.Y - _playerRef->PosY;

  std::string dir;

  if (dy > 0)
  {
    dir += "S";
  }
  else if (dy < 0)
  {
    dir += "N";
  }

  if (dx > 0)
  {
    dir += "E";
  }
  else if (dx < 0)
  {
    dir += "W";
  }

  Game::gPrnt.PrintFB(2,
                      th - 4,
                      dir,
                      Printer::kAlignLeft,
                      Colors::WhiteColor,
                      Colors::BlackColor);
}

// =============================================================================

void MainState::DisplayExitHint()
{
  int th = Printer::TerminalHeight;

  Game::gPrnt.PrintFB(1,
                      th - 3,
                      '>',
                      Colors::WhiteColor,
                      Colors::DoorHighlightColor);

  std::string dir;

  auto curLvl = Game::gMap.CurrentLevel;
  if (curLvl->ExitFound)
  {
    int dx = curLvl->LevelExit.X - _playerRef->PosX;
    int dy = curLvl->LevelExit.Y - _playerRef->PosY;

    if (dy > 0)
    {
      dir += "S";
    }
    else if (dy < 0)
    {
      dir += "N";
    }

    if (dx > 0)
    {
      dir += "E";
    }
    else if (dx < 0)
    {
      dir += "W";
    }
  }

  Game::gPrnt.PrintFB(2,
                      th - 3,
                      curLvl->ExitFound ? dir : "??",
                      Printer::kAlignLeft,
                      Colors::WhiteColor,
                      Colors::BlackColor);
}

// =============================================================================

void MainState::DisplayStatusIcons()
{
  int startPos = 5;

  DisplayHungerStatus(startPos);
  DisplayWeaponCondition(startPos);
  DisplayArmorCondition(startPos);
  DisplayAmmoCondition(startPos);
  DisplayActiveEffects(startPos);
}

// =============================================================================

void MainState::DisplayHungerStatus(const int& startPos)
{
  if (_playerRef->IsStarving)
  {
    Game::gPrnt.PrintFB(startPos,
                        _th - 3,
                        '%',
                        Colors::WhiteColor,
                        Colors::RedColor);
  }
  else
  {
    int hungerMax = _playerRef->Attrs.HungerRate.Get();
    int part = hungerMax - hungerMax * 0.25;
    if (_playerRef->Attrs.Hunger >= part)
    {
      Game::gPrnt.PrintFB(startPos,
                          _th - 3,
                          '%',
                          Colors::WhiteColor,
                          0x999900);
    }
  }
}

// =============================================================================

void MainState::DisplayWeaponCondition(const int& startPos)
{
  ItemComponent* weapon =
      _playerRef->Equipment->EquipmentByCategory[EquipmentCategory::WEAPON][0];

  if (weapon != nullptr &&
     (weapon->Data.ItemType_ == ItemType::WEAPON
   || weapon->Data.ItemType_ == ItemType::RANGED_WEAPON))
  {
    int maxDur = weapon->Data.Durability.Max().Get();
    int warning = maxDur * 0.3;

    if (weapon->Data.Durability.Min().Get() <= warning)
    {
      Game::gPrnt.PrintFB(startPos + 2,
                          _th - 3,
                          ')',
                          Colors::YellowColor,
                          Colors::BlackColor);
    }
  }
}

// =============================================================================

void MainState::DisplayArmorCondition(const int& startPos)
{
  ItemComponent* armor =
      _playerRef->Equipment->EquipmentByCategory[EquipmentCategory::TORSO][0];

  if (armor != nullptr && armor->Data.ItemType_ == ItemType::ARMOR)
  {
    int maxDur = armor->Data.Durability.Max().Get();
    int warning = maxDur * 0.3;

    if (armor->Data.Durability.Min().Get() <= warning)
    {
      Game::gPrnt.PrintFB(startPos + 4,
                          _th - 3,
                          '[',
                          Colors::YellowColor,
                          Colors::BlackColor);
    }
  }
}

// =============================================================================

void MainState::DisplayAmmoCondition(const int& startPos)
{
  ItemComponent* arrows =
      _playerRef->Equipment->EquipmentByCategory[EquipmentCategory::SHIELD][0];

  if (arrows != nullptr && arrows->Data.ItemType_ == ItemType::ARROWS)
  {
    int amount = arrows->Data.Amount;
    if (amount <= 3)
    {
      Game::gPrnt.PrintFB(startPos + 6,
                          _th - 3,
                          '^',
                          Colors::YellowColor,
                          Colors::BlackColor);
    }
  }
}

// =============================================================================

void MainState::DisplayActiveEffects(const int& startPos)
{
  int offsetX = startPos;

  std::unordered_map<std::string, int> effectDurationByName;

  for (auto& kvp : _playerRef->GetActiveEffects())
  {
    for (const ItemBonusStruct& item : kvp.second)
    {
      std::string shortName =
          GlobalConstants::BonusDisplayNameByType.at(item.Type);

      int duration = item.Duration;
      if (duration != -1)
      {
        effectDurationByName[shortName] += duration;
      }
      else
      {
        effectDurationByName[shortName] = duration;
      }
    }
  }

  for (auto& kvp : effectDurationByName)
  {
    bool isFading = (kvp.second <= GlobalConstants::TurnReadyValue
                  && kvp.second != -1);

    uint32_t color = isFading ?
                     Colors::ShadesOfGrey::Four :
                     Colors::WhiteColor;

    Game::gPrnt.PrintFB(offsetX,
                        _th - 4,
                        kvp.first,
                        Printer::kAlignLeft,
                        color,
                        Colors::BlackColor);

    offsetX += 4;
  }
}

// =============================================================================

void MainState::PrintNoAttackInTown()
{
  int index = Game::gRng.RandomRange(0, 2);
  Game::gPrnt.AddMessage(Strings::MsgNotInTown[index]);
}

// =============================================================================

void MainState::GetActorsAround()
{
  _actorsForDebugDisplay.clear();

  int lx = _playerRef->PosX - 1;
  int ly = _playerRef->PosY - 1;
  int hx = _playerRef->PosX + 1;
  int hy = _playerRef->PosY + 1;

  if (lx >= 0 && ly >= 0
   && hx < Game::gMap.CurrentLevel->MapSize.X - 1
   && hy < Game::gMap.CurrentLevel->MapSize.Y - 1)
  {
    for (int x = lx; x <= hx; x++)
    {
      for (int y = ly; y <= hy; y++)
      {
        for (auto& a : Game::gMap.CurrentLevel->ActorGameObjects)
        {
          if (a->PosX == x && a->PosY == y)
          {
            _actorsForDebugDisplay.push_back(a->ObjectId());
          }
        }
      }
    }
  }
}

// =============================================================================

void MainState::DisplayScenarioInformation()
{
  std::vector<std::string> messages;

  auto seedString = Game::gRng.GetSeedString();

  std::stringstream ss;

  ss << "Seed string: \"" << seedString.first << "\"";
  messages.push_back(ss.str());

  ss.str("");

  ss << "Seed value: " << seedString.second;
  messages.push_back(ss.str());

  Game::gApp.ShowMessageBox(MessageBoxType::ANY_KEY,
                            "Scenario Information", messages);
}
