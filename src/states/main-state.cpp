#include "main-state.h"

#include "application.h"
#include "map.h"
#include "printer.h"
#include "stairs-component.h"
#include "target-state.h"
#include "spells-processor.h"

#include <iomanip>

void MainState::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

void MainState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case ALT_K7:
    case NUMPAD_7:
      ProcessMovement({ -1, -1 });
      break;

    case ALT_K8:
    case NUMPAD_8:
      ProcessMovement({ 0, -1 });
      break;

    case ALT_K9:
    case NUMPAD_9:
      ProcessMovement({ 1, -1 });
      break;

    case ALT_K4:
    case NUMPAD_4:
      ProcessMovement({ -1, 0 });
      break;

    case ALT_K2:
    case NUMPAD_2:
      ProcessMovement({ 0, 1 });
      break;

    case ALT_K6:
    case NUMPAD_6:
      ProcessMovement({ 1, 0 });
      break;

    case ALT_K1:
    case NUMPAD_1:
      ProcessMovement({ -1, 1 });
      break;

    case ALT_K3:
    case NUMPAD_3:
      ProcessMovement({ 1, 1 });
      break;

    case ALT_K5:
    case NUMPAD_5:
      Printer::Instance().AddMessage("You waited...");
      _playerRef->FinishTurn();
      break;

    case 'a':
    {
      if (Map::Instance().CurrentLevel->Peaceful)
      {
        PrintNoAttackInTown();
      }
      else
      {
        Application::Instance().ChangeState(GameStates::ATTACK_STATE);
      }
    }
    break;

    case '$':
    {
      auto str = Util::StringFormat("You have %i %s", _playerRef->Money, GlobalConstants::MoneyName.data());
      Printer::Instance().AddMessage(str);
    }
    break;

    case 'e':
      Application::Instance().ChangeState(GameStates::INVENTORY_STATE);
      break;

    case 'm':
      Application::Instance().ChangeState(GameStates::SHOW_MESSAGES_STATE);
      break;

    case 'l':
      Application::Instance().ChangeState(GameStates::LOOK_INPUT_STATE);
      break;

    case 'i':
      Application::Instance().ChangeState(GameStates::INTERACT_INPUT_STATE);
      break;

    case 'I':
      DisplayScenarioInformation();
      break;

    case 'g':
      TryToPickupItem();
      break;

    case '@':
      Application::Instance().ChangeState(GameStates::INFO_STATE);
      break;

    case 'H':
    case 'h':
    case '?':
      Application::Instance().ChangeState(GameStates::HELP_STATE);
      break;

    case 'Q':
      Application::Instance().ChangeState(GameStates::EXITING_STATE);
      break;

    case 'f':
      ProcessRangedWeapon();
      break;

    case '>':
      CheckStairs('>');

      // FIXME: for debug, further going down won't work
      // because of this override.
      //
      // NOTE: Comment out CheckStairs() above to avoid pointer loop.
      //
      // CheckStairs() also uses Map::ChangeLevel() which in turn
      // can display welcome text via MessageBox(), which sets
      // previousState and currentState variables.
      // If leave CheckStairs() uncommented, then during first visit
      // on a level message box will be displayed.
      // It will set previousState to MainState
      // and then call below will "override" that previousState
      // to MessageBoxState and now we have both previousState
      // and currentState variables pointing to the same state,
      // which will lead to inability to close message box.
      //
      //Map::Instance().ChangeLevel(MapType::MINES_5, true);
      break;

    case '<':
      CheckStairs('<');
      break;

    #ifdef DEBUG_BUILD
    case 'L':
      _playerRef->LevelUp();
      break;

    case 'F':
    {
      _playerRef->DistanceField.Emanate();
      std::string field = _playerRef->DistanceField.GetFieldString();
      DebugLog("\n%s", field.data());
    }
    break;

    case 'P':
      //Map::Instance().PrintMapArrayRevealedStatus();
      Map::Instance().PrintMapLayout();
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

    default:
      break;
  }

  if (!_playerRef->IsAlive())
  {
    Application::Instance().ChangeState(GameStates::ENDGAME_STATE);
  }
}

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
                                 GlobalConstants::WhiteColor);

    #ifdef DEBUG_BUILD
    PrintDebugInfo();
    #endif

    Printer::Instance().Render();
  }
}

void MainState::ProcessMovement(const Position& dirOffsets)
{
  if (_playerRef->TryToAttack(dirOffsets.X, dirOffsets.Y))
  {
    _playerRef->FinishTurn();
  }
  else if (_playerRef->Move(dirOffsets.X, dirOffsets.Y))
  {
    // This line must be the first in order to
    // allow potential messages to show in FinishTurn()
    // (e.g. starvation damage message) after player moved.
    Printer::Instance().ShowLastMessage = false;

    Map::Instance().CurrentLevel->MapOffsetX -= dirOffsets.X;
    Map::Instance().CurrentLevel->MapOffsetY -= dirOffsets.Y;

    _playerRef->FinishTurn();
  }
}

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
                                 m,
                                 Printer::kAlignRight,
                                 GlobalConstants::WhiteColor);
    count++;
  }
}

void MainState::TryToPickupItem()
{
  auto res = Map::Instance().GetGameObjectToPickup(_playerRef->PosX, _playerRef->PosY);
  if (res.first != -1)
  {
    if (ProcessMoneyPickup(res))
    {
      CheckIfSomethingElseIsLyingHere({ _playerRef->PosX, _playerRef->PosY });
      return;
    }

    if (_playerRef->Inventory.IsFull())
    {
      Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Epic Fail", { "Inventory is full!" }, GlobalConstants::MessageBoxRedBorderColor);
      return;
    }

    ProcessItemPickup(res);

    CheckIfSomethingElseIsLyingHere({ _playerRef->PosX, _playerRef->PosY });
  }
  else
  {
    Printer::Instance().AddMessage("There's nothing here");
  }
}

void MainState::CheckIfSomethingElseIsLyingHere(const Position& pos)
{
  auto res = Map::Instance().GetGameObjectToPickup(pos.X, pos.Y);
  if (res.first != -1)
  {
    Printer::Instance().AddMessage("There's something else lying here");
  }
}

void MainState::DrawHPMP()
{
  int curHp = _playerRef->Attrs.HP.Min().Get();
  int maxHp = _playerRef->Attrs.HP.Max().Get();
  int curMp = _playerRef->Attrs.MP.Min().Get();
  int maxMp = _playerRef->Attrs.MP.Max().Get();

  int th = Printer::TerminalHeight;

  UpdateBar(0, th - 2, _playerRef->Attrs.HP);

  auto str = Util::StringFormat("%i/%i", curHp, maxHp);
  Printer::Instance().PrintFB(GlobalConstants::HPMPBarLength / 2,
                               th - 2,
                               str,
                               Printer::kAlignCenter,
                               GlobalConstants::WhiteColor,
                               "#880000");

  UpdateBar(0, th - 1, _playerRef->Attrs.MP);

  str = Util::StringFormat("%i/%i", curMp, maxMp);
  Printer::Instance().PrintFB(GlobalConstants::HPMPBarLength / 2, th - 1, str, Printer::kAlignCenter, "#FFFFFF", "#000088");
}

void MainState::UpdateBar(int x, int y, RangedAttribute& attr)
{
  float ratio = ((float)attr.Min().Get() / (float)attr.Max().Get());
  int len = ratio * GlobalConstants::HPMPBarLength;

  std::string bar = "[";
  for (int i = 0; i < GlobalConstants::HPMPBarLength; i++)
  {
    bar += (i < len) ? "=" : " ";
  }

  bar += "]";

  Printer::Instance().PrintFB(x, y, bar, Printer::kAlignLeft, GlobalConstants::WhiteColor);
}

void MainState::CheckStairs(int stairsSymbol)
{
  if (stairsSymbol == '>')
  {
    auto tile = Map::Instance().CurrentLevel->MapArray[_playerRef->PosX][_playerRef->PosY].get();
    if (tile->Image != '>')
    {
      Printer::Instance().AddMessage("There are no stairs leading down here");
      return;
    }

    Component* c = tile->GetComponent<StairsComponent>();
    StairsComponent* stairs = static_cast<StairsComponent*>(c);
    Map::Instance().ChangeLevel(stairs->LeadsTo, true);
  }
  else if (stairsSymbol == '<')
  {
    auto tile = Map::Instance().CurrentLevel->MapArray[_playerRef->PosX][_playerRef->PosY].get();
    if (tile->Image != '<')
    {
      Printer::Instance().AddMessage("There are no stairs leading up here");
      return;
    }

    Component* c = tile->GetComponent<StairsComponent>();
    StairsComponent* stairs = static_cast<StairsComponent*>(c);
    Map::Instance().ChangeLevel(stairs->LeadsTo, false);
  }
}

void MainState::PrintDebugInfo()
{
  _debugInfo = Util::StringFormat("Act: %i Ofst: %i %i: Plr: [%i;%i] Hunger: %i",
                                  _playerRef->Attrs.ActionMeter,
                                  Map::Instance().CurrentLevel->MapOffsetX,
                                  Map::Instance().CurrentLevel->MapOffsetY,
                                  _playerRef->PosX,
                                  _playerRef->PosY,
                                  _playerRef->Attrs.Hunger);

  Printer::Instance().PrintFB(0, 1, _debugInfo, Printer::kAlignLeft, GlobalConstants::WhiteColor);

  _debugInfo = Util::StringFormat("Key: %i Actors: %i Respawn: %i",
                                  _keyPressed,
                                  Map::Instance().CurrentLevel->ActorGameObjects.size(),
                                  Map::Instance().CurrentLevel->RespawnCounter());

  Printer::Instance().PrintFB(0, 2, _debugInfo, Printer::kAlignLeft, GlobalConstants::WhiteColor);

  _debugInfo = Util::StringFormat("Level Start: [%i;%i]", Map::Instance().CurrentLevel->LevelStart.X, Map::Instance().CurrentLevel->LevelStart.Y);
  Printer::Instance().PrintFB(0, 3, _debugInfo, Printer::kAlignLeft, GlobalConstants::WhiteColor);

  _debugInfo = Util::StringFormat("Level Exit: [%i;%i]", Map::Instance().CurrentLevel->LevelExit.X, Map::Instance().CurrentLevel->LevelExit.Y);
  Printer::Instance().PrintFB(0, 4, _debugInfo, Printer::kAlignLeft, GlobalConstants::WhiteColor);

  _debugInfo = Util::StringFormat("Colors Used: %i", Printer::Instance().ColorsUsed());
  Printer::Instance().PrintFB(0, 5, _debugInfo, Printer::kAlignLeft, GlobalConstants::WhiteColor);

  _debugInfo = Util::StringFormat("Turns passed: %i", Application::Instance().TurnsPassed);
  Printer::Instance().PrintFB(0, 6, _debugInfo, Printer::kAlignLeft, GlobalConstants::WhiteColor);

  Printer::Instance().PrintFB(0, 7, "Actors watched:", Printer::kAlignLeft, GlobalConstants::WhiteColor);

  int yOffset = 0;
  bool found = false;
  for (auto& id : _actorsForDebugDisplay)
  {
    for (auto& a : Map::Instance().CurrentLevel->ActorGameObjects)
    {
      if (a->ObjectId() == id)
      {
        _debugInfo = Util::StringFormat("%s_%lu (%i)", a->ObjectName.data(), id, a->Attrs.ActionMeter);
        Printer::Instance().PrintFB(0, 8 + yOffset, _debugInfo, Printer::kAlignLeft, GlobalConstants::WhiteColor);
        yOffset++;
        found = true;
      }
    }
  }

  if (!found)
  {
    Printer::Instance().PrintFB(0, 8, "<Press 's' to scan 1x1 around player>", Printer::kAlignLeft, GlobalConstants::WhiteColor);
  }
}

void MainState::ProcessRangedWeapon()
{
  if (Map::Instance().CurrentLevel->Peaceful)
  {
    // NOTE: comment out all lines for debug if needed
    PrintNoAttackInTown();
    return;
  }

  // TODO: wands in both hands?

  ItemComponent* weapon = _playerRef->EquipmentByCategory[EquipmentCategory::WEAPON][0];
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
      Printer::Instance().AddMessage("Not a ranged weapon!");
    }
  }
  else
  {
    Printer::Instance().AddMessage("Equip a weapon first!");
  }
}

void MainState::ProcessWeapon(ItemComponent* weapon)
{
  ItemComponent* arrows = _playerRef->EquipmentByCategory[EquipmentCategory::SHIELD][0];
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
      Printer::Instance().AddMessage("What are you going to shoot with?");
    }
    else
    {
      if ( (isBow && arrows->Data.AmmoType == ArrowType::BOLTS)
        || (isXBow && arrows->Data.AmmoType == ArrowType::ARROWS) )
      {
        Printer::Instance().AddMessage("Wrong ammunition type!");
        return;
      }

      if (arrows->Data.Amount == 0)
      {
        Printer::Instance().AddMessage("No ammunition!");
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
    Printer::Instance().AddMessage("What are you going to shoot with?");
  }
}

void MainState::ProcessWand(ItemComponent* wand)
{
  // NOTE: amount of charges should be subtracted
  // separately in corresponding methods
  // (i.e. EffectsProcessor or inside TargetState),
  // because combat wands require targeting,
  // which is checked against out of bounds,
  // and only after it's OK and player hits "fire",
  // the actual firing takes place.

  if (wand->Data.Amount == 0)
  {
    Printer::Instance().AddMessage("No charges left!");
  }
  else
  {
    switch (wand->Data.SpellHeld.SpellType_)
    {
      // TODO: finish wands effects and attack
      // (e.g. wand of heal others etc.)

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

bool MainState::ProcessMoneyPickup(std::pair<int, GameObject*>& pair)
{
  ItemComponent* ic = pair.second->GetComponent<ItemComponent>();
  if (ic->Data.ItemType_ == ItemType::COINS)
  {
    auto message = Util::StringFormat("Picked up: %i %s", ic->Data.Amount, ic->OwnerGameObject->ObjectName.data());
    Printer::Instance().AddMessage(message);

    _playerRef->Money += ic->Data.Amount;
    auto it = Map::Instance().CurrentLevel->GameObjects.begin();
    Map::Instance().CurrentLevel->GameObjects.erase(it + pair.first);
    return true;
  }

  return false;
}

void MainState::ProcessItemPickup(std::pair<int, GameObject*>& pair)
{
  ItemComponent* ic = pair.second->GetComponent<ItemComponent>();

  auto go = Map::Instance().CurrentLevel->GameObjects[pair.first].release();

  _playerRef->Inventory.Add(go);

  std::string objName = ic->Data.IsIdentified ? go->ObjectName : ic->Data.UnidentifiedName;

  std::string message;
  if (ic->Data.IsStackable)
  {
    message = Util::StringFormat("Picked up: %i %s", ic->Data.Amount, objName.data());
  }
  else
  {
    message = Util::StringFormat("Picked up: %s", objName.data());
  }

  Printer::Instance().AddMessage(message);

  auto it = Map::Instance().CurrentLevel->GameObjects.begin();
  Map::Instance().CurrentLevel->GameObjects.erase(it + pair.first);
}

void MainState::DisplayStartHint()
{
  int th = Printer::TerminalHeight;

  Printer::Instance().PrintFB(0,
                               th - 4,
                               '<',
                               GlobalConstants::WhiteColor,
                               GlobalConstants::DoorHighlightColor);

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

  Printer::Instance().PrintFB(1,
                               th - 4,
                               dir,
                               Printer::kAlignLeft,
                               GlobalConstants::WhiteColor);
}

void MainState::DisplayExitHint()
{
  int th = Printer::TerminalHeight;

  Printer::Instance().PrintFB(0,
                               th - 3,
                               '>',
                               GlobalConstants::WhiteColor,
                               GlobalConstants::DoorHighlightColor);

  auto curLvl = Map::Instance().CurrentLevel;
  if (curLvl->ExitFound)
  {
    int dx = curLvl->LevelExit.X - _playerRef->PosX;
    int dy = curLvl->LevelExit.Y - _playerRef->PosY;

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

    Printer::Instance().PrintFB(1,
                                 th - 3,
                                 dir,
                                 Printer::kAlignLeft,
                                 GlobalConstants::WhiteColor);
  }
  else
  {
    Printer::Instance().PrintFB(1,
                                 th - 3,
                                 "??",
                                 Printer::kAlignLeft,
                                 GlobalConstants::WhiteColor);
  }
}

void MainState::DisplayStatusIcons()
{
  int th = Printer::TerminalHeight;

  int startPos = 4;

  // Hungry

  if (_playerRef->IsStarving)
  {
    Printer::Instance().PrintFB(startPos,
                                 th - 3,
                                 '%',
                                 GlobalConstants::WhiteColor,
                                 GlobalConstants::RedColor);
  }
  else
  {
    int hungerMax = _playerRef->Attrs.HungerRate.Get();
    int part = hungerMax - hungerMax * 0.25;
    if (_playerRef->Attrs.Hunger >= part)
    {
      Printer::Instance().PrintFB(startPos, th - 3, '%', GlobalConstants::WhiteColor, "#999900");
    }
  }

  // Weapon condition

  ItemComponent* weapon = _playerRef->EquipmentByCategory[EquipmentCategory::WEAPON][0];
  if (weapon != nullptr &&
     (weapon->Data.ItemType_ == ItemType::WEAPON
   || weapon->Data.ItemType_ == ItemType::RANGED_WEAPON))
  {
    int maxDur = weapon->Data.Durability.Max().Get();
    int warning = maxDur * 0.3f;

    if (weapon->Data.Durability.Min().Get() <= warning)
    {
      Printer::Instance().PrintFB(startPos + 2, th - 3, ')', "#FFFF00");
    }
  }

  // Armor condition

  ItemComponent* armor = _playerRef->EquipmentByCategory[EquipmentCategory::TORSO][0];
  if (armor != nullptr && armor->Data.ItemType_ == ItemType::ARMOR)
  {
    int maxDur = armor->Data.Durability.Max().Get();
    int warning = maxDur * 0.3f;

    if (armor->Data.Durability.Min().Get() <= warning)
    {
      Printer::Instance().PrintFB(startPos + 4, th - 3, '[', "#FFFF00");
    }
  }

  // Ammo condition

  ItemComponent* arrows = _playerRef->EquipmentByCategory[EquipmentCategory::SHIELD][0];
  if (arrows != nullptr && arrows->Data.ItemType_ == ItemType::ARROWS)
  {
    int amount = arrows->Data.Amount;
    if (amount <= 3)
    {
      Printer::Instance().PrintFB(startPos + 6, th - 3, '^', "#FFFF00");
    }
  }

  // Active effects

  std::string lastDisplayedEffect;

  int offsetX = startPos;
  for (auto& kvp : _playerRef->Effects())
  {
    for (const ItemBonusStruct& item : kvp.second)
    {
      auto shortName = GlobalConstants::BonusDisplayNameByType.at(item.Type);
      if (shortName == lastDisplayedEffect)
      {
        continue;
      }

      std::string color = (item.Duration <= GlobalConstants::TurnReadyValue && item.Duration != -1) ? GlobalConstants::GroundColor : GlobalConstants::WhiteColor;

      Printer::Instance().PrintFB(offsetX,
                                   th - 4,
                                   shortName,
                                   Printer::kAlignLeft,
                                   color);
      offsetX += 4;

      lastDisplayedEffect = shortName;
    }
  }
}

void MainState::PrintNoAttackInTown()
{
  std::vector<std::string> variants = { "Not here", "Not in town" };
  int index = RNG::Instance().RandomRange(0, 2);
  Printer::Instance().AddMessage(variants[index]);
}

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
