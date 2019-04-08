#include "player.h"

#include "map.h"
#include "printer.h"
#include "door-component.h"
#include "game-objects-factory.h"
#include "ai-monster-basic.h"
#include "ai-component.h"
#include "ai-npc.h"
#include "application.h"
#include "item-component.h"

void Player::Init()
{
  PosX = Map::Instance().CurrentLevel->LevelStart.X;
  PosY = Map::Instance().CurrentLevel->LevelStart.Y;
  Image = '@';
  FgColor = GlobalConstants::PlayerColor;
  Attrs.ActionMeter = GlobalConstants::TurnReadyValue;

  // FIXME: debug
  //Money = 1000;

  Inventory.MaxCapacity = GlobalConstants::InventoryMaxSize;

  SetAttributes();
  SetDefaultItems();
  SetDefaultEquipment();
  SetDefaultSkills();

  _previousCell = Map::Instance().CurrentLevel->MapArray[PosX][PosY].get();
  _currentCell = Map::Instance().CurrentLevel->MapArray[PosX][PosY].get();
  _currentCell->Occupied = true;
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
        MeleeAttack(go);
        return true;
      }
    }
  }

  return false;
}

bool Player::Move(int dx, int dy)
{
  auto cell = Map::Instance().CurrentLevel->MapArray[PosX + dx][PosY + dy].get();
  auto staticObject = Map::Instance().CurrentLevel->StaticMapObjects[PosX + dx][PosY + dy].get();

  if (!cell->Blocking)
  {
    if (staticObject != nullptr)
    {
      if (staticObject->Blocking)
      {
        // Search for components only if there are any
        if (staticObject->ComponentsSize() != 0)
        {
          auto c = staticObject->GetComponent<DoorComponent>();

          // Automatically interact with door if it's closed
          if (c != nullptr && staticObject->Interact())
          {
            FinishTurn();
          }
        }
      }
      else
      {
        MoveGameObject(dx, dy);
        return true;
      }
    }
    else if (cell->Occupied)
    {
      // Do not return true, since it will cause
      // MapOffsetY and MapOffsetX to be incremented in MainState,
      // but we manually call AdjustCamera() inside this method in
      // SwitchPlaces().
      PassByNPC(dx, dy);
    }
    else
    {
      MoveGameObject(dx, dy);
      return true;
    }
  }

  return false;
}

void Player::PassByNPC(int dx, int dy)
{
  auto actor = Map::Instance().GetActorAtPosition(PosX + dx, PosY + dy);
  auto c = actor->GetComponent<AIComponent>();
  AIComponent* aic = static_cast<AIComponent*>(c);
  AINPC* npc = static_cast<AINPC*>(aic->CurrentModel);
  if (!aic->CurrentModel->IsAgressive)
  {
    if (npc->Data.IsStanding)
    {
      std::string name = (npc->Data.IsAquainted) ? npc->Data.Name : "The " + actor->ObjectName;
      Printer::Instance().AddMessage(name + " won't move over");
    }
    else
    {
      SwitchPlaces(aic);
    }
  }
}

void Player::CheckVisibility()
{  
  int tw = Printer::Instance().TerminalWidth;
  int th = Printer::Instance().TerminalHeight;

  // Update map around player

  auto& map = Map::Instance().CurrentLevel->MapArray;
  auto& staticObjects = Map::Instance().CurrentLevel->StaticMapObjects;

  // FIXME: some objects can modify visibility radius
  int radius = (map[PosX][PosY]->ObjectName == "Tree") ? VisibilityRadius.Get() / 4 : VisibilityRadius.Get();

  auto mapCells = Util::GetRectAroundPoint(PosX, PosY, tw / 2, th / 2, Map::Instance().CurrentLevel->MapSize);
  for (auto& cell : mapCells)
  {
    map[cell.X][cell.Y]->Visible = false;    

    if (staticObjects[cell.X][cell.Y] != nullptr)
    {
      staticObjects[cell.X][cell.Y]->Visible = false;
    }
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

      if (map[point.X][point.Y]->BlocksSight
          || (staticObjects[point.X][point.Y] != nullptr
           && staticObjects[point.X][point.Y]->BlocksSight) )
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
  auto& staticObjects = Map::Instance().CurrentLevel->StaticMapObjects;

  auto curLvl = Map::Instance().CurrentLevel;

  if (x == curLvl->LevelExit.X
   && y == curLvl->LevelExit.Y
   && !curLvl->ExitFound)
  {
    curLvl->ExitFound = true;
  }

  map[x][y]->Visible = true;

  if (staticObjects[x][y] != nullptr)
  {
    staticObjects[x][y]->Visible = true;
  }

  if (!map[x][y]->Revealed)
  {
    map[x][y]->Revealed = true;

    if (staticObjects[x][y] != nullptr)
    {
      staticObjects[x][y]->Revealed = true;
    }
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
  Attrs.HP.Talents = 3;

  Attrs.Str.Set(3);
  Attrs.Def.Set(2);
  Attrs.Skl.Set(1);

  Attrs.HP.Set(30);

  Attrs.HungerRate.Set(1500);
  Attrs.HungerSpeed.Set(1);

  HealthRegenTurns = 30;
}

void Player::SetThiefAttrs()
{
  Attrs.Spd.Talents = 3;
  Attrs.Skl.Talents = 2;
  Attrs.Def.Talents = 1;
  Attrs.HP.Talents = 1;

  Attrs.Def.Set(1);
  Attrs.Skl.Set(2);
  Attrs.Spd.Set(3);

  Attrs.HP.Set(20);

  Attrs.HungerRate.Set(2000);
  Attrs.HungerSpeed.Set(1);  

  HealthRegenTurns = 50;
}

void Player::SetArcanistAttrs()
{
  Attrs.Mag.Talents = 3;
  Attrs.Res.Talents = 2;
  Attrs.Spd.Talents = 1;
  Attrs.MP.Talents = 3;

  Attrs.Mag.Set(2);
  Attrs.Res.Set(2);
  Attrs.Spd.Set(1);

  Attrs.HP.Set(10);
  Attrs.MP.Set(30);

  Attrs.HungerRate.Set(3000);
  Attrs.HungerSpeed.Set(1);  

  HealthRegenTurns = 80;
}

void Player::SetDefaultEquipment()
{
  EquipmentByCategory[EquipmentCategory::HEAD]   = { nullptr };
  EquipmentByCategory[EquipmentCategory::NECK]   = { nullptr };
  EquipmentByCategory[EquipmentCategory::TORSO]  = { nullptr };
  EquipmentByCategory[EquipmentCategory::LEGS]   = { nullptr };
  EquipmentByCategory[EquipmentCategory::BOOTS]  = { nullptr };
  EquipmentByCategory[EquipmentCategory::WEAPON] = { nullptr };
  EquipmentByCategory[EquipmentCategory::SHIELD] = { nullptr };
  EquipmentByCategory[EquipmentCategory::RING]   = { nullptr, nullptr };

  std::vector<GameObject*> weaponAndArmor;

  GameObject* weapon = nullptr;
  GameObject* armor = nullptr;

  switch (GetClass())
  {
    case PlayerClass::THIEF:
    {
      weapon = GameObjectsFactory::Instance().CreateRangedWeapon(0, 0, RangedWeaponType::SHORT_BOW, ItemPrefix::UNCURSED);
      Inventory.AddToInventory(weapon);

      GameObject* arrows = GameObjectsFactory::Instance().CreateArrows(0, 0, ArrowType::ARROWS, ItemPrefix::UNCURSED, 60);
      Inventory.AddToInventory(arrows);

      weaponAndArmor.push_back(arrows);
      weaponAndArmor.push_back(weapon);      
    }
    break;

    case PlayerClass::SOLDIER:
    {
      weapon = GameObjectsFactory::Instance().CreateWeapon(0, 0, WeaponType::SHORT_SWORD, ItemPrefix::UNCURSED);
      Inventory.AddToInventory(weapon);

      armor = GameObjectsFactory::Instance().CreateArmor(0, 0, ArmorType::LEATHER, ItemPrefix::UNCURSED);
      Inventory.AddToInventory(armor);

      weaponAndArmor.push_back(weapon);
      weaponAndArmor.push_back(armor);
    }
    break;

    case PlayerClass::ARCANIST:
    {
      weapon = GameObjectsFactory::Instance().CreateWeapon(0, 0, WeaponType::STAFF, ItemPrefix::UNCURSED);
      Inventory.AddToInventory(weapon);

      weaponAndArmor.push_back(weapon);
    }
    break;
  }
  // Since equipping produces message in log,
  // we explicitly delete it here.

  for (auto& i : weaponAndArmor)
  {
    ItemComponent* ic = i->GetComponent<ItemComponent>();
    ic->Equip();

    auto it = Printer::Instance().Messages().begin();
    Printer::Instance().Messages().erase(it);
  }
}

/// 'what' is either actor or GameObject, 'with' is a weapon
void Player::RangedAttack(GameObject* what, ItemComponent* with)
{
  int dmg = Util::RollDamage(with->Data.Damage.CurrentValue, with->Data.Damage.OriginalValue);

  // If it's not the ground GameObject
  if (what->Type != GameObjectType::GROUND)
  {
    what->ReceiveDamage(this, dmg, false);

    if (what->IsDestroyed)
    {
      ProcessKill(what);
    }
  }
  else
  {
    // Create arrow object on the cell where it landed
    GameObject* arrow = GameObjectsFactory::Instance().CopycatItem(EquipmentByCategory[EquipmentCategory::SHIELD][0]);
    arrow->PosX = what->PosX;
    arrow->PosY = what->PosY;
    ItemComponent* ic = arrow->GetComponent<ItemComponent>();
    ic->Data.Amount = 1;    
    ic->Data.IsEquipped = false;
    Map::Instance().InsertGameObject(arrow);
  }

  ItemComponent* weapon = EquipmentByCategory[EquipmentCategory::WEAPON][0];
  ItemComponent* arrows = EquipmentByCategory[EquipmentCategory::SHIELD][0];

  arrows->Data.Amount--;

  if (arrows->Data.Amount <= 0)
  {
    BreakItem(arrows, true);
  }

  WeaponLosesDurability();

  if (ShouldBreak(weapon))
  {
    BreakItem(weapon);
  }
}

/// 'what' is either actor or GameObject, 'with' is a wand
void Player::MagicAttack(GameObject* what, ItemComponent* with)
{
  with->Data.Amount--;

  auto baseDamagePair = GlobalConstants::SpellBaseDamageByType.at(with->Data.SpellHeld);

  int bonus = Attrs.Mag.Get();

  int centralDamage = Util::RollDamage(baseDamagePair.first, baseDamagePair.second);

  centralDamage += bonus;

  switch (with->Data.SpellHeld)
  {
    case SpellType::STRIKE:
      ProcessMagicAttack(what, with, centralDamage, false);
      break;

    case SpellType::MAGIC_MISSILE:
    case SpellType::FROST:
      ProcessMagicAttack(what, with, centralDamage, true);
      break;

    case SpellType::FIREBALL:
      ProcessAoEDamage(what, with, centralDamage, true);
      break;
  }
}

void Player::ProcessMagicAttack(GameObject* target, ItemComponent* weapon, int damage, bool againstRes)
{
  Position p = { target->PosX, target->PosY };

  auto actor = Map::Instance().GetActorAtPosition(p.X, p.Y);
  if (actor != nullptr)
  {
    TryToDamageObject(actor, damage, againstRes);
  }
  else
  {
    auto mapObjs = Map::Instance().GetGameObjectsAtPosition(p.X, p.Y);
    for (auto& obj : mapObjs)
    {
      TryToDamageObject(obj, damage, againstRes);
    }
  }
}

void Player::ProcessAoEDamage(GameObject* target, ItemComponent* weapon, int centralDamage, bool againstRes)
{
  auto pointsAffected = Printer::Instance().DrawExplosion({ target->PosX, target->PosY }, 3);

  //Util::PrintVector("points affected", pointsAffected);

  for (auto& p : pointsAffected)
  {
    int d = Util::LinearDistance({ target->PosX, target->PosY }, p);
    if (d == 0)
    {
      d = 1;
    }

    int dmgHere = centralDamage / d;

    auto actor = Map::Instance().GetActorAtPosition(p.X, p.Y);
    TryToDamageObject(actor, dmgHere, againstRes);

    auto mapObjs = Map::Instance().GetGameObjectsAtPosition(p.X, p.Y);
    for (auto& obj : mapObjs)
    {
      TryToDamageObject(obj, dmgHere, againstRes);
    }

    // Check self damage
    if (PosX == p.X && PosY == p.Y)
    {
      int dmgHere = centralDamage / d;
      dmgHere -= Attrs.Res.Get();

      ReceiveDamage(this, dmgHere, true);
    }
  }
}

void Player::TryToDamageObject(GameObject* object, int amount, bool againstRes)
{
  if (object != nullptr)
  {
    int dmgHere = amount;

    if (againstRes)
    {
      dmgHere -= object->Attrs.Res.Get();
    }

    if (dmgHere <= 0)
    {
      auto msg = Util::StringFormat("%s seems unaffected!", object->ObjectName.data());
      Printer::Instance().AddMessage(msg);
    }
    else
    {
      object->ReceiveDamage(this, dmgHere, againstRes);
    }
  }
}

void Player::MeleeAttack(GameObject* go, bool alwaysHit)
{
  bool hitLanded = alwaysHit ? true : WasHitLanded(go);
  if (!hitLanded)
  {
    Application::Instance().DisplayAttack(go, GlobalConstants::DisplayAttackDelayMs, "You missed", "#FFFFFF");
  }
  else
  {
    ItemComponent* weapon = EquipmentByCategory[EquipmentCategory::WEAPON][0];
    int dmg = CalculateDamageValue(weapon, go);
    ProcessAttack(weapon, go, dmg);
  }

  Attrs.Hunger += Attrs.HungerSpeed.Get() * 2;

  FinishTurn();
}

void Player::ProcessAttack(ItemComponent* weapon, GameObject* defender, int damageToInflict)
{
  bool shouldTearDownWall = false;

  if (weapon != nullptr)
  {
    // Melee attack with ranged weapon in hand ignores durability,
    // since it is considered a punch
    bool isRanged = (weapon->Data.ItemType_ == ItemType::RANGED_WEAPON
                  || weapon->Data.ItemType_ == ItemType::WAND);

    shouldTearDownWall = (weapon->Data.WeaponType_ == WeaponType::PICKAXE);

    if (!isRanged)
    {
      WeaponLosesDurability();
    }

    Application::Instance().DisplayAttack(defender, GlobalConstants::DisplayAttackDelayMs, "", "#FF0000");

    if (ShouldBreak(weapon))
    {
      BreakItem(weapon);
    }
  }

  bool canBeTearedDown = (defender->Type == GameObjectType::PICKAXEABLE);
  bool isWallOnBorder = IsGameObjectBorder(defender);

  shouldTearDownWall &= (canBeTearedDown && !isWallOnBorder);

  if (shouldTearDownWall)
  {
    defender->Attrs.HP.Set(0);
    defender->IsDestroyed = true;

    auto msg = Util::StringFormat("You tear down the %s", defender->ObjectName.data());
    Printer::Instance().AddMessage(msg);
  }
  else
  {
    defender->ReceiveDamage(this, damageToInflict, false);

    if (defender->IsDestroyed)
    {
      ProcessKill(defender);
    }
  }
}

bool Player::IsGameObjectBorder(GameObject* go)
{
  auto& lvl = Map::Instance().CurrentLevel;

  return (go->PosX == 0
       || go->PosY == 0
       || go->PosX == lvl->MapSize.X - 2
       || go->PosY == lvl->MapSize.Y - 2);
}

bool Player::WasHitLanded(GameObject* defender)
{
  int attackChanceScale = 2;
  int defaultHitChance = 50;
  int hitChance = defaultHitChance;

  int d = Attrs.Skl.Get() - defender->Attrs.Skl.Get();

  hitChance += (d * attackChanceScale);

  hitChance = Util::Clamp(hitChance, GlobalConstants::MinHitChance, GlobalConstants::MaxHitChance);

  auto logMsg = Util::StringFormat("Player (SKL %i, LVL %i) attacks %s (SKL: %i, LVL %i): chance = %i",
                                   Attrs.Skl.CurrentValue,
                                   Attrs.Lvl.CurrentValue,
                                   defender->ObjectName.data(),
                                   defender->Attrs.Skl.CurrentValue,
                                   defender->Attrs.Lvl.CurrentValue,
                                   hitChance);
  Logger::Instance().Print(logMsg);

  return Util::Rolld100(hitChance);
}

int Player::CalculateDamageValue(ItemComponent* weapon, GameObject* defender)
{
  int totalDmg = 0;

  if (weapon == nullptr)
  {
    totalDmg = Util::RollDamage(1, 4);
    totalDmg += Attrs.Str.Get() - defender->Attrs.Def.Get();
    if (totalDmg <= 0)
    {
      totalDmg = 1;
    }
  }
  else
  {
    bool isRanged = (weapon->Data.ItemType_ == ItemType::RANGED_WEAPON
                  || weapon->Data.ItemType_ == ItemType::WAND);

    auto wd = weapon->Data.Damage;

    // Melee attack with ranged weapon in hand fallbacks to 1d4 "punch"
    int weaponDamage = isRanged ? Util::RollDamage(1, 4) :
                                  Util::RollDamage(wd.CurrentValue, wd.OriginalValue);

    totalDmg = weaponDamage;
    totalDmg += Attrs.Str.Get() - defender->Attrs.Def.Get();
    if (totalDmg <= 0)
    {
      totalDmg = 1;
    }
  }

  return totalDmg;
}

void Player::ReceiveDamage(GameObject* from, int amount, bool isMagical, bool godMode)
{  
  if (godMode)
  {
    std::string objName = (from != nullptr) ? from->ObjectName : "unknown";
    auto str = Util::StringFormat("You laugh at the face of %s", objName.data());
    Printer::Instance().AddMessage(str);
    return;
  }

  if (isMagical)
  {
    if (from == this)
    {
      auto str = Util::StringFormat("You hit yourself for %i damage!", amount);
      Printer::Instance().AddMessage(str);
      Attrs.HP.CurrentValue -= amount;
    }
  }
  else
  {
    if (!DamageArmor(amount))
    {
      auto str = Util::StringFormat("You were hit for %i damage", amount);
      Printer::Instance().AddMessage(str);

      Attrs.HP.CurrentValue -= amount;
    }
  }
}

bool Player::DamageArmor(int amount)
{
  ItemComponent* armor = EquipmentByCategory[EquipmentCategory::TORSO][0];
  if (armor != nullptr)
  {
    int durabilityLeft = armor->Data.Durability.CurrentValue;
    int armorDamage = durabilityLeft - amount;
    if (armorDamage < 0)
    {
      int hpDamage = std::abs(armorDamage);

      auto str = Util::StringFormat("You were hit for %i damage", hpDamage);
      Printer::Instance().AddMessage(str);

      Attrs.HP.CurrentValue -= hpDamage;
      BreakItem(armor);
    }
    else
    {
      auto str = Util::StringFormat("Your armor takes %i damage", amount);
      Printer::Instance().AddMessage(str);

      armor->Data.Durability.Add(-amount);
      if (ShouldBreak(armor))
      {
        BreakItem(armor);
      }
    }

    return true;
  }

  return false;
}

bool Player::ShouldBreak(ItemComponent *ic)
{
  return (ic->Data.Durability.CurrentValue <= 0);
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

void Player::LevelUp(int baseHpOverride)
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
  int maxRndMp = Attrs.Mag.OriginalValue + Attrs.MP.Talents;

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

void Player::ProcessKill(GameObject* monster)
{
  if (monster->Type == GameObjectType::HARMLESS)
  {
    return;
  }

  // FIXME: exp value is too unbalanced

  int dungeonLvl = Map::Instance().CurrentLevel->DungeonLevel;
  int defaultExp = monster->Attrs.Rating() - Attrs.Rating();

  defaultExp = monster->Attrs.Rating();
  defaultExp = Util::Clamp(defaultExp, 1, GlobalConstants::AwardedExpMax);

  //int exp = defaultExp * dungeonLvl; // + dungeonLvl
  int exp = defaultExp;

  GameObjectsFactory::Instance().GenerateLootIfPossible(monster->PosX, monster->PosY, monster->Type);

  AwardExperience(exp);

  if (TotalKills.count(monster->ObjectName))
  {
    TotalKills[monster->ObjectName]++;
  }
  else
  {
    TotalKills[monster->ObjectName] = 1;
  }
}

void Player::WaitForTurn()
{
  GameObject::WaitForTurn();

  // Redraw screen only when player is ready for action
  // (assuming Application::_currentState is MainState)
  if (Attrs.ActionMeter >= GlobalConstants::TurnReadyValue)
  {
    // FIXME: try to optimize.
    // When you move player in dungeons while holding direction key,
    // it gets "jaggy" due to this constant redrawing of stuff.
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
      std::string str;

      if (damager == this)
      {
        str = Util::StringFormat("%s has commited suicide", Name.data());
      }
      else
      {
        str = Util::StringFormat("%s was killed by a %s", Name.data(), damager->ObjectName.data());
      }

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
  // NOTE: probably lots of shitcode anyway

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
  Attrs.ActionMeter -= GlobalConstants::TurnReadyValue;

  if (Attrs.ActionMeter < 0)
  {
    Attrs.ActionMeter = 0;
  }

  ProcessStarvation();
  GameObject::ProcessEffects();
}

void Player::ProcessStarvation()
{
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
      ReceiveDamage(nullptr, 1, false);
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
  auto go = GameObjectsFactory::Instance().CreateHealingPotion(ItemPrefix::UNCURSED);
  ItemComponent* ic = go->GetComponent<ItemComponent>();
  ic->Data.Amount = 3;

  Inventory.AddToInventory(go);

  go = GameObjectsFactory::Instance().CreateFood(0, 0, FoodType::IRON_RATIONS, ItemPrefix::UNCURSED);
  ic = go->GetComponent<ItemComponent>();
  ic->Data.Amount = 1;
  ic->Data.IsIdentified = true;

  Inventory.AddToInventory(go);

  go = GameObjectsFactory::Instance().CreateRepairKit(0, 0, 30, ItemPrefix::BLESSED);

  Inventory.AddToInventory(go);
}

void Player::SetThiefDefaultItems()
{
  Money = 100;

  auto go = GameObjectsFactory::Instance().CreateHealingPotion(ItemPrefix::UNCURSED);
  Inventory.AddToInventory(go);

  go = GameObjectsFactory::Instance().CreateManaPotion(ItemPrefix::UNCURSED);
  Inventory.AddToInventory(go);  
}

void Player::SetArcanistDefaultItems()
{
  auto go = GameObjectsFactory::Instance().CreateManaPotion(ItemPrefix::BLESSED);
  auto c = go->GetComponent<ItemComponent>();
  ItemComponent* ic = static_cast<ItemComponent*>(c);
  ic->Data.Amount = 5;

  Inventory.AddToInventory(go);

  go = GameObjectsFactory::Instance().CreateReturner(0, 0, 3, ItemPrefix::UNCURSED);
  Inventory.AddToInventory(go);
}

void Player::SetDefaultSkills()
{
  switch (GetClass())
  {
    case PlayerClass::SOLDIER:
      SkillLevelBySkill[PlayerSkills::REPAIR] = 1;
      break;

    case PlayerClass::ARCANIST:
      SkillLevelBySkill[PlayerSkills::RECHARGE] = 1;
      SkillLevelBySkill[PlayerSkills::SPELLCASTING] = 1;
      break;

    case PlayerClass::THIEF:
      SkillLevelBySkill[PlayerSkills::AWARENESS] = 1;
      break;
  }
}

bool Player::WeaponLosesDurability()
{
  auto weapon = EquipmentByCategory[EquipmentCategory::WEAPON][0];
  weapon->Data.Durability.Add(-1);

  return true;
}

void Player::RecalculateStatsModifiers()
{
  for (auto& a : _attributesRefsByType)
  {
    a.second.Modifier = 0;

    for (auto& kvp : EquipmentByCategory)
    {
      for (ItemComponent* item : kvp.second)
      {
        if (item != nullptr)
        {
          int bonus = item->Data.StatBonuses.at(a.first);
          a.second.Modifier += bonus;
        }
      }
    }
  }

  // Hardcoded weapon and armor penalties

  ItemComponent* weapon = EquipmentByCategory.at(EquipmentCategory::WEAPON)[0];
  if (weapon != nullptr)
  {
    int req = weapon->Data.StatRequirements.at(StatsEnum::STR);
    if(req != 0)
    {
      int penalty = _attributesRefsByType.at(StatsEnum::STR).CurrentValue - req;
      if (penalty < 0)
      {
        _attributesRefsByType.at(StatsEnum::SKL).Modifier += penalty;        
      }
    }
  }

  ItemComponent* armor = EquipmentByCategory.at(EquipmentCategory::TORSO)[0];
  if (armor != nullptr)
  {
    int req = armor->Data.StatRequirements.at(StatsEnum::STR);
    if(req != 0)
    {
      int penalty = _attributesRefsByType.at(StatsEnum::STR).CurrentValue - req;
      if (penalty < 0)
      {
        _attributesRefsByType.at(StatsEnum::SPD).Modifier += penalty;
      }
    }
  }
}

void Player::BreakItem(ItemComponent* ic, bool suppressMessage)
{
  if (!suppressMessage)
  {
    auto objName = ic->OwnerGameObject->ObjectName;

    std::string breakStr = (ic->Data.ItemType_ == ItemType::ARMOR)
                          ? "is destroyed!"
                          : "breaks!";

    auto str = Util::StringFormat("%s %s", objName.data(), breakStr.data());
    Printer::Instance().AddMessage(str);
  }

  auto typeHash = ic->Data.ItemTypeHash;

  EquipmentCategory ec = ic->Data.EqCategory;

  for (int i = 0; i < Inventory.Contents.size(); i++)
  {
    auto c = Inventory.Contents[i]->GetComponent<ItemComponent>();
    ItemComponent* ic = static_cast<ItemComponent*>(c);

    if (ic->Data.ItemTypeHash == typeHash && ic->Data.IsEquipped)
    {
      Inventory.Contents.erase(Inventory.Contents.begin() + i);
      break;
    }
  }

  EquipmentByCategory[ec][0] = nullptr;

  RecalculateStatsModifiers();
}

void Player::SwitchPlaces(AIComponent* other)
{
  int dxPlayer = other->OwnerGameObject->PosX - PosX;
  int dyPlayer = other->OwnerGameObject->PosY - PosY;

  int dxNpc = -dxPlayer;
  int dyNpc = -dyPlayer;

  MoveGameObject(dxPlayer, dyPlayer);
  other->OwnerGameObject->Move(dxNpc, dyNpc);
  other->OwnerGameObject->FinishTurn();

  Map::Instance().CurrentLevel->AdjustCamera();

  AINPC* npc = static_cast<AINPC*>(other->CurrentModel);
  std::string name = (npc->Data.IsAquainted) ? npc->Data.Name : "the " + other->OwnerGameObject->ObjectName;
  Printer::Instance().AddMessage("You pass by " + name);
}

void Player::AddBonusItems()
{
  GameObject* go = nullptr;
  std::vector<std::string> text;

  switch(GetClass())
  {
    case PlayerClass::THIEF:
    {
      text =
      {
        "REWARD 10,000",
        "",
        "For the capture dead or alive of one",
        "",
        Name,
        "",
        "the leader of the worst band of thieves",
        "the City has ever had to deal with.",
        "The above reward will be paid",
        "for his / her capture",
        "or positive proof of death.",
        "",
        "Signed:",
        "NORMAN TRUART, Sheriff",
        "City Watch HQ",
        "21 / II / 988"
      };

      go = GameObjectsFactory::Instance().CreateNote("Wanted Poster", text);
      Inventory.AddToInventory(go);
    }
    break;

    case PlayerClass::SOLDIER:
    {
      text =
      {
        "Pvt. " + Name,
        "is hereby granted permission",
        "to take a leave of absence",
        "from 13 / III / 988",
        "until 13 / IV / 988",
        "",
        "Signed:",
        "Lt. LAURA MOSLEY",
        "City Watch, Stonemarket Dpt.",
        "21 / II / 988"
      };

      go = GameObjectsFactory::Instance().CreateNote("Leave Warrant", text);
      Inventory.AddToInventory(go);
    }
    break;

    case PlayerClass::ARCANIST:
    {
      text =
      {
        Name,
        "",
        "The Order instructs you to visit",
        Map::Instance().CurrentLevel->LevelName,
        "Investigate the place as thorough as possible",
        "for we believe there is a disturbance",
        "lurking there somewhere.",
        "Find the source of this unbalance,",
        "restore it and come back to us.",
        "We'll be praying for your success.",
        "",
        "Signed:",
        "+ Bishop DANIEL"
      };

      go = GameObjectsFactory::Instance().CreateNote("Orders", text);
      Inventory.AddToInventory(go);
    }
    break;
  }
}

PlayerClass Player::GetClass()
{
  return _classesMap[SelectedClass];
}

std::string& Player::GetClassName()
{
  return _classesName[SelectedClass];
}

bool Player::HasSkill(PlayerSkills skillToCheck)
{
  return (SkillLevelBySkill.count(skillToCheck) == 1);
}
