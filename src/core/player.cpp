#include "player.h"

#include "map.h"
#include "printer.h"
#include "util.h"
#include "door-component.h"
#include "game-objects-factory.h"
#include "ai-monster-basic.h"
#include "ai-component.h"
#include "application.h"
#include "item-component.h"

void Player::Init()
{
  PosX = Map::Instance().CurrentLevel->LevelStart.X;
  PosY = Map::Instance().CurrentLevel->LevelStart.Y;
  Image = '@';
  FgColor = GlobalConstants::PlayerColor;
  Attrs.ActionMeter = 100;

  Inventory.MaxCapacity = GlobalConstants::InventoryMaxSize;

  SetAttributes();
  SetDefaultItems();
  SetDefaultEquipment();

  _previousCell = Map::Instance().CurrentLevel->MapArray[PosX][PosY].get();
  _currentCell = Map::Instance().CurrentLevel->MapArray[PosX][PosY].get();
  _currentCell->Occupied = true;

  // FIXME: remove afterwards

#if 0
  for (int i = 0; i < 40; i++)
  {
    auto go = GameObjectsFactory::Instance().CreateRandomPotion();
    //auto go = GameObjectsFactory::Instance().CreateExpPotion();
    //auto go = GameObjectsFactory::Instance().CreateHealingPotion(ItemPrefix::CURSED);
    if (go != nullptr)
    {
      auto c = go->GetComponent<ItemComponent>();
      //((ItemComponent*)c)->Data.IsIdentified = true;

      /*
      int res = RNG::Instance().RandomRange(0, 2);
      if (res == 0) ((ItemComponent*)c)->Data.IsPrefixDiscovered = true;
      */

      go->PosX = 2 + i;
      go->PosY = 1;

      Map::Instance().InsertGameObject(go);
    }

    //auto msg = Util::StringFormat("Created 0x%X", go);
    //Logger::Instance().Print(msg);
  }

  /*
  for (int i = 0; i < 20; i++)
  {
    auto go = new GameObject(2, 2 + i, 'O', "#FFFFFF");
    go->ObjectName = "Rock";

    auto ic = go->AddComponent<ItemComponent>();    
    ((ItemComponent*)ic)->Data.Description = { "This is a placeholder test object" };

    Map::Instance().InsertGameObject(go);

    //auto msg = Util::StringFormat("Created 0x%X", go);
    //Logger::Instance().Print(msg);
  }

  for (int i = 0; i < 3; i++)
  {
    auto go = new GameObject(3 + i, 1, 'o', "#FFFFFF");

    auto name = Util::StringFormat("Ring %i", i);
    go->ObjectName = name;

    auto ic = go->AddComponent<ItemComponent>();
    ((ItemComponent*)ic)->Data.EqCategory = EquipmentCategory::RING;
    ((ItemComponent*)ic)->Data.Description = { "This is a placeholder equippable object" };

    Map::Instance().InsertGameObject(go);
  }

  auto go = new GameObject(10, 1, 'O', "#FFFFFF");
  go->ObjectName = "Medallion";

  auto ic = go->AddComponent<ItemComponent>();
  ((ItemComponent*)ic)->Data.EqCategory = EquipmentCategory::NECK;
  ((ItemComponent*)ic)->Data.Description = { "This is a placeholder equippable object" };

  Map::Instance().InsertGameObject(go);
  */
#endif

}

void Player::Draw()
{
  bool cond = (BgColor.length() == 0 || BgColor == "#000000");
  GameObject::Draw(FgColor, cond ? Map::Instance().CurrentLevel->MapArray[PosX][PosY]->BgColor : BgColor);  
}

bool Player::TryToAttack(int dx, int dy)
{
  auto go = Map::Instance().GetActorAtPosition(PosX + dx, PosY + dy);
  if (go != nullptr)
  {
    Component* c = go->GetComponent<AIComponent>();
    if (c != nullptr)
    {
      AIComponent* aic = static_cast<AIComponent*>(c);
      if (aic->CurrentModel->IsAgressive)
      {
        Attack(go);
        return true;
      }
    }
  }

  return false;
}

bool Player::Move(int dx, int dy)
{
  auto cell = Map::Instance().CurrentLevel->MapArray[PosX + dx][PosY + dy].get();

  if (!cell->Occupied && !cell->Blocking)
  {
    _previousCell = Map::Instance().CurrentLevel->MapArray[PosX][PosY].get();
    _previousCell->Occupied = false;

    PosX += dx;
    PosY += dy;

    _currentCell = Map::Instance().CurrentLevel->MapArray[PosX][PosY].get();
    _currentCell->Occupied = true;

    return true;
  }
  else
  {    
    // Search for components only if there are any

    if (cell->ComponentsSize() != 0)
    {
      auto c = cell->GetComponent<DoorComponent>();

      // Automatically interact with door if it's closed
      if (c != nullptr && cell->Blocking && cell->Interact())
      {
        FinishTurn();
      }
    }
  }

  return false;
}

void Player::CheckVisibility()
{  
  int tw = Printer::Instance().TerminalWidth;
  int th = Printer::Instance().TerminalHeight;

  // FIXME: think
  //
  // Should we compensate for different terminal sizes,
  // so we get more circle-like visible area around player?
  /*

  int vrx = VisibilityRadius;
  int vry = VisibilityRadius;

  if (tw > th)
  {
    vry /= 2;
  }
  else if (tw < th)
  {
    vrx /= 2;
  }
  */

  // Update map around player

  auto& map = Map::Instance().CurrentLevel->MapArray;

  // FIXME: think
  int radius = (map[PosX][PosY]->ObjectName == "Tree") ? VisibilityRadius / 4 : VisibilityRadius;

  auto mapCells = Util::GetRectAroundPoint(PosX, PosY, tw / 2, th / 2, Map::Instance().CurrentLevel->MapSize);
  for (auto& cell : mapCells)
  {
    map[cell.X][cell.Y]->Visible = false;
  }

  // Update visibility around player

  for (auto& cell : mapCells)
  {
    float d = Util::LinearDistance(PosX, PosY, cell.X, cell.Y);

    if (d > (float)radius)
    {
      continue;
    }

    // Bresenham FoV

    auto line = Util::BresenhamLine(PosX, PosY, cell.X, cell.Y);
    for (auto& point : line)
    {
      if (point.X == PosX && point.Y == PosY)
      {
        continue;
      }

      if (map[point.X][point.Y]->BlocksSight)
      {
        DiscoverCell(point.X, point.Y);
        break;
      }

      DiscoverCell(point.X, point.Y);
    }
  }    
}

void Player::DiscoverCell(int x, int y)
{
  auto& map = Map::Instance().CurrentLevel->MapArray;
  auto curLvl = Map::Instance().CurrentLevel;

  if (x == curLvl->LevelExit.X
   && y == curLvl->LevelExit.Y
   && !curLvl->ExitFound)
  {
    curLvl->ExitFound = true;
  }

  map[x][y]->Visible = true;

  if (!map[x][y]->Revealed)
  {
    map[x][y]->Revealed = true;
  }
}

void Player::SetAttributes()
{
  switch (GetClass())
  {
    case PlayerClass::SOLDIER:
      SetSoldierAttrs();
      break;

    case PlayerClass::THIEF:
      SetThiefAttrs();
      break;

    case PlayerClass::ARCANIST:
      SetArcanistAttrs();
      break;

    default:
      // TODO: custom class defaults to soldier for now
      SetSoldierAttrs();
      break;
  }  
}

void Player::SetSoldierAttrs()
{
  Attrs.Str.Talents = 3;
  Attrs.Def.Talents = 2;
  Attrs.Skl.Talents = 1;
  Attrs.HP.Talents = 2;

  Attrs.Str.Set(3);
  Attrs.Def.Set(2);
  Attrs.Skl.Set(1);

  Attrs.HP.Set(30);

  Attrs.HungerRate.Set(1000);
  Attrs.HungerSpeed.Set(1);

  HealthRegenTurns = 30;
}

void Player::SetThiefAttrs()
{
  Attrs.Spd.Talents = 3;
  Attrs.Skl.Talents = 3;
  Attrs.Def.Talents = 1;
  Attrs.HP.Talents = 1;

  Attrs.Def.Set(1);
  Attrs.Skl.Set(2);
  Attrs.Spd.Set(3);

  Attrs.HP.Set(20);

  Attrs.HungerRate.Set(1500);
  Attrs.HungerSpeed.Set(1);  

  HealthRegenTurns = 50;
}

void Player::SetArcanistAttrs()
{
  Attrs.Mag.Talents = 3;
  Attrs.Res.Talents = 3;
  Attrs.Spd.Talents = 1;
  Attrs.MP.Talents = 3;

  Attrs.Mag.Set(2);
  Attrs.Res.Set(2);
  Attrs.Spd.Set(1);

  Attrs.HP.Set(10);
  Attrs.MP.Set(30);

  Attrs.HungerRate.Set(2000);
  Attrs.HungerSpeed.Set(1);  

  HealthRegenTurns = 80;
}

void Player::SetDefaultEquipment()
{
  // Initialize map

  EquipmentByCategory[EquipmentCategory::HEAD] = { nullptr };
  EquipmentByCategory[EquipmentCategory::NECK] = { nullptr };
  EquipmentByCategory[EquipmentCategory::TORSO] = { nullptr };
  EquipmentByCategory[EquipmentCategory::LEGS] = { nullptr };
  EquipmentByCategory[EquipmentCategory::BOOTS] = { nullptr };
  EquipmentByCategory[EquipmentCategory::WEAPON] = { nullptr };
  EquipmentByCategory[EquipmentCategory::RING] = { nullptr, nullptr };

  // TODO: assign default equipment according to selected class

  switch (GetClass())
  {
    case PlayerClass::THIEF:
    {
      auto go = GameObjectsFactory::Instance().CreateWeapon(WeaponType::DAGGER, true);
      Inventory.AddToInventory(go);
    }
    break;

    case PlayerClass::SOLDIER:
    {
      auto go = GameObjectsFactory::Instance().CreateWeapon(WeaponType::SHORT_SWORD, true);
      Inventory.AddToInventory(go);
    }
    break;

    case PlayerClass::ARCANIST:
    {
      auto go = GameObjectsFactory::Instance().CreateWeapon(WeaponType::STAFF, true);
      Inventory.AddToInventory(go);
    }
    break;
  }
}

void Player::Attack(GameObject* go)
{
  if (go->Attrs.Indestructible)
  {
    Application::Instance().DisplayAttack(go, GlobalConstants::DisplayAttackDelayMs, "#FFFFFF");

    auto str = Util::StringFormat("Your attack bounces off %s!", go->ObjectName.data());
    Printer::Instance().AddMessage(str);

    Application::Instance().DrawCurrentState();
    Application::Instance().DisplayAttack(go, GlobalConstants::DisplayAttackDelayMs);

    if (EquipmentByCategory[EquipmentCategory::WEAPON][0] != nullptr
     && DoesWeaponLosesDurability())
    {
      Printer::Instance().AddMessage("Your weapon lost durability");
    }
  }
  else
  {    
    int defaultHitChance = 50;
    int hitChance = defaultHitChance;

    int d = Attrs.Skl.Get() - go->Attrs.Skl.CurrentValue;
    if (d > 0)
    {
      hitChance += (d * 5);
    }
    else
    {
      hitChance -= (d * 5);
    }

    hitChance = Util::Clamp(hitChance, GlobalConstants::MinHitChance, GlobalConstants::MaxHitChance);

    auto logMsg = Util::StringFormat("Player (SKL %i, LVL %i) attacks %s (SKL: %i, LVL %i): chance = %i",
                                     Attrs.Skl.CurrentValue,
                                     Attrs.Lvl.CurrentValue,
                                     go->ObjectName.data(),
                                     go->Attrs.Skl.CurrentValue,
                                     go->Attrs.Lvl.CurrentValue,
                                     hitChance);
    Logger::Instance().Print(logMsg);

    if (Util::Rolld100(hitChance))
    {
      Application::Instance().DisplayAttack(go, GlobalConstants::DisplayAttackDelayMs, "#FF0000");

      int weaponDamage = 0;

      bool durabilityLost = false;

      if (EquipmentByCategory[EquipmentCategory::WEAPON][0] != nullptr)
      {
        auto wd = EquipmentByCategory[EquipmentCategory::WEAPON][0]->Data.Damage;
        int numRolls = wd.CurrentValue;
        int diceSides = wd.OriginalValue;

        for (int i = 0; i < numRolls; i++)
        {
          int res = RNG::Instance().RandomRange(1, diceSides + 1);
          weaponDamage += res;
        }                

        durabilityLost = DoesWeaponLosesDurability();
      }

      int totalDmg = weaponDamage;
      totalDmg += Attrs.Str.Get() - go->Attrs.Def.CurrentValue;
      if (totalDmg <= 0)
      {
        totalDmg = 1;
      }

      go->ReceiveDamage(this, totalDmg);

      if (go->IsDestroyed)
      {
        ProcessKill(go);
      }

      Application::Instance().DrawCurrentState();
      Application::Instance().DisplayAttack(go, GlobalConstants::DisplayAttackDelayMs);

      if (durabilityLost)
      {
        Printer::Instance().AddMessage("Your weapon lost durability");
      }
    }
    else
    {
      Application::Instance().DisplayAttack(go, GlobalConstants::DisplayAttackDelayMs, "#FFFFFF");

      Printer::Instance().AddMessage("You missed");

      Application::Instance().DrawCurrentState();
      Application::Instance().DisplayAttack(go, GlobalConstants::DisplayAttackDelayMs);
    }
  }

  Attrs.Hunger += Attrs.HungerSpeed.Get() * 2;

  FinishTurn();
}

void Player::ReceiveDamage(GameObject* from, int amount)
{  
  auto str = Util::StringFormat("You were hit for %i damage", amount);
  Printer::Instance().AddMessage(str);

  Attrs.HP.CurrentValue -= amount;  
}

void Player::AwardExperience(int amount)
{
  int amnt = amount * (Attrs.Exp.Talents + 1);

  Attrs.Exp.CurrentValue += amnt;

  auto msg = Util::StringFormat("Received %i EXP", amnt);
  Printer::Instance().AddMessage(msg);

  if (Attrs.Exp.CurrentValue >= 100)
  {
    Attrs.Exp.Set(0);
    LevelUp();
  }
  else if (Attrs.Exp.CurrentValue < 0)
  {
    Attrs.Exp.Set(0);
    LevelDown();
  }
}

void Player::LevelUp()
{
  for (auto& kvp : _statRaisesMap)
  {
    kvp.second = 0;
  }

  // Try to raise main stats

  for (auto& i : _mainAttributes)
  {
    auto kvp = i.second;

    auto log = Util::StringFormat("Raising %s", kvp.first.data());
    Logger::Instance().Print(log);

    if (CanRaiseAttribute(kvp.second))
    {
      _statRaisesMap[kvp.first] = 1;

      kvp.second.OriginalValue++;
      kvp.second.CurrentValue = kvp.second.OriginalValue;
    }    
  }

  // HP and MP

  int minRndHp = (Attrs.HP.Talents + 1);
  int maxRndHp = 2 * (Attrs.HP.Talents + 1);

  int hpToAdd = RNG::Instance().RandomRange(minRndHp, maxRndHp);
  Attrs.HP.OriginalValue += hpToAdd;

  _statRaisesMap["HP"] = hpToAdd;

  int minRndMp = Attrs.Mag.OriginalValue;
  int maxRndMp = Attrs.Mag.OriginalValue * (Attrs.MP.Talents + 1);

  int mpToAdd = RNG::Instance().RandomRange(minRndMp, maxRndMp);
  Attrs.MP.OriginalValue += mpToAdd;

  _statRaisesMap["MP"] = mpToAdd;

  Attrs.Lvl.OriginalValue++;
  Attrs.Lvl.CurrentValue = Attrs.Lvl.OriginalValue;

  auto res = GetPrettyLevelUpText();

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "Level Up!", res, "#888800", "#000044");

  Printer::Instance().AddMessage("You have gained a level!");

  /*
   * TODO: Raise skills etc.
   *
  auto class_ = _classesMap[SelectedClass];

  switch (class_)
  {
    case PlayerClass::SOLDIER:
      break;

    case PlayerClass::THIEF:
      break;

    case PlayerClass::ARCANIST:
      break;

    case PlayerClass::CUSTOM:
      break;
  }
  */  
}

void Player::LevelDown()
{
  for (auto& kvp : _statRaisesMap)
  {
    kvp.second = 0;
  }

  // Try to raise main stats

  for (auto& i : _mainAttributes)
  {
    auto kvp = i.second;

    if (CanRaiseAttribute(kvp.second))
    {
      _statRaisesMap[kvp.first] = -1;

      kvp.second.OriginalValue--;
      if (kvp.second.OriginalValue < 0)
      {
        kvp.second.OriginalValue = 0;
      }

      kvp.second.CurrentValue = kvp.second.OriginalValue;
    }
  }

  // HP and MP

  int minRndHp = (Attrs.HP.Talents + 1);
  int maxRndHp = 2 * (Attrs.HP.Talents + 1);

  int hpToAdd = RNG::Instance().RandomRange(minRndHp, maxRndHp);
  Attrs.HP.OriginalValue -= hpToAdd;

  if (Attrs.HP.OriginalValue <= 0)
  {
    Attrs.HP.OriginalValue = 1;
  }

  _statRaisesMap["HP"] = -hpToAdd;

  int minRndMp = Attrs.Mag.OriginalValue;
  int maxRndMp = Attrs.Mag.OriginalValue * (Attrs.MP.Talents + 1);

  int mpToAdd = RNG::Instance().RandomRange(minRndMp, maxRndMp);
  Attrs.MP.OriginalValue -= mpToAdd;

  if (Attrs.MP.OriginalValue < 0)
  {
    Attrs.MP.OriginalValue = 0;
  }

  _statRaisesMap["MP"] = -mpToAdd;

  Attrs.Lvl.OriginalValue--;
  if (Attrs.Lvl.OriginalValue <= 1)
  {
    Attrs.Lvl.OriginalValue = 1;
  }

  Attrs.Lvl.CurrentValue = Attrs.Lvl.OriginalValue;

  auto res = GetPrettyLevelUpText();

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "Level DOWN!", res, "#FF0000", "#000044");

  Printer::Instance().AddMessage("You have LOST a level!");

  /*
   * Raise skills etc.
   *
  auto class_ = _classesMap[SelectedClass];

  switch (class_)
  {
    case PlayerClass::SOLDIER:
      break;

    case PlayerClass::THIEF:
      break;

    case PlayerClass::ARCANIST:
      break;

    case PlayerClass::CUSTOM:
      break;
  }
  */
}

bool Player::CanRaiseAttribute(Attribute& attr)
{
  int chance = GlobalConstants::AttributeMinimumRaiseChance;

  int iterations = attr.Talents;
  for (int i = 0; i < iterations; i++)
  {
    if (Util::Rolld100(chance))
    {
      return true;
    }

    chance += GlobalConstants::AttributeIncreasedRaiseStep;
  }

  return Util::Rolld100(chance);
}

void Player::ProcessKill(GameObject* monster)
{
  int defaultExp = monster->Attrs.HP.OriginalValue;
  int exp = 0;

  float ratio = (float)monster->Attrs.Lvl.CurrentValue / (float)Attrs.Lvl.CurrentValue;

  int amount = (float)defaultExp * ratio;

  exp += amount;

  exp = Util::Clamp(exp, 1, GlobalConstants::AwardedExpMax);

  GameObjectsFactory::Instance().GenerateLootIfPossible(monster->PosX, monster->PosY, monster->Type);

  AwardExperience(exp);
}

void Player::WaitForTurn()
{
  GameObject::WaitForTurn();

  // Redraw screen only when player is ready for action
  // (assuming Application::_currentState is MainState)
  if (Attrs.ActionMeter >= 100)
  {
    Application::Instance().DrawCurrentState();
  }
}

bool Player::IsAlive(GameObject* damager)
{
  if (Attrs.HP.CurrentValue <= 0)
  {
    Attrs.HP.CurrentValue = 0;

    Image = '%';
    FgColor = GlobalConstants::PlayerColor;
    BgColor = "#FF0000";
    IsDestroyed = true;

    if (damager != nullptr)
    {
      auto str = Util::StringFormat("%s was killed by a %s", Name.data(), damager->ObjectName.data());
      Printer::Instance().AddMessage(str);
    }

    Printer::Instance().AddMessage("You are dead. Not big surprise.");    

    return false;
  }

  return true;
}

std::vector<std::string> Player::GetPrettyLevelUpText()
{
  std::vector<std::string> levelUpResults;

  std::string mbStr;
  for (auto& i : _mainAttributes)
  {
    auto kvp = i.second;

    mbStr = Util::StringFormat("%s: %i", kvp.first.data(), kvp.second.OriginalValue);
    levelUpResults.push_back(mbStr);
  }

  levelUpResults.push_back("");

  mbStr = Util::StringFormat("HP:  %i", Attrs.HP.OriginalValue);
  levelUpResults.push_back(mbStr);

  mbStr = Util::StringFormat("MP:  %i", Attrs.MP.OriginalValue);
  levelUpResults.push_back(mbStr);

  // Try to make everything look pretty
  //
  // NOTE: probably lots of shitcode

  int maxLen = 0;
  for (auto& s : levelUpResults)
  {
    if (s.length() > maxLen)
    {
      maxLen = s.length();
    }
  }

  for (auto& s : levelUpResults)
  {
    if (s.length() < maxLen)
    {
      int d = maxLen - s.length();
      s.append(d, ' ');
    }
  }

  int index = 0;
  for (auto& i : _mainAttributes)
  {
    auto kvp = i.second;

    std::string addition;
    if (_statRaisesMap[kvp.first] >= 0)
    {
      addition = Util::StringFormat("  +%i", _statRaisesMap[kvp.first]);
    }
    else
    {
      addition = Util::StringFormat("  -%i", std::abs(_statRaisesMap[kvp.first]));
    }

    levelUpResults[index] += addition;
    index++;
  }

  // Take into account empty string between stats and hitpoints
  index++;

  std::string addition;
  if (_statRaisesMap["HP"] >= 0)
  {
    addition = Util::StringFormat("  +%i", _statRaisesMap["HP"]);
  }
  else
  {
    addition = Util::StringFormat("  -%i", std::abs(_statRaisesMap["HP"]));
  }

  levelUpResults[index] += addition;
  index++;

  if (_statRaisesMap["MP"] >= 0)
  {
    addition = Util::StringFormat("  +%i", _statRaisesMap["MP"]);
  }
  else
  {
    addition = Util::StringFormat("  -%i", std::abs(_statRaisesMap["MP"]));
  }

  levelUpResults[index] += addition;
  index++;

  return levelUpResults;
}

void Player::FinishTurn()
{
  Attrs.ActionMeter -= 100;

  if (Attrs.ActionMeter < 0)
  {
    Attrs.ActionMeter = 0;
  }

  if (IsStarving)
  {
    _starvingTimeout++;

    if (_starvingTimeout > 11)
    {
      _starvingTimeout = 0;
    }
  }
  else
  {
    _healthRegenTurnsCounter++;

    if (_healthRegenTurnsCounter >= HealthRegenTurns)
    {
      _healthRegenTurnsCounter = 0;

      if (Attrs.HP.CurrentValue < Attrs.HP.OriginalValue)
      {
        Attrs.Hunger += Attrs.HungerSpeed.Get();
        Attrs.HP.Add(1);
      }
    }
  }
}

void Player::ProcessHunger()
{
  Attrs.Hunger += Attrs.HungerSpeed.Get();

  int maxHunger = Attrs.HungerRate.CurrentValue;
  Attrs.Hunger = Util::Clamp(Attrs.Hunger, 0, maxHunger);

  if (Attrs.Hunger == maxHunger)
  {
    if (_starvingTimeout > 10)
    {
      Printer::Instance().AddMessage("You are starving!");
      ReceiveDamage(nullptr, 1);
    }

    IsStarving = true;
  }
  else
  {
    _starvingTimeout = 0;
    IsStarving = false;
  }
}

void Player::SetDefaultItems()
{
  switch (GetClass())
  {
    case PlayerClass::SOLDIER:
      SetSoldierDefaultItems();
      break;

    case PlayerClass::THIEF:
      SetThiefDefaultItems();
      break;

    case PlayerClass::ARCANIST:
      SetArcanistDefaultItems();
      break;
  }
}

void Player::SetSoldierDefaultItems()
{
  auto go = GameObjectsFactory::Instance().CreateHealingPotion();
  auto ic = go->GetComponent<ItemComponent>();
  ((ItemComponent*)ic)->Data.Amount = 3;

  Inventory.AddToInventory(go);

  go = GameObjectsFactory::Instance().CreateFood(0, 0, FoodType::IRON_RATIONS, ItemPrefix::UNCURSED);
  ic = go->GetComponent<ItemComponent>();
  ((ItemComponent*)ic)->Data.Amount = 1;
  ((ItemComponent*)ic)->Data.IsIdentified = true;

  Inventory.AddToInventory(go);
}

void Player::SetThiefDefaultItems()
{
  auto go = GameObjectsFactory::Instance().CreateHealingPotion();
  auto ic = go->GetComponent<ItemComponent>();
  Inventory.AddToInventory(go);

  go = GameObjectsFactory::Instance().CreateManaPotion();
  ic = go->GetComponent<ItemComponent>();
  Inventory.AddToInventory(go);

  go = GameObjectsFactory::Instance().CreateMoney(100);
  ic = go->GetComponent<ItemComponent>();
  Inventory.AddToInventory(go);  
}

void Player::SetArcanistDefaultItems()
{
  auto go = GameObjectsFactory::Instance().CreateManaPotion();
  auto c = go->GetComponent<ItemComponent>();
  ItemComponent* ic = static_cast<ItemComponent*>(c);
  ic->Data.Amount = 5;

  Inventory.AddToInventory(go);
}

bool Player::DoesWeaponLosesDurability()
{
  if (Util::Rolld100(10))
  {
    EquipmentByCategory[EquipmentCategory::WEAPON][0]->Data.Durability.Add(-1);

    if (EquipmentByCategory[EquipmentCategory::WEAPON][0]->Data.Durability.CurrentValue <= 0)
    {
      // NOTE: destroy broken item?
      auto objName = EquipmentByCategory[EquipmentCategory::WEAPON][0]->OwnerGameObject->ObjectName;
      auto str = Util::StringFormat("%s breaks!", objName.data());
      Printer::Instance().AddMessage(str);
    }

    //auto dbg = Util::StringFormat("Durability: %i / %i", EquipmentByCategory[EquipmentCategory::WEAPON][0]->Data.Durability.CurrentValue, EquipmentByCategory[EquipmentCategory::WEAPON][0]->Data.Durability.OriginalValue);
    //Logger::Instance().Print(dbg);

    return true;
  }

  return false;
}
