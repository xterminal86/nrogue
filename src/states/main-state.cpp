#include "main-state.h"

#include "application.h"
#include "map.h"
#include "printer.h"
#include "stairs-component.h"
#include "target-state.h"
#include "spells-processor.h"
#include "pickup-item-state.h"

void MainState::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;

  _keysToRecord[ALT_K7]   = true;
  _keysToRecord[NUMPAD_7] = true;
  _keysToRecord[ALT_K8]   = true;
  _keysToRecord[NUMPAD_8] = true;
  _keysToRecord[ALT_K9]   = true;
  _keysToRecord[NUMPAD_9] = true;
  _keysToRecord[ALT_K4]   = true;
  _keysToRecord[NUMPAD_4] = true;
  _keysToRecord[ALT_K2]   = true;
  _keysToRecord[NUMPAD_2] = true;
  _keysToRecord[ALT_K6]   = true;
  _keysToRecord[NUMPAD_6] = true;
  _keysToRecord[ALT_K1]   = true;
  _keysToRecord[NUMPAD_1] = true;
  _keysToRecord[ALT_K3]   = true;
  _keysToRecord[NUMPAD_3] = true;
  _keysToRecord[ALT_K5]   = true;
  _keysToRecord[NUMPAD_5] = true;
  _keysToRecord['a']      = true;
  _keysToRecord['f']      = true;
  _keysToRecord['e']      = true;
  _keysToRecord['i']      = true;
  _keysToRecord['g']      = true;
  _keysToRecord['>']      = true;
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
    Application::Instance().ChangeState(GameStates::ENDGAME_STATE);
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
      Printer::Instance().AddMessage(Strings::MsgWait);
      _playerRef->FinishTurn();
      break;
    // -------------------------------------------------------------------------
    case 'a':
    {
      if (Map::Instance().CurrentLevel->Peaceful)
      {
        PrintNoAttackInTown();
      }
      else if (_playerRef->IsSwimming())
      {
        Printer::Instance().AddMessage(Strings::MsgNotInWater);
      }
      else
      {
        Application::Instance().ChangeState(GameStates::ATTACK_STATE);
      }
    }
    break;
    // -------------------------------------------------------------------------
    case '$':
    {
      auto str = Util::StringFormat("You have %i %s",
                                    _playerRef->Money,
                                    Strings::MoneyName.data());
      Printer::Instance().AddMessage(str);
    }
    break;
    // -------------------------------------------------------------------------
    case 'e':
      Application::Instance().ChangeState(GameStates::INVENTORY_STATE);
      break;
    // -------------------------------------------------------------------------
    case 'm':
      Application::Instance().ChangeState(GameStates::SHOW_MESSAGES_STATE);
      break;
    // -------------------------------------------------------------------------
    case 'l':
      Application::Instance().ChangeState(GameStates::LOOK_INPUT_STATE);
      break;
    // -------------------------------------------------------------------------
    case 'i':
      Application::Instance().ChangeState(GameStates::INTERACT_INPUT_STATE);
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
      Application::Instance().ChangeState(GameStates::INFO_STATE);
      break;
    // -------------------------------------------------------------------------
    case 'H':
    case 'h':
    case '?':
      Application::Instance().ChangeState(GameStates::HELP_STATE);
      break;
    // -------------------------------------------------------------------------
    case 'Q':
      Application::Instance().ChangeState(GameStates::EXITING_STATE);
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
      Application::Instance().ChangeState(GameStates::SAVE_GAME_STATE);
      break;
    // -------------------------------------------------------------------------
#ifdef DEBUG_BUILD
    case '`':
      Application::Instance().ChangeState(GameStates::DEV_CONSOLE);
      break;

    case 'T':
    {
      int exitX = Map::Instance().CurrentLevel->LevelExit.X;
      int exitY = Map::Instance().CurrentLevel->LevelExit.Y;

      if (_playerRef->MoveTo(exitX, exitY))
      {
        Map::Instance().CurrentLevel->AdjustCamera();
        Update(true);
        _playerRef->FinishTurn();
      }
      else
      {
        auto str = Util::StringFormat("[%i;%i] is occupied!", exitX, exitY);
        Printer::Instance().AddMessage(str);
        DebugLog("%s\n", str.data());
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

  RECORD_ACTION(_keyPressed);
}

// =============================================================================

void MainState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    _playerRef->CheckVisibility();

    Map::Instance().Draw();

    _playerRef->Draw();

    DisplayStartHint();
    DisplayExitHint();
    DisplayStatusIcons();
    DrawHPMP();

    if (Printer::Instance().ShowLastMessage)
    {
      DisplayGameLog();
    }
    else
    {
      Printer::Instance().ResetMessagesToDisplay();
    }

    Printer::Instance().PrintFB(Printer::TerminalWidth - 1,
                                0,
                                Map::Instance().CurrentLevel->LevelName,
                                Printer::kAlignRight,
                                Colors::WhiteColor);

    #ifdef DEBUG_BUILD
    PrintDebugInfo();
    #endif

    Printer::Instance().Render();
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
    Printer::Instance().ShowLastMessage = false;

    CheckItemsOnGround();

    Map::Instance().CurrentLevel->MapOffsetX -= dirOffsets.X;
    Map::Instance().CurrentLevel->MapOffsetY -= dirOffsets.Y;

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

    GameObject* tile = Map::Instance().CurrentLevel->MapArray[px][py].get();

    bool stairsHere = (tile->Image == '>' || tile->Image == '<');
    if (stairsHere)
    {
      StairsComponent* sc = tile->GetComponent<StairsComponent>();
      if (sc->IsEnabled)
      {
        Printer::Instance().AddMessage((tile->Image == '>')
                                       ? Strings::MsgStairsDown
                                       : Strings::MsgStairsUp);
      }
    }
  }
}

// =============================================================================

void MainState::CheckItemsOnGround()
{
  auto items = Map::Instance().GetGameObjectsToPickup(_playerRef->PosX, _playerRef->PosY);
  if (items.size() > 1)
  {
    Printer::Instance().AddMessage(Strings::MsgItemsLyingHere);
  }
}

// =============================================================================

void MainState::DisplayGameLog()
{
  int x = Printer::TerminalWidth - 1;
  int y = Printer::TerminalHeight - 1;

  int count = 0;
  auto msgs = Printer::Instance().GetLastMessages();
  for (auto& m : msgs)
  {
    Printer::Instance().PrintFB(x,
                                y - count,
                                m.Message,
                                Printer::kAlignRight,
                                m.FgColor,
                                m.BgColor);
    count++;
  }
}

// =============================================================================

void MainState::TryToPickupItems()
{
  auto items = Map::Instance().GetGameObjectsToPickup(_playerRef->PosX, _playerRef->PosY);
  if (!items.empty())
  {
    if (items.size() == 1)
    {
      PickupSingleItem(items[0]);
    }
    else
    {
      auto gs = Application::Instance().GetGameStateRefByName(GameStates::PICKUP_ITEM_STATE);
      PickupItemState* pis = static_cast<PickupItemState*>(gs);
      pis->Setup(items);
      Application::Instance().ChangeState(GameStates::PICKUP_ITEM_STATE);
    }
  }
  else
  {
    Printer::Instance().AddMessage(Strings::MsgNothingHere);
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
    Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                           Strings::MessageBoxEpicFailHeaderText,
                                           { Strings::MsgInventoryFull },
                                           Colors::MessageBoxRedBorderColor);
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
  Printer::Instance().PrintFB(GlobalConstants::HPMPBarLength / 2,
                              th - 2,
                              str,
                              Printer::kAlignCenter,
                              Colors::WhiteColor,
                              0x880000);

  UpdateBar(1, th - 1, _playerRef->Attrs.MP);

  str = Util::StringFormat("%i/%i", curMp, maxMp);
  Printer::Instance().PrintFB(GlobalConstants::HPMPBarLength / 2,
                              th - 1,
                              str,
                              Printer::kAlignCenter,
                              Colors::WhiteColor,
                              0x000088);

#ifdef USE_SDL
  int expCur = _playerRef->Attrs.Exp.Min().Get();
  int expMax = _playerRef->Attrs.Exp.Max().Get();

  auto tws = Printer::Instance().GetTileWHScaled();

  int xPos1 = tws.first * 2;
  int xPos2 = GlobalConstants::HPMPBarLength * tws.first;
  xPos2 = xPos1 + (int)(((double)expCur / (double)expMax) * (double)xPos2);
  int yPos = (th - 1) * tws.second;

  Printer::Instance().DrawRect(xPos1, yPos - 1, xPos2, yPos + 1, 0xFFFF00);
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

  Printer::Instance().PrintFB(x,
                              y,
                              bar,
                              Printer::kAlignLeft,
                              Colors::WhiteColor);
}

// =============================================================================

std::pair<GameObject*, bool> MainState::CheckStairs(int stairsSymbol)
{
  GameObject* stairsTile = Map::Instance().CurrentLevel->MapArray[_playerRef->PosX][_playerRef->PosY].get();

  //
  // We're relying here on stairsSymbol to be exactly '>' or '<'
  //
  if (stairsTile->Image != stairsSymbol)
  {
    Printer::Instance().AddMessage((stairsSymbol == '>')
                                  ? Strings::MsgNoStairsDown
                                  : Strings::MsgNoStairsUp);
    _stairsTileInfo.first = nullptr;
  }
  else
  {
    StairsComponent* sc = stairsTile->GetComponent<StairsComponent>();
    if (!sc->IsEnabled)
    {
      Printer::Instance().AddMessage((stairsSymbol == '>')
                                    ? Strings::MsgNoStairsDown
                                    : Strings::MsgNoStairsUp);
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
  StairsComponent* stairs = stairsTileInfo.first->GetComponent<StairsComponent>();
  if (stairs->IsEnabled)
  {
    Map::Instance().ChangeLevel(stairs->LeadsTo, shouldGoDown);
  }
}

// =============================================================================

void MainState::PrintDebugInfo()
{
  _debugInfo = Util::StringFormat("Act: %i Ofst: %i %i: Plr: [%i;%i] Hunger: %i",
                                  _playerRef->Attrs.ActionMeter,
                                  Map::Instance().CurrentLevel->MapOffsetX,
                                  Map::Instance().CurrentLevel->MapOffsetY,
                                  _playerRef->PosX,
                                  _playerRef->PosY,
                                  _playerRef->Attrs.Hunger);

  Printer::Instance().PrintFB(1, 1, _debugInfo, Printer::kAlignLeft, Colors::WhiteColor);

  _debugInfo = Util::StringFormat("Key: %i Actors: %i Respawn: %i",
                                  _keyPressed,
                                  Map::Instance().CurrentLevel->ActorGameObjects.size(),
                                  Map::Instance().CurrentLevel->RespawnCounter());

  Printer::Instance().PrintFB(1, 2, _debugInfo, Printer::kAlignLeft, Colors::WhiteColor);

  _debugInfo = Util::StringFormat("Level Start: [%i;%i]", Map::Instance().CurrentLevel->LevelStart.X, Map::Instance().CurrentLevel->LevelStart.Y);
  Printer::Instance().PrintFB(1, 3, _debugInfo, Printer::kAlignLeft, Colors::WhiteColor);

  _debugInfo = Util::StringFormat("Level Exit: [%i;%i]", Map::Instance().CurrentLevel->LevelExit.X, Map::Instance().CurrentLevel->LevelExit.Y);
  Printer::Instance().PrintFB(1, 4, _debugInfo, Printer::kAlignLeft, Colors::WhiteColor);

  _debugInfo = Util::StringFormat("Colors Used: %i", Printer::Instance().ColorsUsed());
  Printer::Instance().PrintFB(1, 5, _debugInfo, Printer::kAlignLeft, Colors::WhiteColor);

  _debugInfo = Util::StringFormat("PT: %lu MU: %lu", Application::Instance().PlayerTurnsPassed, Application::Instance().MapUpdateCyclesPassed);
  Printer::Instance().PrintFB(1, 6, _debugInfo, Printer::kAlignLeft, Colors::WhiteColor);

  Printer::Instance().PrintFB(1, 7, "Actors watched:", Printer::kAlignLeft, Colors::WhiteColor);

  int yOffset = 0;
  bool found = false;
  for (auto& id : _actorsForDebugDisplay)
  {
    for (auto& a : Map::Instance().CurrentLevel->ActorGameObjects)
    {
      if (a->ObjectId() == id)
      {
        _debugInfo = Util::StringFormat("%s_%lu (%i)", a->ObjectName.data(), id, a->Attrs.ActionMeter);
        Printer::Instance().PrintFB(1, 8 + yOffset, _debugInfo, Printer::kAlignLeft, Colors::WhiteColor);
        yOffset++;
        found = true;
      }
    }
  }

  if (!found)
  {
    Printer::Instance().PrintFB(1, 8, "<Press 's' to scan 1x1 around player>", Printer::kAlignLeft, Colors::WhiteColor);
  }
}

// =============================================================================

void MainState::ProcessRangedWeapon()
{
  if (Map::Instance().CurrentLevel->Peaceful)
  {
    // NOTE: comment out all lines for debug if needed
    PrintNoAttackInTown();
    return;
  }

  if (_playerRef->IsSwimming())
  {
    Printer::Instance().AddMessage(Strings::MsgNotInWater);
    return;
  }

  // TODO: wands in both hands?

  ItemComponent* weapon = _playerRef->Equipment->EquipmentByCategory[EquipmentCategory::WEAPON][0];
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
      Printer::Instance().AddMessage(Strings::MsgNotRangedWeapon);
    }
  }
  else
  {
    Printer::Instance().AddMessage(Strings::MsgEquipWeapon);
  }
}

// =============================================================================

void MainState::ProcessWeapon(ItemComponent* weapon)
{
  ItemComponent* arrows = _playerRef->Equipment->EquipmentByCategory[EquipmentCategory::SHIELD][0];
  if (arrows != nullptr)
  {
    bool isBow = (weapon->Data.RangedWeaponType_ == RangedWeaponType::SHORT_BOW
               || weapon->Data.RangedWeaponType_ == RangedWeaponType::LONGBOW
               || weapon->Data.RangedWeaponType_ == RangedWeaponType::WAR_BOW);

    bool isXBow = (weapon->Data.RangedWeaponType_ == RangedWeaponType::LIGHT_XBOW
                || weapon->Data.RangedWeaponType_ == RangedWeaponType::XBOW
                || weapon->Data.RangedWeaponType_ == RangedWeaponType::HEAVY_XBOW);

    if (arrows->Data.ItemType_ != ItemType::ARROWS)
    {
      Printer::Instance().AddMessage(Strings::MsgWhatToShoot);
    }
    else
    {
      if ( (isBow && arrows->Data.AmmoType == ArrowType::BOLTS)
        || (isXBow && arrows->Data.AmmoType == ArrowType::ARROWS) )
      {
        Printer::Instance().AddMessage(Strings::MsgWrongAmmo);
        return;
      }

      if (arrows->Data.Amount == 0)
      {
        Printer::Instance().AddMessage(Strings::MsgNoAmmo);
      }
      else
      {
        auto s = Application::Instance().GetGameStateRefByName(GameStates::TARGET_STATE);
        TargetState* ts = static_cast<TargetState*>(s);
        ts->Setup(weapon);
        Application::Instance().ChangeState(GameStates::TARGET_STATE);
      }
    }
  }
  else
  {
    Printer::Instance().AddMessage(Strings::MsgWhatToShoot);
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
    Printer::Instance().AddMessage(Strings::MsgNoCharges);
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
        SpellsProcessor::Instance().ProcessWand(wand);
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
        auto s = Application::Instance().GetGameStateRefByName(GameStates::TARGET_STATE);
        TargetState* ts = static_cast<TargetState*>(s);
        ts->Setup(wand);
        Application::Instance().ChangeState(GameStates::TARGET_STATE);
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
    auto message = Util::StringFormat(Strings::FmtPickedUpIS, ic->Data.Amount, ic->OwnerGameObject->ObjectName.data());
    Printer::Instance().AddMessage(message);

    _playerRef->Money += ic->Data.Amount;
    auto it = Map::Instance().CurrentLevel->GameObjects.begin();
    Map::Instance().CurrentLevel->GameObjects.erase(it + pair.first);
    return true;
  }

  return false;
}

// =============================================================================

void MainState::ProcessItemPickup(std::pair<int, GameObject*>& pair)
{
  ItemComponent* ic = pair.second->GetComponent<ItemComponent>();

  auto go = Map::Instance().CurrentLevel->GameObjects[pair.first].release();

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

  Printer::Instance().AddMessage(message);

  auto it = Map::Instance().CurrentLevel->GameObjects.begin();
  Map::Instance().CurrentLevel->GameObjects.erase(it + pair.first);
}

// =============================================================================

void MainState::DisplayStartHint()
{
  int th = Printer::TerminalHeight;

  Printer::Instance().PrintFB(1,
                              th - 4,
                              '<',
                              Colors::WhiteColor,
                              Colors::DoorHighlightColor);

  auto curLvl = Map::Instance().CurrentLevel;
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

  Printer::Instance().PrintFB(2,
                              th - 4,
                              dir,
                              Printer::kAlignLeft,
                              Colors::WhiteColor);
}

// =============================================================================

void MainState::DisplayExitHint()
{
  int th = Printer::TerminalHeight;

  Printer::Instance().PrintFB(1,
                              th - 3,
                              '>',
                              Colors::WhiteColor,
                              Colors::DoorHighlightColor);

  std::string dir;

  auto curLvl = Map::Instance().CurrentLevel;
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

  Printer::Instance().PrintFB(2,
                              th - 3,
                              curLvl->ExitFound ? dir : "??",
                              Printer::kAlignLeft,
                              Colors::WhiteColor);
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
    Printer::Instance().PrintFB(startPos,
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
      Printer::Instance().PrintFB(startPos,
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
  ItemComponent* weapon = _playerRef->Equipment->EquipmentByCategory[EquipmentCategory::WEAPON][0];
  if (weapon != nullptr &&
     (weapon->Data.ItemType_ == ItemType::WEAPON
   || weapon->Data.ItemType_ == ItemType::RANGED_WEAPON))
  {
    int maxDur = weapon->Data.Durability.Max().Get();
    int warning = maxDur * 0.3;

    if (weapon->Data.Durability.Min().Get() <= warning)
    {
      Printer::Instance().PrintFB(startPos + 2,
                                  _th - 3,
                                  ')',
                                  Colors::YellowColor);
    }
  }
}

// =============================================================================

void MainState::DisplayArmorCondition(const int& startPos)
{
  ItemComponent* armor = _playerRef->Equipment->EquipmentByCategory[EquipmentCategory::TORSO][0];
  if (armor != nullptr && armor->Data.ItemType_ == ItemType::ARMOR)
  {
    int maxDur = armor->Data.Durability.Max().Get();
    int warning = maxDur * 0.3;

    if (armor->Data.Durability.Min().Get() <= warning)
    {
      Printer::Instance().PrintFB(startPos + 4,
                                  _th - 3,
                                  '[',
                                  Colors::YellowColor);
    }
  }
}

// =============================================================================

void MainState::DisplayAmmoCondition(const int& startPos)
{
  ItemComponent* arrows = _playerRef->Equipment->EquipmentByCategory[EquipmentCategory::SHIELD][0];
  if (arrows != nullptr && arrows->Data.ItemType_ == ItemType::ARROWS)
  {
    int amount = arrows->Data.Amount;
    if (amount <= 3)
    {
      Printer::Instance().PrintFB(startPos + 6,
                                  _th - 3,
                                  '^',
                                  Colors::YellowColor);
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
      std::string shortName = GlobalConstants::BonusDisplayNameByType.at(item.Type);
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

    Printer::Instance().PrintFB(offsetX,
                                _th - 4,
                                kvp.first,
                                Printer::kAlignLeft,
                                color);

    offsetX += 4;
  }
}

// =============================================================================

void MainState::PrintNoAttackInTown()
{
  int index = RNG::Instance().RandomRange(0, 2);
  Printer::Instance().AddMessage(Strings::MsgNotInTown[index]);
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
   && hx < Map::Instance().CurrentLevel->MapSize.X - 1
   && hy < Map::Instance().CurrentLevel->MapSize.Y - 1)
  {
    for (int x = lx; x <= hx; x++)
    {
      for (int y = ly; y <= hy; y++)
      {
        for (auto& a : Map::Instance().CurrentLevel->ActorGameObjects)
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

  auto seedString = RNG::Instance().GetSeedString();

  std::stringstream ss;

  ss << "Seed string: \"" << seedString.first << "\"";
  messages.push_back(ss.str());

  ss.str("");

  ss << "Seed value: " << seedString.second;
  messages.push_back(ss.str());

  Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Scenario Information", messages);
}
