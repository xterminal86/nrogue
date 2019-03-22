#include "mainstate.h"

#include "application.h"
#include "map.h"
#include "printer.h"
#include "rng.h"
#include "item-component.h"
#include "stairs-component.h"
#include "map-level-base.h"
#include "target-state.h"

void MainState::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

void MainState::HandleInput()
{
  _keyPressed = GetKeyDown();

  if (_playerRef->Attrs.ActionMeter >= GlobalConstants::TurnReadyValue)
  {
    switch (_keyPressed)
    {
      case NUMPAD_7:
        if (_playerRef->TryToAttack(-1, -1))
        {
          _playerRef->FinishTurn();
        }
        else if (_playerRef->Move(-1, -1))
        {
          Map::Instance().CurrentLevel->MapOffsetY++;
          Map::Instance().CurrentLevel->MapOffsetX++;

          _playerRef->FinishTurn();

          Printer::Instance().ShowLastMessage = false;

          _playerRef->ProcessHunger();
        }
        break;

      case NUMPAD_8:
        if (_playerRef->TryToAttack(0, -1))
        {
          _playerRef->FinishTurn();
        }
        else if (_playerRef->Move(0, -1))
        {
          Map::Instance().CurrentLevel->MapOffsetY++;

          _playerRef->FinishTurn();

          Printer::Instance().ShowLastMessage = false;

          _playerRef->ProcessHunger();
        }
        break;

      case NUMPAD_9:
        if (_playerRef->TryToAttack(1, -1))
        {
          _playerRef->FinishTurn();
        }
        else if (_playerRef->Move(1, -1))
        {
          Map::Instance().CurrentLevel->MapOffsetY++;
          Map::Instance().CurrentLevel->MapOffsetX--;

          _playerRef->FinishTurn();

          Printer::Instance().ShowLastMessage = false;

          _playerRef->ProcessHunger();
        }
        break;

      case NUMPAD_4:
        if (_playerRef->TryToAttack(-1, 0))
        {
          _playerRef->FinishTurn();
        }
        else if (_playerRef->Move(-1, 0))
        {
          Map::Instance().CurrentLevel->MapOffsetX++;

          _playerRef->FinishTurn();

          Printer::Instance().ShowLastMessage = false;

          _playerRef->ProcessHunger();
        }
        break;

      case NUMPAD_2:
        if (_playerRef->TryToAttack(0, 1))
        {
          _playerRef->FinishTurn();
        }
        else if (_playerRef->Move(0, 1))
        {
          Map::Instance().CurrentLevel->MapOffsetY--;

          _playerRef->FinishTurn();

          Printer::Instance().ShowLastMessage = false;

          _playerRef->ProcessHunger();
        }
        break;

      case NUMPAD_6:
        if (_playerRef->TryToAttack(1, 0))
        {
          _playerRef->FinishTurn();
        }
        else if (_playerRef->Move(1, 0))
        {
          Map::Instance().CurrentLevel->MapOffsetX--;

          _playerRef->FinishTurn();

          Printer::Instance().ShowLastMessage = false;

          _playerRef->ProcessHunger();
        }
        break;

      case NUMPAD_1:
        if (_playerRef->TryToAttack(-1, 1))
        {
          _playerRef->FinishTurn();
        }
        else if (_playerRef->Move(-1, 1))
        {
          Map::Instance().CurrentLevel->MapOffsetY--;
          Map::Instance().CurrentLevel->MapOffsetX++;

          _playerRef->FinishTurn();

          Printer::Instance().ShowLastMessage = false;

          _playerRef->ProcessHunger();
        }
        break;

      case NUMPAD_3:
        if (_playerRef->TryToAttack(1, 1))
        {
          _playerRef->FinishTurn();
        }
        else if (_playerRef->Move(1, 1))
        {
          Map::Instance().CurrentLevel->MapOffsetY--;
          Map::Instance().CurrentLevel->MapOffsetX--;

          _playerRef->FinishTurn();

          Printer::Instance().ShowLastMessage = false;

          _playerRef->ProcessHunger();
        }
        break;

      case NUMPAD_5:
        Printer::Instance().AddMessage("You waited...");
        _playerRef->FinishTurn();
        _playerRef->ProcessHunger();
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
        auto str = Util::StringFormat("You have %i gold coins", _playerRef->Money);
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

      case 'g':
        TryToPickupItem();
        break;

      case '@':
        Application::Instance().ChangeState(GameStates::INFO_STATE);
        break;

      case '?':
        DisplayHelp();
        break;

      case 'Q':
        Application::Instance().ChangeState(GameStates::EXITING_STATE);
        break;

      case 'f':
        ProcessRangedWeapon();
        break;

      case '>':
        CheckStairs('>');
        break;

      case '<':
        CheckStairs('<');
        break;

      // ***** FIXME: for debug, remove afterwards

      case 'L':
        _playerRef->LevelUp();
        break;

      case 'p':
        //Map::Instance().PrintMapArrayRevealedStatus();
        Map::Instance().PrintMapLayout();
        break;

      case 'T':
      {
        int exitX = Map::Instance().CurrentLevel->LevelExit.X;
        int exitY = Map::Instance().CurrentLevel->LevelExit.Y;

        _playerRef->MoveTo(exitX, exitY);

        Map::Instance().CurrentLevel->AdjustCamera();
        Update(true);
      }
      break;

      // *****

      default:
        break;
    }
  }

  // Update all game objects if player is not ready to act
  if (_playerRef->Attrs.ActionMeter < GlobalConstants::TurnReadyValue)
  {
    Map::Instance().UpdateGameObjects();
    _playerRef->WaitForTurn();
    //Update(true);
  }
}

void MainState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    _playerRef->CheckVisibility();

    Map::Instance().Draw(_playerRef->PosX, _playerRef->PosY);

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

    Printer::Instance().PrintFB(Printer::Instance().TerminalWidth - 1, 0, Map::Instance().CurrentLevel->LevelName, Printer::kAlignRight, "#FFFFFF");

    _debugInfo = Util::StringFormat("World seed: %lu", RNG::Instance().Seed);
    Printer::Instance().PrintFB(0, 0, _debugInfo, Printer::kAlignLeft, "#FFFFFF");

    // FIXME: print some debug info on the screen

    // PrintDebugInfo();

    // *****

    Printer::Instance().Render();
  }
}

void MainState::DisplayGameLog()
{
  int x = Printer::Instance().TerminalWidth - 1;
  int y = Printer::Instance().TerminalHeight - 1;

  int count = 0;
  auto msgs = Printer::Instance().GetLastMessages();
  for (auto& m : msgs)
  {
    Printer::Instance().PrintFB(x, y - count, m, Printer::kAlignRight, "#FFFFFF");
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
      return;
    }

    if (_playerRef->Inventory.IsFull())
    {
      Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Epic Fail", { "Inventory is full!" }, GlobalConstants::MessageBoxRedBorderColor);
      return;
    }

    ProcessItemPickup(res);
  }
  else
  {
    Printer::Instance().AddMessage("There's nothing here");
  }
}

void MainState::DrawHPMP()
{
  int curHp = _playerRef->Attrs.HP.CurrentValue;
  int maxHp = _playerRef->Attrs.HP.OriginalValue;
  int curMp = _playerRef->Attrs.MP.CurrentValue;
  int maxMp = _playerRef->Attrs.MP.OriginalValue;

  int th = Printer::Instance().TerminalHeight;

  UpdateBar(0, th - 2, _playerRef->Attrs.HP);

  auto str = Util::StringFormat("%i/%i", curHp, maxHp);
  Printer::Instance().PrintFB(GlobalConstants::HPMPBarLength / 2, th - 2, str, Printer::kAlignCenter, "#FFFFFF", "#880000");

  UpdateBar(0, th - 1, _playerRef->Attrs.MP);

  str = Util::StringFormat("%i/%i", curMp, maxMp);
  Printer::Instance().PrintFB(GlobalConstants::HPMPBarLength / 2, th - 1, str, Printer::kAlignCenter, "#FFFFFF", "#000088");
}

void MainState::UpdateBar(int x, int y, Attribute attr)
{
  float ratio = ((float)attr.CurrentValue / (float)attr.OriginalValue);
  int len = ratio * GlobalConstants::HPMPBarLength;

  std::string bar = "[";
  for (int i = 0; i < GlobalConstants::HPMPBarLength; i++)
  {
    bar += (i < len) ? "=" : " ";
  }

  bar += "]";

  Printer::Instance().PrintFB(x, y, bar, Printer::kAlignLeft, "#FFFFFF");
}

void MainState::DisplayHelp()
{
  Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Help", _helpText);
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

  Printer::Instance().PrintFB(0, 1, _debugInfo, Printer::kAlignLeft, "#FFFFFF");

  _debugInfo = Util::StringFormat("Key: %i Actors: %i Respawn: %i",
                                  _keyPressed,
                                  Map::Instance().CurrentLevel->ActorGameObjects.size(),
                                  Map::Instance().CurrentLevel->RespawnCounter());

  Printer::Instance().PrintFB(0, 2, _debugInfo, Printer::kAlignLeft, "#FFFFFF");

  _debugInfo = Util::StringFormat("Level Start: [%i;%i]", Map::Instance().CurrentLevel->LevelStart.X, Map::Instance().CurrentLevel->LevelStart.Y);
  Printer::Instance().PrintFB(0, 3, _debugInfo, Printer::kAlignLeft, "#FFFFFF");

  _debugInfo = Util::StringFormat("Level Exit: [%i;%i]", Map::Instance().CurrentLevel->LevelExit.X, Map::Instance().CurrentLevel->LevelExit.Y);
  Printer::Instance().PrintFB(0, 4, _debugInfo, Printer::kAlignLeft, "#FFFFFF");

  _debugInfo = Util::StringFormat("Colors Used: %i", Printer::Instance().ColorsUsed());
  Printer::Instance().PrintFB(0, 5, _debugInfo, Printer::kAlignLeft, "#FFFFFF");
}

void MainState::ProcessRangedWeapon()
{
  if (Map::Instance().CurrentLevel->Peaceful)
  {
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
    bool isBow = (weapon->Data.RangedWeaponType_ == RangedWeaponType::LIGHT_BOW
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
  if (wand->Data.Amount == 0)
  {
    Printer::Instance().AddMessage("No charges left!");
  }
  else
  {
    switch (wand->Data.SpellHeld)
    {
      // TODO:
      case SpellType::LIGHT:
        break;

      case SpellType::FIREBALL:
      case SpellType::STRIKE:
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

  _playerRef->Inventory.AddToInventory(go);

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
  int th = Printer::Instance().TerminalHeight;

  Printer::Instance().PrintFB(0, th - 4, '<', "#FFFFFF", GlobalConstants::DoorHighlightColor);

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

  Printer::Instance().PrintFB(1, th - 4, dir, Printer::kAlignLeft, "#FFFFFF");
}

void MainState::DisplayExitHint()
{
  int th = Printer::Instance().TerminalHeight;

  Printer::Instance().PrintFB(0, th - 3, '>', "#FFFFFF", GlobalConstants::DoorHighlightColor);

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

    Printer::Instance().PrintFB(1, th - 3, dir, Printer::kAlignLeft, "#FFFFFF");
  }
  else
  {
    Printer::Instance().PrintFB(1, th - 3, "??", Printer::kAlignLeft, "#FFFFFF");
  }
}

void MainState::DisplayStatusIcons()
{
  int th = Printer::Instance().TerminalHeight;

  int startPos = 4;

  // Hungry

  if (_playerRef->IsStarving)
  {
    Printer::Instance().PrintFB(startPos, th - 3, '%', "#FF0000");
  }
  else
  {
    int hungerMax = _playerRef->Attrs.HungerRate.CurrentValue;
    int part = hungerMax - hungerMax * 0.25;
    if (_playerRef->Attrs.Hunger >= part)
    {
      Printer::Instance().PrintFB(startPos, th - 3, '%', "#FFFF00");
    }
  }

  // Weapon condition

  ItemComponent* weapon = _playerRef->EquipmentByCategory[EquipmentCategory::WEAPON][0];
  if (weapon != nullptr && weapon->Data.ItemType_ == ItemType::WEAPON)
  {
    int maxDur = weapon->Data.Durability.OriginalValue;
    int warning = maxDur * 0.3f;

    if (weapon->Data.Durability.CurrentValue <= warning)
    {
      Printer::Instance().PrintFB(startPos + 2, th - 3, ')', "#FFFF00");
    }
  }

  // Armor condition

  ItemComponent* armor = _playerRef->EquipmentByCategory[EquipmentCategory::TORSO][0];
  if (armor != nullptr && armor->Data.ItemType_ == ItemType::ARMOR)
  {
    int maxDur = armor->Data.Durability.OriginalValue;
    int warning = maxDur * 0.3f;

    if (armor->Data.Durability.CurrentValue <= warning)
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
}

void MainState::PrintNoAttackInTown()
{
  std::vector<std::string> variants = { "Not here", "Not in town" };
  int index = RNG::Instance().RandomRange(0, 2);
  Printer::Instance().AddMessage(variants[index]);
}
