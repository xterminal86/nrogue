#include "player.h"

#include "map.h"
#include "printer.h"
#include "door-component.h"
#include "game-objects-factory.h"
#include "items-factory.h"
#include "ai-monster-basic.h"
#include "ai-component.h"
#include "ai-npc.h"
#include "application.h"
#include "spells-database.h"
#include "custom-class-state.h"

void Player::Init()
{
  Type = GameObjectType::PLAYER;

  PosX = Map::Instance().CurrentLevel->LevelStart.X;
  PosY = Map::Instance().CurrentLevel->LevelStart.Y;

  Image = '@';

  IsLiving = true;

  #ifdef USE_SDL
  Image = GlobalConstants::CP437IndexByType[NameCP437::FACE_2];
  #endif

  FgColor = Colors::PlayerColor;
  Attrs.ActionMeter = GlobalConstants::TurnReadyValue;

  Inventory.MaxCapacity = GlobalConstants::InventoryMaxSize;

  SetAttributes();
  SetDefaultItems();
  SetDefaultEquipment();
  SetDefaultSkills();

  _previousCell = Map::Instance().CurrentLevel->MapArray[PosX][PosY].get();
  _currentCell = Map::Instance().CurrentLevel->MapArray[PosX][PosY].get();
  _currentCell->Occupied = true;

  Attrs.Exp.Reset(0);
  Attrs.Exp.SetMax(100);

  DistanceField.Init(this, 40);

  // FIXME: debug

  //Money = 0;
  //Money = 100000;
  //Attrs.HP.Reset(1);
  //Attrs.HungerRate.Set(0);
}

void Player::Draw()
{
  auto& mapRef = Map::Instance().CurrentLevel;

  // If game object has black bg color,
  // replace it with current floor color
  std::string bgColor = BgColor;

  bool cond = (BgColor == Colors::BlackColor);
  bool isOnStaticObject = (mapRef->StaticMapObjects[PosX][PosY] != nullptr);

  if (cond)
  {
    if (isOnStaticObject)
    {
      bgColor = mapRef->StaticMapObjects[PosX][PosY]->BgColor;
    }
    else
    {
      bgColor = mapRef->MapArray[PosX][PosY]->BgColor;
    }
  }

  GameObject::Draw(FgColor, bgColor);
}

bool Player::TryToAttack(int dx, int dy)
{
  _attackDir.X = dx;
  _attackDir.Y = dy;

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

  bool moveOk = false;
  bool passByNPC = false;

  if (!cell->Blocking)
  {
    if (staticObject != nullptr)
    {
      if (staticObject->Blocking)
      {
        auto dc = staticObject->GetComponent<DoorComponent>();

        // Automatically interact with door if it's closed
        if (dc != nullptr)
        {
          if (staticObject->Interact())
          {
            if (dc->OpenedBy == GlobalConstants::OpenedByAnyone)
            {
              auto str = Util::StringFormat("You %s: %s", (dc->IsOpen ? "opened" : "closed"), staticObject->ObjectName.data());
              Printer::Instance().AddMessage(str);
            }
          }
        }
      }
      else
      {
        MoveGameObject(dx, dy);
        moveOk = true;
      }
    }
    else if (cell->Occupied)
    {
      auto actor = Map::Instance().GetActorAtPosition(PosX + dx, PosY + dy);
      if (actor != nullptr)
      {
        // Do not return true, since it will cause
        // MapOffsetY and MapOffsetX to be incremented in MainState,
        // but we manually call AdjustCamera() inside this method in
        // SwitchPlaces().
        passByNPC = PassByNPC(actor);
      }
    }
    else
    {
      MoveGameObject(dx, dy);
      moveOk = true;
    }
  }

  if (moveOk || passByNPC)
  {
    DistanceField.SetDirty();
  }

  return moveOk;
}

bool Player::PassByNPC(GameObject* actor)
{
  bool ok = true;

  auto c = actor->GetComponent<AIComponent>();
  AIComponent* aic = static_cast<AIComponent*>(c);
  AINPC* npc = static_cast<AINPC*>(aic->CurrentModel);
  if (!aic->CurrentModel->IsAgressive)
  {
    if (npc->Data.IsImmovable)
    {
      std::string name = (npc->Data.IsAquainted) ? npc->Data.Name : "The " + actor->ObjectName;
      Printer::Instance().AddMessage(name + " won't move over");
      ok = false;
    }
    else
    {
      SwitchPlaces(aic);
    }
  }

  return ok;
}

void Player::CheckVisibility()
{
  int tw = Printer::TerminalWidth;
  int th = Printer::TerminalHeight;

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

    case PlayerClass::CUSTOM:
      SetCustomClassAttrs();
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

  Attrs.HP.Reset(35);

  Attrs.HungerRate.Set(1500);
  Attrs.HungerSpeed.Set(1);

  HealthRegenTurns = 40;
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

  Attrs.HP.Reset(15);

  Attrs.HungerRate.Set(2000);
  Attrs.HungerSpeed.Set(1);

  HealthRegenTurns = 60;
}

void Player::SetArcanistAttrs()
{
  Attrs.Mag.Talents = 3;
  Attrs.Res.Talents = 2;
  Attrs.Spd.Talents = 1;
  Attrs.MP.Talents = 3;

  Attrs.Mag.Set(3);
  Attrs.Res.Set(2);
  Attrs.Spd.Set(1);

  Attrs.HP.Reset(5);
  Attrs.MP.Reset(30);

  Attrs.HungerRate.Set(3000);
  Attrs.HungerSpeed.Set(1);

  HealthRegenTurns = 80;
}

void Player::SetCustomClassAttrs()
{
  GameState* stateRef = Application::Instance().GetGameStateRefByName(GameStates::CUSTOM_CLASS_STATE);
  CustomClassState* ccs = static_cast<CustomClassState*>(stateRef);
  ccs->InitPlayerAttributes(this);
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

  std::vector<GameObject*> weaponAndArmorToEquip;

  GameObject* weapon = nullptr;
  GameObject* armor = nullptr;

  switch (GetClass())
  {
    case PlayerClass::THIEF:
    {
      weapon = ItemsFactory::Instance().CreateRangedWeapon(0, 0, RangedWeaponType::SHORT_BOW, ItemPrefix::UNCURSED, ItemQuality::NORMAL);
      Inventory.Add(weapon);

      GameObject* arrows = ItemsFactory::Instance().CreateArrows(0, 0, ArrowType::ARROWS, ItemPrefix::BLESSED, 60);
      Inventory.Add(arrows);

      GameObject* dagger = ItemsFactory::Instance().CreateWeapon(0, 0, WeaponType::DAGGER, ItemPrefix::UNCURSED, ItemQuality::NORMAL);
      Inventory.Add(dagger);

      weaponAndArmorToEquip.push_back(arrows);
      weaponAndArmorToEquip.push_back(weapon);
    }
    break;

    case PlayerClass::SOLDIER:
    {
      weapon = ItemsFactory::Instance().CreateWeapon(0, 0, WeaponType::SHORT_SWORD, ItemPrefix::UNCURSED, ItemQuality::NORMAL);
      Inventory.Add(weapon);

      armor = ItemsFactory::Instance().CreateArmor(0, 0, ArmorType::PADDING, ItemPrefix::UNCURSED, ItemQuality::NORMAL);
      Inventory.Add(armor);

      weaponAndArmorToEquip.push_back(weapon);
      weaponAndArmorToEquip.push_back(armor);
    }
    break;

    case PlayerClass::ARCANIST:
    {
      weapon = ItemsFactory::Instance().CreateWeapon(0, 0, WeaponType::STAFF, ItemPrefix::UNCURSED, ItemQuality::NORMAL);
      Inventory.Add(weapon);

      auto wand = ItemsFactory::Instance().CreateWand(0, 0, WandMaterials::EBONY_3, SpellType::MAGIC_MISSILE, ItemPrefix::BLESSED, ItemQuality::NORMAL);
      Inventory.Add(wand);

      weaponAndArmorToEquip.push_back(weapon);
    }
    break;

    case PlayerClass::CUSTOM:
    {
      weapon = ItemsFactory::Instance().CreateRandomWeapon();
      Inventory.Add(weapon);

      armor = ItemsFactory::Instance().CreateRandomArmor();
      Inventory.Add(armor);

      auto acc = ItemsFactory::Instance().CreateRandomAccessory(0, 0);
      Inventory.Add(acc);

      auto potion = ItemsFactory::Instance().CreateRandomPotion();
      Inventory.Add(potion);

      auto gem = ItemsFactory::Instance().CreateGem(0, 0);
      Inventory.Add(gem);
    }
    break;
  }

  // Since equipping produces message in log,
  // we explicitly delete it here.
  for (auto& i : weaponAndArmorToEquip)
  {
    ItemComponent* ic = i->GetComponent<ItemComponent>();
    ic->Equip();

    auto it = Printer::Instance().Messages().begin();
    Printer::Instance().Messages().erase(it);
  }
}

/// 'what' is either actor or GameObject, 'with' is a weapon (i.e. arrow)
void Player::RangedAttack(GameObject* what, ItemComponent* with)
{
  ItemComponent* weapon = EquipmentByCategory[EquipmentCategory::WEAPON][0];
  ItemComponent* arrows = EquipmentByCategory[EquipmentCategory::SHIELD][0];

  int dmg = CalculateDamageValue(with, what, false);

  // If it's not the ground GameObject
  if (what->Type != GameObjectType::GROUND)
  {
    bool succ = what->ReceiveDamage(this, dmg, false);
    if (succ && weapon->Data.HasBonus(ItemBonusType::LEECH) && what->IsLiving)
    {
      Attrs.HP.AddMin(dmg);
    }

    AIComponent* aic = what->GetComponent<AIComponent>();

    if (aic != nullptr
     && weapon != nullptr
     && weapon->Data.HasBonus(ItemBonusType::KNOCKBACK))
    {
      int tiles = weapon->Data.GetBonus(ItemBonusType::KNOCKBACK)->BonusValue;
      KnockBack(what, tiles);
    }

    if (what->IsDestroyed)
    {
      ProcessKill(what);
    }
  }
  else if (what->Type != GameObjectType::DEEP_WATER
        || what->Type != GameObjectType::LAVA)
  {
    // Create arrow object on the cell where it landed
    ItemComponent* arrow = GameObjectsFactory::Instance().CloneItem(EquipmentByCategory[EquipmentCategory::SHIELD][0]);
    arrow->OwnerGameObject->PosX = what->PosX;
    arrow->OwnerGameObject->PosY = what->PosY;
    arrow->Data.Amount = 1;
    arrow->Data.IsEquipped = false;
    Map::Instance().InsertGameObject(arrow->OwnerGameObject);
  }

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

  SpellInfo si = with->Data.SpellHeld;

  auto baseDamagePair = si.SpellBaseDamage;

  int bonus = Attrs.Mag.Get();

  int centralDamage = Util::RollDamage(baseDamagePair.first, baseDamagePair.second);

  centralDamage += bonus;

  // TODO: cursed wands side-effects?

  switch (with->Data.SpellHeld.SpellType_)
  {
    case SpellType::NONE:
      Printer::Instance().AddMessage(GlobalConstants::NoActionText);
      break;

    case SpellType::STRIKE:
      ProcessMagicAttack(what, with, centralDamage, false);
      break;

    case SpellType::MAGIC_MISSILE:
    case SpellType::FROST:
      ProcessMagicAttack(what, with, centralDamage, true);
      break;

    case SpellType::FIREBALL:
      Map::Instance().ProcessAoEDamage(what, with, centralDamage, true);
      break;

    case SpellType::TELEPORT:
      ProcessTeleport(what, with);
      break;
  }
}

void Player::ProcessTeleport(GameObject* target, ItemComponent* weapon)
{
  Position p = { target->PosX, target->PosY };

  auto actor = Map::Instance().GetActorAtPosition(p.X, p.Y);
  if (actor != nullptr)
  {
    auto pos = Map::Instance().GetRandomEmptyCell();
    MapType mt = Map::Instance().CurrentLevel->MapType_;
    Map::Instance().TeleportToExistingLevel(mt, pos, actor);

    auto str = Util::StringFormat("%s suddenly disappears!", actor->ObjectName.data());
    Printer::Instance().AddMessage(str);
  }
}

void Player::ProcessMagicAttack(GameObject* target, ItemComponent* weapon, int damage, bool againstRes)
{
  Position p = { target->PosX, target->PosY };

  auto actor = Map::Instance().GetActorAtPosition(p.X, p.Y);
  if (actor != nullptr)
  {
    bool damageDone = Map::Instance().TryToDamageObject(actor, this, damage, againstRes);
    if (weapon->Data.SpellHeld.SpellType_ == SpellType::FROST)
    {
      ItemBonusStruct b;
      b.Type = ItemBonusType::FROZEN;
      b.BonusValue = 1;
      b.Duration = GlobalConstants::TurnReadyValue * damage;
      b.Id = weapon->OwnerGameObject->ObjectId();
      b.Cumulative = true;

      actor->AddEffect(b);
    }

    if (damageDone && actor->IsDestroyed)
    {
      ProcessKill(actor);
    }
  }
  else
  {
    // Magic attack should probably hit everything,
    // since it's, well, magical. You can't really control it.

    // Check items first
    auto mapObjs = Map::Instance().GetGameObjectsAtPosition(p.X, p.Y);
    for (auto& obj : mapObjs)
    {
      Map::Instance().TryToDamageObject(obj, this, damage, againstRes);
    }

    // If nothing is lying in the doorway, for example, damage door
    if (mapObjs.empty())
    {
      auto so = Map::Instance().GetStaticGameObjectAtPosition(p.X, p.Y);
      if (so != nullptr)
      {
        Map::Instance().TryToDamageObject(so, this, damage, againstRes);
      }
    }
  }
}

void Player::MeleeAttack(GameObject* go, bool alwaysHit)
{
  bool hitLanded = alwaysHit ? true : WasHitLanded(go);
  if (!hitLanded)
  {
    Application::Instance().DisplayAttack(go, GlobalConstants::DisplayAttackDelayMs, "You missed", Colors::WhiteColor);
  }
  else
  {
    Application::Instance().DisplayAttack(go, GlobalConstants::DisplayAttackDelayMs, "", Colors::RedColor);
    ItemComponent* weapon = EquipmentByCategory[EquipmentCategory::WEAPON][0];

    bool isRanged = false;
    if (weapon != nullptr)
    {
      isRanged = (weapon->Data.ItemType_ == ItemType::RANGED_WEAPON
               || weapon->Data.ItemType_ == ItemType::WAND);
    }

    int dmg = CalculateDamageValue(weapon, go, isRanged);
    ProcessAttack(weapon, go, dmg);

    AIComponent* aic = go->GetComponent<AIComponent>();

    if (aic != nullptr
     && weapon != nullptr
     && weapon->Data.HasBonus(ItemBonusType::KNOCKBACK))
    {
      int tiles = weapon->Data.GetBonus(ItemBonusType::KNOCKBACK)->BonusValue;
      KnockBack(go, tiles);
    }
  }

  Attrs.Hunger += Attrs.HungerSpeed.Get() * 2;
}

void Player::KnockBack(GameObject* go, int tiles)
{
  auto& mapRef = Map::Instance().CurrentLevel->MapArray;
  auto curLvl = Map::Instance().CurrentLevel;

  Position newPos = { go->PosX, go->PosY };

  bool canBeMoved = false;

  for (int i = 1; i <= tiles; i++)
  {
    newPos.X += _attackDir.X;
    newPos.Y += _attackDir.Y;

    if (mapRef[newPos.X][newPos.Y]->Occupied || curLvl->IsCellBlocking(newPos))
    {
      break;
    }

    canBeMoved = true;
  }

  if (canBeMoved)
  {
    ItemBonusStruct bs;
    bs.Type = ItemBonusType::PARALYZE;
    bs.BonusValue = 1;
    bs.Duration = 1;
    bs.Id = ObjectId();

    go->AddEffect(bs);
    go->FinishTurn();

    if (go->IsAlive())
    {
      go->MoveTo(newPos);
    }
  }
}

void Player::ProcessAttack(ItemComponent* weapon, GameObject* defender, int damageToInflict)
{
  bool shouldTearDownWall = false;
  bool hasLeech = false;

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

    if (weapon->Data.HasBonus(ItemBonusType::LEECH))
    {
      hasLeech = true;
    }

    if (ShouldBreak(weapon))
    {
      BreakItem(weapon);
    }
  }

  // NOTE: set object type to HARMLESS during instantiation
  // to avoid it being pickaxed away.
  //
  // GameObject default type is HARMLESS.
  //
  bool canBeTearedDown = (defender->Type == GameObjectType::PICKAXEABLE);
  bool isWallOnBorder  = IsGameObjectBorder(defender);

  shouldTearDownWall &= (canBeTearedDown && !isWallOnBorder);

  if (shouldTearDownWall)
  {
    defender->Attrs.HP.SetMin(0);
    defender->IsDestroyed = true;

    auto msg = Util::StringFormat("You tear down the %s", defender->ObjectName.data());
    Printer::Instance().AddMessage(msg);
  }
  else
  {
    bool succ = defender->ReceiveDamage(this, damageToInflict, false);
    if (succ && hasLeech && defender->IsLiving)
    {
      auto b = weapon->Data.GetBonus(ItemBonusType::LEECH);
      float fract = (float)b->BonusValue * 0.01f;
      int dmgToHp = (int)((float)damageToInflict * fract);
      if (dmgToHp != 0)
      {
        Attrs.HP.AddMin(dmgToHp);
      }
    }

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
  // Amount of addition to hit chance
  // times SKL difference of attacker and defender
  int attackChanceScale = 3;

  int defaultHitChance = 50;
  int hitChance = defaultHitChance;

  int skillDiff = Attrs.Skl.Get() - defender->Attrs.Skl.Get();
  int difficulty = (skillDiff * attackChanceScale);

  hitChance += difficulty;

  hitChance = Util::Clamp(hitChance, GlobalConstants::MinHitChance, GlobalConstants::MaxHitChance);

  if (HasEffect(ItemBonusType::BLINDNESS))
  {
    hitChance /= 2;
  }

  auto logMsg = Util::StringFormat("Player (SKL %i, LVL %i) attacks %s (SKL: %i, LVL %i): chance = %i",
                                   Attrs.Skl.Get(),
                                   Attrs.Lvl.Get(),
                                   defender->ObjectName.data(),
                                   defender->Attrs.Skl.Get(),
                                   defender->Attrs.Lvl.Get(),
                                   hitChance);
  Logger::Instance().Print(logMsg);

  return Util::Rolld100(hitChance);
}

int Player::CalculateDamageValue(ItemComponent* weapon, GameObject* defender, bool meleeAttackWithRangedWeapon)
{
  int totalDmg = 0;

  // Unarmed strike
  if (weapon == nullptr)
  {
    totalDmg = Util::RollDamage(1, 2);
    totalDmg += Attrs.Str.Get() - defender->Attrs.Def.Get();

    if (totalDmg <= 0)
    {
      totalDmg = 1;
    }
  }
  else
  {
    // Melee attack with ranged weapon in hand fallbacks to 1d4 "punch"
    int weaponDamage = meleeAttackWithRangedWeapon
                       ? Util::RollDamage(1, 2)
                       : Util::RollDamage(weapon->Data.Damage.Min().Get(),
                                          weapon->Data.Damage.Max().Get());

    totalDmg = weaponDamage;

    int targetDef = weapon->Data.HasBonus(ItemBonusType::IGNORE_DEFENCE)
                  ? 0
                  : defender->Attrs.Def.Get();

    ItemBonusStruct* res = weapon->Data.GetBonus(ItemBonusType::DAMAGE);
    if (res != nullptr)
    {
      totalDmg += res->BonusValue;
    }

    totalDmg += Attrs.Str.Get() - targetDef;

    if (totalDmg <= 0)
    {
      totalDmg = 1;
    }
  }

  // TODO: if STR is < 0 resulting damage is 0
  /*
  if (Attrs.Str.Get() < 0)
  {
    totalDmg = 0;
  }
  */

  return totalDmg;
}

std::string Player::ProcessMagicalDamage(GameObject* from, int& amount)
{
  std::string logMsg;

  if (from == this)
  {
    logMsg = Util::StringFormat("You hit yourself for %i damage!", amount);
  }
  else
  {
    logMsg = Util::StringFormat("%s => @ (%i)", from->ObjectName.data(), amount);
  }

  int abs = GetDamageAbsorbtionValue(true);
  amount -= abs;

  if (amount < 0)
  {
    amount = 0;
  }

  if (HasEffect(ItemBonusType::MANA_SHIELD) && Attrs.MP.Min().Get() != 0)
  {
    Attrs.MP.AddMin(-amount);
  }
  else
  {
    if (Attrs.MP.Min().Get() == 0)
    {
      DispelEffect(ItemBonusType::MANA_SHIELD);
    }

    Attrs.HP.AddMin(-amount);
  }

  return logMsg;
}

std::string Player::ProcessPhysicalDamage(GameObject* from, int& amount)
{
  std::string logMsg;

  int abs = GetDamageAbsorbtionValue(false);
  amount -= abs;

  if (amount < 0)
  {
    amount = 0;
  }

  if (!DamageArmor(from, amount))
  {
    if (amount == 0)
    {
      logMsg = Util::StringFormat("%s => @ (%i)", from->ObjectName.data(), amount);
    }
    else
    {
      logMsg = Util::StringFormat("%s => @ (%i)", from->ObjectName.data(), amount);
    }

    if (HasEffect(ItemBonusType::MANA_SHIELD) && Attrs.MP.Min().Get() != 0)
    {
      Attrs.MP.AddMin(-amount);
    }
    else
    {
      if (Attrs.MP.Min().Get() == 0)
      {
        DispelEffect(ItemBonusType::MANA_SHIELD);
      }

      Attrs.HP.AddMin(-amount);
    }
  }

  return logMsg;
}

void Player::ReceiveDamage(GameObject* from, int amount, bool isMagical, bool godMode, bool suppressLog)
{
  if (godMode)
  {
    std::string msgString;
    if (from != nullptr)
    {
      msgString = Util::StringFormat("You laugh at the face of %s!", from->ObjectName.data());
    }
    else
    {
      msgString = Util::StringFormat("%s => @ (0)", from->ObjectName.data());
    }

    Printer::Instance().AddMessage(msgString);

    return;
  }

  std::string logMsg;

  int dmgReturned = 0;

  if (isMagical)
  {
    logMsg = ProcessMagicalDamage(from, amount);
  }
  else
  {
    logMsg = ProcessPhysicalDamage(from, amount);

    auto thorns = GetItemsWithBonus(ItemBonusType::THORNS);
    for (auto& i : thorns)
    {
      auto b = i->Data.GetBonus(ItemBonusType::THORNS);
      float fract = (float)b->BonusValue * 0.01f;
      int dmg = (int)((float)amount * fract);
      dmgReturned += dmg;
    }
  }

  if (!suppressLog && !logMsg.empty())
  {
    Printer::Instance().AddMessage(logMsg);
  }

  if (dmgReturned != 0 && from != nullptr)
  {
    auto thornsLogMsg = Util::StringFormat("@ => %s (%i)", from->ObjectName.data(), dmgReturned);
    from->ReceiveDamage(this, dmgReturned, true, thornsLogMsg);
  }
}

bool Player::DamageArmor(GameObject* from, int amount)
{
  ItemComponent* armor = EquipmentByCategory[EquipmentCategory::TORSO][0];
  if (armor != nullptr)
  {
    if (armor->Data.HasBonus(ItemBonusType::INDESTRUCTIBLE))
    {
      return false;
    }

    int durabilityLeft = armor->Data.Durability.Min().Get();
    int armorDamage = durabilityLeft - amount;
    if (armorDamage < 0)
    {
      int hpDamage = std::abs(armorDamage);

      auto str = Util::StringFormat("%s => @ (%i)", from->ObjectName.data(), hpDamage);
      Printer::Instance().AddMessage(str);

      Attrs.HP.AddMin(-hpDamage);
      BreakItem(armor);
    }
    else
    {
      auto str = Util::StringFormat("%s => [ (%i)", from->ObjectName.data(), amount);
      Printer::Instance().AddMessage(str);

      armor->Data.Durability.AddMin(-amount);
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
  return (ic->Data.Durability.Min().Get() <= 0);
}

void Player::AwardExperience(int amount)
{
  int amnt = amount * (Attrs.Exp.Talents + 1);

  // FIXME: debug
  //int amnt = 100;

  Attrs.Exp.AddMin(amnt);

  auto msg = Util::StringFormat("+%i EXP", amnt);
  Printer::Instance().AddMessage(msg);

  if (Attrs.Exp.Min().Get() >= Attrs.Exp.Max().Get())
  {
    Attrs.Exp.SetMin(0);
    LevelUp();
  }
  else if (amnt < 0 && Attrs.Exp.Min().Get() <= 0)
  {
    Attrs.Exp.SetMin(0);
    LevelDown();
  }
}

void Player::LevelUp(int baseHpOverride)
{
  for (auto& kvp : _statRaisesMap)
  {
    kvp.second = 0;
  }

  Printer::Instance().AddMessage("You have gained a level!");

  // Try to raise main stats

  for (auto& i : _mainAttributes)
  {
    auto kvp = i.second;

    auto log = Util::StringFormat("Raising %s", kvp.first.data());
    Logger::Instance().Print(log);

    if (CanRaiseAttribute(kvp.second))
    {
      _statRaisesMap[kvp.first] = 1;

      kvp.second.Add(1);

      auto str = Util::StringFormat("%s +1", kvp.first.data());
      Printer::Instance().AddMessage(str);
    }
  }

  // HP and MP

  int minRndHp = (Attrs.HP.Talents + 1);
  int maxRndHp = 2 * (Attrs.HP.Talents + 1);

  int hpToAdd = RNG::Instance().RandomRange(minRndHp, maxRndHp + 1);
  Attrs.HP.AddMax(hpToAdd);

  _statRaisesMap["HP"] = hpToAdd;

  if (hpToAdd > 0)
  {
    auto str = Util::StringFormat("HP +%i", hpToAdd);
    Printer::Instance().AddMessage(str);
  }

  int minRndMp = Attrs.Mag.OriginalValue();
  int maxRndMp = Attrs.Mag.OriginalValue() + Attrs.MP.Talents;

  int mpToAdd = RNG::Instance().RandomRange(minRndMp, maxRndMp + 1);
  Attrs.MP.AddMax(mpToAdd);

  _statRaisesMap["MP"] = mpToAdd;

  if (mpToAdd > 0)
  {
    auto str = Util::StringFormat("MP +%i", mpToAdd);
    Printer::Instance().AddMessage(str);
  }

  Attrs.Lvl.Add(1);

  auto res = GetPrettyLevelUpText();

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "Level Up!", res, "#888800", "#000044");
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

      kvp.second.Add(-1);
      if (kvp.second.OriginalValue() < 0)
      {
        kvp.second.Set(0);
      }

      auto str = Util::StringFormat("%s -1", kvp.first.data());
      Printer::Instance().AddMessage(str);
    }
  }

  // HP and MP

  int minRndHp = (Attrs.HP.Talents + 1);
  int maxRndHp = 2 * (Attrs.HP.Talents + 1);

  int hpToAdd = RNG::Instance().RandomRange(minRndHp, maxRndHp + 1);
  Attrs.HP.AddMax(-hpToAdd);

  if (Attrs.HP.Max().OriginalValue() <= 0)
  {
    Attrs.HP.SetMax(1);
    Attrs.HP.Restore();
  }

  _statRaisesMap["HP"] = -hpToAdd;

  if (hpToAdd > 0)
  {
    auto str = Util::StringFormat("HP -%i", hpToAdd);
    Printer::Instance().AddMessage(str);
  }

  int minRndMp = Attrs.Mag.OriginalValue();
  int maxRndMp = Attrs.Mag.OriginalValue() + Attrs.MP.Talents;

  int mpToAdd = RNG::Instance().RandomRange(minRndMp, maxRndMp + 1);
  Attrs.MP.AddMax(-mpToAdd);

  if (Attrs.MP.Max().OriginalValue() < 0)
  {
    Attrs.MP.SetMax(0);
    Attrs.MP.Restore();
  }

  _statRaisesMap["MP"] = -mpToAdd;

  if (mpToAdd > 0)
  {
    auto str = Util::StringFormat("MP -%i", mpToAdd);
    Printer::Instance().AddMessage(str);
  }

  Attrs.Lvl.Add(-1);
  if (Attrs.Lvl.OriginalValue() <= 1)
  {
    Attrs.Lvl.Set(1);
  }

  auto res = GetPrettyLevelUpText();

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "Level DOWN!", res, Colors::RedColor, "#000044");

  Printer::Instance().AddMessage("You have LOST a level!");
}

void Player::ProcessKill(GameObject* monster)
{
  // Check for monster's inventory and drop all items from there if any
  ContainerComponent* cc = monster->GetComponent<ContainerComponent>();
  if (cc != nullptr)
  {
    for (auto& i : cc->Contents)
    {
      GameObject* obj = i.release();
      ItemComponent* ic = obj->GetComponent<ItemComponent>();
      ic->OwnerGameObject->SetLevelOwner(Map::Instance().CurrentLevel);
      ic->Transfer();
      ic->OwnerGameObject->PosX = monster->PosX;
      ic->OwnerGameObject->PosY = monster->PosY;
    }
  }

  // Try to generate loot from the kill itself
  GameObjectsFactory::Instance().GenerateLootIfPossible(monster->PosX, monster->PosY, monster->Type);

  // Do not award experience for nothing
  if (monster->Type != GameObjectType::HARMLESS
   && monster->Type != GameObjectType::REMAINS)
  {
    // FIXME: exp value is too unbalanced (maybe)

    //int dungeonLvl = Map::Instance().CurrentLevel->DungeonLevel;
    int defaultExp = monster->Attrs.Rating() - Attrs.Rating();

    defaultExp = monster->Attrs.Rating();
    defaultExp = Util::Clamp(defaultExp, 1, GlobalConstants::AwardedExpMax);

    //int exp = defaultExp * dungeonLvl; // + dungeonLvl
    int exp = defaultExp;

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
}

void Player::WaitForTurn()
{
  GameObject::WaitForTurn();
  ProcessEffectsPlayer();
  ProcessItemsEffects();
}

bool Player::IsAlive()
{
  if (Attrs.HP.Min().Get() <= 0)
  {
    Attrs.HP.SetMin(0);

    Image = '%';
    FgColor = Colors::PlayerColor;
    BgColor = Colors::RedColor;
    IsDestroyed = true;

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

    mbStr = Util::StringFormat("%s: %i", kvp.first.data(), kvp.second.OriginalValue());
    levelUpResults.push_back(mbStr);
  }

  levelUpResults.push_back("");

  mbStr = Util::StringFormat("HP:  %i", Attrs.HP.Max().OriginalValue());
  levelUpResults.push_back(mbStr);

  mbStr = Util::StringFormat("MP:  %i", Attrs.MP.Max().OriginalValue());
  levelUpResults.push_back(mbStr);

  // Try to make everything look pretty
  //
  // NOTE: probably lots of shitcode anyway

  size_t maxLen = 0;
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
  ConsumeEnergy();
  ProcessNaturalRegenMP();
  ProcessHunger();
  ProcessStarvation();
  ProcessEffectsPlayer();
  ProcessItemsEffects();

  // If player killed an enemy but can still make another turn,
  // we must check and remove objects marked for deletion
  // or Application::DrawCurrentState() won't reflect that visually,
  // and we might also still attack / interact with objects,
  // that should've already been destroyed, as well.
  //
  // Probably bad design anyway but fuck it.
  Map::Instance().RemoveDestroyed();
}

void Player::ProcessEffectsPlayer()
{
  GameObject::ProcessEffects();

  bool shouldRedraw = false;

  if (HasEffect(ItemBonusType::PARALYZE))
  {
    //Util::Sleep(100);
    Printer::Instance().AddMessage("You can't move!");
    shouldRedraw = true;
  }

  if (HasEffect(ItemBonusType::BURNING))
  {
    if (Util::Rolld100(50))
    {
      // One-time deletion, can leave "forward" loop iteration
      for (size_t i = 0; i < Inventory.Contents.size(); i++)
      {
        ItemComponent* ic = Inventory.Contents[i]->GetComponent<ItemComponent>();
        if (ic != nullptr)
        {
          if (ic->Data.IsBurnable)
          {
            std::string objName = ic->Data.IsIdentified ? ic->Data.IdentifiedName : ic->Data.UnidentifiedName;
            auto str = Util::StringFormat("%s burns up!", objName.data());
            Printer::Instance().AddMessage(str);
            Inventory.Contents.erase(Inventory.Contents.begin() + i);
            break;
          }
        }
      }
    }
  }

  if (shouldRedraw)
  {
    Application::Instance().ForceDrawMainState();
  }
}

void Player::ProcessStarvation()
{
  if (IsStarving)
  {
    _starvingTimeout++;

    if (_starvingTimeout > GlobalConstants::StarvationDamageTimeout)
    {
      _starvingTimeout = 0;
    }
  }
  else
  {
    ProcessNaturalRegenHP();
  }
}

void Player::ProcessHunger()
{
  // No starving in town
  if (Map::Instance().CurrentLevel->MapType_ == MapType::TOWN)
  {
    return;
  }

  // Check only rings and amulet
  std::vector<ItemComponent*> ringsAndAmuletRefs =
  {
    EquipmentByCategory[EquipmentCategory::RING][0],
    EquipmentByCategory[EquipmentCategory::RING][1],
    EquipmentByCategory[EquipmentCategory::NECK][0],
  };

  for (auto& ref : ringsAndAmuletRefs)
  {
    if (ref != nullptr && ref->Data.HasBonus(ItemBonusType::HUNGER))
    {
      return;
    }
  }

  Attrs.Hunger += Attrs.HungerSpeed.Get();

  // HungerRate's CurrentValue is equal to OriginalValue
  int maxHunger = Attrs.HungerRate.OriginalValue();

  Attrs.Hunger = Util::Clamp(Attrs.Hunger, 0, maxHunger);

  if (Attrs.Hunger == maxHunger)
  {
    if (_starvingTimeout > GlobalConstants::StarvationDamageTimeout - 1)
    {
      Printer::Instance().AddMessage("You are starving!");
      Attrs.HP.AddMin(-1);
    }

    IsStarving = true;
  }
  else
  {
    _starvingTimeout = 0;
    IsStarving = false;
  }
}

void Player::ProcessItemsEffects()
{
  for (auto& item : Inventory.Contents)
  {
    ItemComponent* ic = item->GetComponent<ItemComponent>();
    if (ic != nullptr)
    {
      for (auto& bonus : ic->Data.Bonuses)
      {
        switch (bonus.Type)
        {
          case ItemBonusType::SELF_REPAIR:
          {
            bonus.EffectCounter++;

            if ((bonus.EffectCounter % bonus.Period) == 0)
            {
              bonus.EffectCounter = 0;
              ic->Data.Durability.AddMin(bonus.BonusValue);
            }
          }
          break;
        }
      }
    }
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
  Money = 250;

  auto go = ItemsFactory::Instance().CreateHealingPotion(ItemPrefix::UNCURSED);
  ItemComponent* ic = go->GetComponent<ItemComponent>();
  ic->Data.Amount = 3;

  Inventory.Add(go);

  go = ItemsFactory::Instance().CreateFood(0, 0, FoodType::IRON_RATIONS, ItemPrefix::UNCURSED);
  ic = go->GetComponent<ItemComponent>();
  ic->Data.Amount = 1;
  ic->Data.IsIdentified = true;

  Inventory.Add(go);

  go = ItemsFactory::Instance().CreateRepairKit(0, 0, 30, ItemPrefix::BLESSED);

  Inventory.Add(go);
}

void Player::SetThiefDefaultItems()
{
  Money = 500;

  auto go = ItemsFactory::Instance().CreateHealingPotion(ItemPrefix::UNCURSED);
  Inventory.Add(go);

  go = ItemsFactory::Instance().CreateManaPotion(ItemPrefix::UNCURSED);
  Inventory.Add(go);
}

void Player::SetArcanistDefaultItems()
{
  Money = 0;

  auto go = ItemsFactory::Instance().CreateManaPotion(ItemPrefix::BLESSED);
  auto c = go->GetComponent<ItemComponent>();
  ItemComponent* ic = static_cast<ItemComponent*>(c);
  ic->Data.Amount = 5;

  Inventory.Add(go);

  go = ItemsFactory::Instance().CreateReturner(0, 0, 3, ItemPrefix::UNCURSED);
  Inventory.Add(go);

  go = ItemsFactory::Instance().CreateScroll(0, 0, SpellType::MANA_SHIELD, ItemPrefix::BLESSED);
  Inventory.Add(go);
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

  if (weapon->Data.HasBonus(ItemBonusType::INDESTRUCTIBLE))
  {
    return false;
  }

  weapon->Data.Durability.AddMin(-1);

  return true;
}

void Player::ApplyBonuses(ItemComponent* itemRef)
{
  for (auto& i : itemRef->Data.Bonuses)
  {
    if (i.Period > 0)
    {
      i.EffectCounter = 0;
    }

    ApplyBonus(itemRef, i);
  }
}

void Player::ApplyBonus(ItemComponent* itemRef, const ItemBonusStruct& bonus)
{
  switch (bonus.Type)
  {
    case ItemBonusType::STR:
    case ItemBonusType::DEF:
    case ItemBonusType::MAG:
    case ItemBonusType::RES:
    case ItemBonusType::SKL:
    case ItemBonusType::SPD:
      _attributesRefsByBonus.at(bonus.Type).AddModifier(itemRef->OwnerGameObject->ObjectId(), bonus.BonusValue);
      break;

    case ItemBonusType::HP:
    case ItemBonusType::MP:
      _rangedAttributesRefsByBonus.at(bonus.Type).Max().AddModifier(itemRef->OwnerGameObject->ObjectId(), bonus.BonusValue);
      _rangedAttributesRefsByBonus.at(bonus.Type).CheckOverflow();
      break;

    case ItemBonusType::VISIBILITY:
      VisibilityRadius.AddModifier(itemRef->OwnerGameObject->ObjectId(), bonus.BonusValue);
      break;

    case ItemBonusType::REGEN:
    case ItemBonusType::REFLECT:
    case ItemBonusType::MANA_SHIELD:
    case ItemBonusType::INVISIBILITY:
    case ItemBonusType::THORNS:
    case ItemBonusType::TELEPATHY:
      AddEffect(bonus);
      break;
  }
}

void Player::UnapplyBonuses(ItemComponent* itemRef)
{
  for (auto& i : itemRef->Data.Bonuses)
  {
    if (i.Period > 0)
    {
      i.EffectCounter = 0;
    }

    UnapplyBonus(itemRef, i);
  }
}

void Player::UnapplyBonus(ItemComponent* itemRef, const ItemBonusStruct& bonus)
{
  switch (bonus.Type)
  {
    case ItemBonusType::STR:
    case ItemBonusType::DEF:
    case ItemBonusType::MAG:
    case ItemBonusType::RES:
    case ItemBonusType::SKL:
    case ItemBonusType::SPD:
      _attributesRefsByBonus.at(bonus.Type).RemoveModifier(itemRef->OwnerGameObject->ObjectId());
      break;

    case ItemBonusType::HP:
    case ItemBonusType::MP:
      _rangedAttributesRefsByBonus.at(bonus.Type).Max().RemoveModifier(itemRef->OwnerGameObject->ObjectId());
      _rangedAttributesRefsByBonus.at(bonus.Type).CheckOverflow();
      break;

    case ItemBonusType::VISIBILITY:
      VisibilityRadius.RemoveModifier(itemRef->OwnerGameObject->ObjectId());
      break;

    case ItemBonusType::REGEN:
    case ItemBonusType::REFLECT:
    case ItemBonusType::MANA_SHIELD:
    case ItemBonusType::INVISIBILITY:
    case ItemBonusType::THORNS:
    case ItemBonusType::TELEPATHY:
      RemoveEffect(bonus);
      break;
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

  // One-time deletion, can leave "forward" loop iteration
  for (size_t i = 0; i < Inventory.Contents.size(); i++)
  {
    auto c = Inventory.Contents[i]->GetComponent<ItemComponent>();
    ItemComponent* ic = static_cast<ItemComponent*>(c);

    if (ic->Data.ItemTypeHash == typeHash && ic->Data.IsEquipped)
    {
      UnapplyBonuses(ic);
      Inventory.Contents.erase(Inventory.Contents.begin() + i);
      break;
    }
  }

  EquipmentByCategory[ec][0] = nullptr;
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

void Player::RememberItem(ItemComponent* itemRef, const std::string& effect)
{
  std::string objName = itemRef->Data.UnidentifiedName;

  int existingIndex = -1;
  for (auto& kvp : _useIdentifiedItemsByIndex)
  {
    if (kvp.second.first == objName)
    {
      // If value is not '?unsure?', do nothing,
      // the item is already use-identified.
      if (kvp.second.second != GlobalConstants::UnidentifiedEffectText)
      {
        return;
      }
      else
      {
        // Otherwise break the loop and go down ahead and
        // replace old '?unsure?' value with proper text
        // passed in 'effect' variable here.
        existingIndex = kvp.first;
        break;
      }
    }
  }

  if (existingIndex == -1)
  {
    _useIdentifiedItemsByIndex[_useIdentifiedMapSortingIndex] = { objName, effect };
    _useIdentifiedMapSortingIndex++;
  }
  else
  {
    _useIdentifiedItemsByIndex[existingIndex] = { objName, effect };
  }
}

void Player::AddExtraItems()
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

      go = ItemsFactory::Instance().CreateNote("Wanted Poster", text);
      Inventory.Add(go);
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

      go = ItemsFactory::Instance().CreateNote("Leave Warrant", text);
      Inventory.Add(go);
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

      go = ItemsFactory::Instance().CreateNote("Orders", text);
      Inventory.Add(go);
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

// NOTE: unused for now
bool Player::AreEnemiesInRange()
{
  bool ret = false;

  auto res = Map::Instance().GetActorsInRange(VisibilityRadius.Get());
  for (auto& i : res)
  {
    AIComponent* aic = i->GetComponent<AIComponent>();
    if (aic != nullptr)
    {
      if (aic->CurrentModel != nullptr && aic->CurrentModel->IsAgressive)
      {
        bool visible = Map::Instance().IsObjectVisible({ PosX, PosY }, { i->PosX, i->PosY });
        if (visible)
        {
          ret = true;
          break;
        }
      }
    }
  }

  return ret;
}

bool Player::RecallItem(ItemComponent* itemRef)
{
  auto& objName = itemRef->Data.UnidentifiedName;

  for (auto& kvp : _useIdentifiedItemsByIndex)
  {
    if (kvp.second.first == objName)
    {
      return true;
    }
  }

  return false;
}


int Player::GetDamageAbsorbtionValue(bool magic)
{
  int res = 0;

  ItemBonusType t = magic ? ItemBonusType::MAG_ABSORB : ItemBonusType::DMG_ABSORB;

  for (auto& kvp : EquipmentByCategory)
  {
    for (ItemComponent* item : kvp.second)
    {
      if (item != nullptr && item->Data.HasBonus(t))
      {
        ItemBonusStruct* ibs = item->Data.GetBonus(t);
        res += ibs->BonusValue;
      }
    }
  }

  return res;
}

std::vector<ItemComponent*> Player::GetItemsWithBonus(const ItemBonusType& bonusType)
{
  std::vector<ItemComponent*> res;

  for (auto& item : EquipmentByCategory)
  {
    for (auto& e : item.second)
    {
      if (e != nullptr && e->Data.HasBonus(bonusType))
      {
        res.push_back(e);
      }
    }
  }

  return res;
}
