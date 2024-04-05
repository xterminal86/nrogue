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
#include "gid-generator.h"

void Player::Init()
{
  _objectId = GID::Instance().GenerateGlobalId();

#ifdef DEBUG_BUILD
  GameObjectsById[_objectId] = this;
#endif

  Type = GameObjectType::PLAYER;

  Image = '@';

  IsLiving = true;

  #ifdef USE_SDL
  Image = GlobalConstants::CP437IndexByType[NameCP437::FACE_2];
  #endif

  FgColor = Colors::PlayerColor;
  BgColor = Colors::BlackColor;

  Attrs.ActionMeter = GlobalConstants::TurnReadyValue;

  _components.clear();
  _activeEffects.clear();

  Inventory = AddComponent<ContainerComponent>();
  Equipment = AddComponent<EquipmentComponent>(Inventory);

  SetAttributes();
  SetDefaultItems();
  SetDefaultEquipment();
  SetDefaultSkills();

  _previousCell = Map::Instance().CurrentLevel->MapArray[PosX][PosY].get();
  _currentCell  = Map::Instance().CurrentLevel->MapArray[PosX][PosY].get();

  _currentCell->Occupied = true;

  int expToLvlUp = Util::GetExpForNextLevel(Attrs.Lvl.Get());

  Attrs.Exp.Reset(0);
  Attrs.Exp.SetMax(expToLvlUp);

  DistanceField.Init(this, kDistanceFieldRadius);

  // FIXME: debug

  //Money = 0;
  //Money = 100000;
  //Attrs.HP.Reset(1);
  //Attrs.HungerRate.Set(0);
}

// =============================================================================

void Player::Draw()
{
  auto& mapRef = Map::Instance().CurrentLevel;

  //
  // If game object has black bg color,
  // replace it with current floor color.
  //
  uint32_t bgColor = BgColor;

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

// =============================================================================

bool Player::TryToMeleeAttack(int dx, int dy)
{
  _attackDir.X = dx;
  _attackDir.Y = dy;

  if (IsSwimming())
  {
    return false;
  }

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

// =============================================================================

bool Player::Move(int dx, int dy)
{
  auto cell = Map::Instance().CurrentLevel->MapArray[PosX + dx][PosY + dy].get();
  auto staticObject = Map::Instance().CurrentLevel->StaticMapObjects[PosX + dx][PosY + dy].get();

  bool moveOk = false;
  bool passByNPC = false;

  bool isFlying = HasEffect(ItemBonusType::LEVITATION);

  if (!cell->Blocking || isFlying)
  {
    //
    // Occupied is set only by actors, so if actor is present on this cell,
    // it can be walked into so there's no need to check for static object there.
    //
    if (cell->Occupied)
    {
      auto actor = Map::Instance().GetActorAtPosition(PosX + dx, PosY + dy);
      if (actor != nullptr)
      {
        //
        // Do not return true, since it will cause
        // MapOffsetY and MapOffsetX to be incremented in MainState,
        // but we manually call AdjustCamera() inside this method in
        // SwitchPlaces().
        //
        passByNPC = PassByNPC(actor);
      }
    }
    else if (staticObject != nullptr)
    {
      if (staticObject->Blocking)
      {
        auto dc = staticObject->GetComponent<DoorComponent>();

        //
        // Automatically interact with door if it's closed.
        //
        if (dc != nullptr)
        {
          auto ir = dc->Interact();
          if (ir.first == InteractionResult::SUCCESS)
          {
            FinishTurn();
          }
        }
      }
      else
      {
        //
        // If static object is no longer blocking (e.g. door was opened)
        // just move there.
        //
        MoveGameObject(dx, dy);
        moveOk = true;
      }
    }
    else
    {
      //
      // Some code duplication couldn't be avoided. :-(
      //
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

// =============================================================================

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

// =============================================================================

void Player::CheckVisibility()
{
  int tw = Printer::TerminalWidth;
  int th = Printer::TerminalHeight;

  //
  // Update map around player.
  //
  auto& map = Map::Instance().CurrentLevel->MapArray;
  auto& staticObjects = Map::Instance().CurrentLevel->StaticMapObjects;

  //
  // FIXME: some objects can modify visibility radius
  //
  int radius = (map[PosX][PosY]->ObjectName == Strings::TileNames::TreeText)
              ? VisibilityRadius.Get() / 4
              : VisibilityRadius.Get();

  auto mapCells = Util::GetRectAroundPoint(PosX,
                                           PosY,
                                           tw / 2,
                                           th / 2,
                                           Map::Instance().CurrentLevel->MapSize);

#ifdef DEBUG_BUILD
  for (auto& cell : mapCells)
  {
    map[cell.X][cell.Y]->Visible = ToggleFogOfWar;

    if (staticObjects[cell.X][cell.Y] != nullptr)
    {
      staticObjects[cell.X][cell.Y]->Visible = ToggleFogOfWar;
    }
  }
#else
  for (auto& cell : mapCells)
  {
    map[cell.X][cell.Y]->Visible = false;

    if (staticObjects[cell.X][cell.Y] != nullptr)
    {
      staticObjects[cell.X][cell.Y]->Visible = false;
    }
  }
#endif

  //
  // Update visibility around player.
  //
  for (auto& cell : mapCells)
  {
    double d = Util::LinearDistance(PosX, PosY, cell.X, cell.Y);

    if (d > (double)radius)
    {
      continue;
    }

    //
    // Bresenham driven Field of Vision
    //
    // From what I understand it's considered most shitty implementation of one,
    // but it'll do for now.
    //
    auto line = Util::BresenhamLine(PosX, PosY, cell.X, cell.Y);
    for (auto& point : line)
    {
      if (point.X == PosX && point.Y == PosY)
      {
        continue;
      }

      DiscoverCell(point.X, point.Y);

      bool mapBlock = map[point.X][point.Y]->BlocksSight;
      bool staticBlock = (staticObjects[point.X][point.Y] != nullptr
                       && staticObjects[point.X][point.Y]->BlocksSight);

      if (mapBlock || staticBlock)
      {
        break;
      }
    }
  }
}

// =============================================================================

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

    curLvl->FowLayer[x][y].Image   = staticObjects[x][y]->Image;
    curLvl->FowLayer[x][y].FowName = Util::GetFowName(staticObjects[x][y].get());
  }
  else
  {
    curLvl->FowLayer[x][y].Image   = (map[x][y]->Image == ' ')
                                     ? '.'
                                     : map[x][y]->Image;
    curLvl->FowLayer[x][y].FowName = Util::GetFowName(map[x][y].get());
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

// =============================================================================

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

// =============================================================================

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

// =============================================================================

void Player::SetThiefAttrs()
{
  Attrs.Spd.Talents = 3;
  Attrs.Skl.Talents = 3;
  Attrs.Def.Talents = 1;
  Attrs.HP.Talents = 1;

  Attrs.Def.Set(1);
  Attrs.Skl.Set(3);
  Attrs.Spd.Set(3);

  Attrs.HP.Reset(15);

  Attrs.HungerRate.Set(2000);
  Attrs.HungerSpeed.Set(1);

  HealthRegenTurns = 60;
}

// =============================================================================

void Player::SetArcanistAttrs()
{
  Attrs.Mag.Talents = 3;
  Attrs.Res.Talents = 3;
  Attrs.MP.Talents = 3;

  Attrs.Mag.Set(3);
  Attrs.Res.Set(3);

  Attrs.HP.Reset(5);
  Attrs.MP.Reset(30);

  Attrs.HungerRate.Set(3000);
  Attrs.HungerSpeed.Set(1);

  HealthRegenTurns = 80;
}

// =============================================================================

void Player::SetCustomClassAttrs()
{
  GameState* stateRef = Application::Instance().GetGameStateRefByName(GameStates::CUSTOM_CLASS_STATE);
  CustomClassState* ccs = static_cast<CustomClassState*>(stateRef);
  ccs->InitPlayerAttributes(this);
}

// =============================================================================

void Player::SetAttackDir(const Position& dir)
{
  _attackDir = dir;
}

// =============================================================================

void Player::SetKnockBackDir(const Position& dir)
{
  _knockBackDir = dir;
}

// =============================================================================

void Player::SetDefaultEquipment()
{
  std::vector<GameObject*> weaponAndArmorToEquip;

  GameObject* weapon = nullptr;
  GameObject* armor = nullptr;

  switch (GetClass())
  {
    case PlayerClass::THIEF:
    {
      weapon = ItemsFactory::Instance().CreateRangedWeapon(0, 0, RangedWeaponType::SHORT_BOW, ItemPrefix::UNCURSED, ItemQuality::NORMAL);
      Inventory->Add(weapon);

      GameObject* arrows = ItemsFactory::Instance().CreateArrows(0, 0, ArrowType::ARROWS, ItemPrefix::BLESSED, 60);
      Inventory->Add(arrows);

      GameObject* dagger = ItemsFactory::Instance().CreateMeleeWeapon(0, 0, WeaponType::DAGGER, ItemPrefix::UNCURSED, ItemQuality::NORMAL);
      Inventory->Add(dagger);

      weaponAndArmorToEquip.push_back(arrows);
      weaponAndArmorToEquip.push_back(weapon);
    }
    break;

    case PlayerClass::SOLDIER:
    {
      weapon = ItemsFactory::Instance().CreateMeleeWeapon(0, 0, WeaponType::SHORT_SWORD, ItemPrefix::UNCURSED, ItemQuality::NORMAL);
      Inventory->Add(weapon);

      armor = ItemsFactory::Instance().CreateArmor(0, 0, ArmorType::PADDING, ItemPrefix::UNCURSED, ItemQuality::NORMAL);
      Inventory->Add(armor);

      weaponAndArmorToEquip.push_back(weapon);
      weaponAndArmorToEquip.push_back(armor);
    }
    break;

    case PlayerClass::ARCANIST:
    {
      weapon = ItemsFactory::Instance().CreateMeleeWeapon(0, 0, WeaponType::STAFF, ItemPrefix::UNCURSED, ItemQuality::NORMAL);
      Inventory->Add(weapon);

      auto wand = ItemsFactory::Instance().CreateWand(0, 0, WandMaterials::EBONY_3, SpellType::MAGIC_MISSILE, ItemPrefix::BLESSED, ItemQuality::NORMAL);
      Inventory->Add(wand);

      weaponAndArmorToEquip.push_back(weapon);
    }
    break;

    case PlayerClass::CUSTOM:
    {
      weapon = ItemsFactory::Instance().CreateRandomWeapon();
      Inventory->Add(weapon);

      armor = ItemsFactory::Instance().CreateRandomArmor();
      Inventory->Add(armor);

      auto acc = ItemsFactory::Instance().CreateRandomAccessory(0, 0);
      Inventory->Add(acc);

      auto potion = ItemsFactory::Instance().CreateRandomPotion();
      Inventory->Add(potion);

      auto gem = ItemsFactory::Instance().CreateGem(0, 0);
      Inventory->Add(gem);
    }
    break;
  }

  // Since equipping produces message in log,
  // we explicitly delete it here.
  for (auto& i : weaponAndArmorToEquip)
  {
    ItemComponent* ic = i->GetComponent<ItemComponent>();
    Equipment->Equip(ic);

    auto it = Printer::Instance().Messages().begin();
    Printer::Instance().Messages().erase(it);
  }
}

// =============================================================================

//
// 'what' is either actor or GameObject, 'with' is a weapon (i.e. arrow)
//
void Player::RangedAttack(GameObject* what, ItemComponent* with)
{
  ItemComponent* weapon = Equipment->EquipmentByCategory[EquipmentCategory::WEAPON][0];
  ItemComponent* arrows = Equipment->EquipmentByCategory[EquipmentCategory::SHIELD][0];

  int dmg = Util::CalculateDamageValue(this, what, weapon, false);

  //
  // If it's not the ground GameObject
  //
  if (what->Type != GameObjectType::GROUND)
  {
    AIComponent* aic = what->GetComponent<AIComponent>();

    if (aic != nullptr
     && weapon != nullptr
     && weapon->Data.HasBonus(ItemBonusType::KNOCKBACK))
    {
      int tiles = weapon->Data.GetBonus(ItemBonusType::KNOCKBACK)->BonusValue;
      Util::KnockBack(this, what, _knockBackDir, tiles);
    }

    //
    // KnockBack() could set receiver HP to 0
    //
    if (what->HasNonZeroHP())
    {
      bool ignoreArmor = (weapon != nullptr && weapon->Data.HasBonus(ItemBonusType::IGNORE_ARMOR));

      bool succ = what->ReceiveDamage(this, dmg, false, ignoreArmor, false, false);

      if (succ
       && weapon != nullptr
       && weapon->Data.HasBonus(ItemBonusType::LEECH)
       && what->IsLiving)
      {
        Attrs.HP.AddMin(dmg);
      }

      if (what->IsDestroyed)
      {
        ProcessKill(what);
      }
    }
  }
  else if (what->Type != GameObjectType::DEEP_WATER
        || what->Type != GameObjectType::LAVA
        || what->Type != GameObjectType::CHASM)
  {
    // Create arrow object on the cell where it landed
    ItemComponent* arrow = GameObjectsFactory::Instance().CloneItem(Equipment->EquipmentByCategory[EquipmentCategory::SHIELD][0]);
    arrow->OwnerGameObject->PosX = what->PosX;
    arrow->OwnerGameObject->PosY = what->PosY;
    arrow->Data.Amount = 1;
    arrow->Data.IsEquipped = false;
    Map::Instance().PlaceGameObject(arrow->OwnerGameObject);
  }

  arrows->Data.Amount--;

  if (arrows->Data.Amount <= 0)
  {
    arrows->Break(this);
  }

  std::string logMsg = Util::TryToDamageEquipment(this, weapon, -1);
  if (!logMsg.empty())
  {
    Printer::Instance().AddMessage(logMsg);
  }
}

// =============================================================================

//
// 'what' is either actor or GameObject, 'with' is a wand
//
void Player::MagicAttack(GameObject* what, ItemComponent* with)
{
  with->Data.Amount--;

  SpellInfo si = with->Data.SpellHeld;

  auto baseDamagePair = si.SpellBaseDamage;

  int bonus = Attrs.Mag.Get();

  int centralDamage = Util::RollDices(baseDamagePair.first,
                                      baseDamagePair.second);

  centralDamage += bonus;

  //
  // TODO: cursed wands side-effects?
  //
  // NOTE: double-check with spells list in MainState::ProcessWand()
  // so that nothing was accidentally forgotten to be added
  // for handling here.
  //
  switch (with->Data.SpellHeld.SpellType_)
  {
    case SpellType::NONE:
      Printer::Instance().AddMessage(Strings::NoActionText);
      break;

    case SpellType::STRIKE:
      ProcessMagicAttack(what, with, centralDamage, false);
      break;

    case SpellType::MAGIC_MISSILE:
    case SpellType::FROST:
    case SpellType::LIGHTNING:
      ProcessMagicAttack(what, with, centralDamage, true);
      break;

    case SpellType::FIREBALL:
      Map::Instance().ProcessAoEDamage(what, with, centralDamage, true);
      break;

    case SpellType::TELEPORT:
    {
      if (!Map::Instance().CurrentLevel->MysteriousForcePresent)
      {
        std::string msg = Util::ProcessTeleport(what);
        Printer::Instance().AddMessage(msg);
      }
      else
      {
        Printer::Instance().AddMessage(Strings::NoActionText);
      }
    }
    break;

    default:
      DebugLog("[WAR] Player::MagicAttack() spell %i not handled!", (int)with->Data.SpellHeld.SpellType_);
      break;
  }
}

// =============================================================================

void Player::ProcessMagicAttack(GameObject* target,
                                ItemComponent* weapon,
                                int damage,
                                bool againstRes)
{
  Position p = target->GetPosition();

  auto actor = Map::Instance().GetActorAtPosition(p.X, p.Y);
  if (actor != nullptr)
  {
    bool damageDone = Util::TryToDamageObject(actor, this, damage, againstRes).first;
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
    //
    // Magic attack should probably hit everything,
    // since it's, well, magical. You can't really control it.
    //

    //
    // Check items first.
    //
    auto mapObjs = Map::Instance().GetGameObjectsAtPosition(p.X, p.Y);
    for (auto& obj : mapObjs)
    {
      Util::TryToDamageObject(obj, this, damage, againstRes);
    }

    //
    // If nothing is lying in the doorway, for example, damage door.
    //
    if (mapObjs.empty())
    {
      auto so = Map::Instance().GetStaticGameObjectAtPosition(p.X, p.Y);
      if (so != nullptr)
      {
        Util::TryToDamageObject(so, this, damage, againstRes);
      }
    }
  }
}

// =============================================================================

void Player::MeleeAttack(GameObject* what, bool alwaysHit)
{
  int hitChance = Util::CalculateHitChanceMelee(this, what);

  #ifdef DEBUG_BUILD
  auto logMsg = Util::StringFormat("Player (SKL %i, LVL %i) attacks %s (SKL: %i, LVL %i): chance = %i",
                                   Attrs.Skl.Get(),
                                   Attrs.Lvl.Get(),
                                   what->ObjectName.data(),
                                   what->Attrs.Skl.Get(),
                                   what->Attrs.Lvl.Get(),
                                   hitChance);

  Logger::Instance().Print(logMsg);
  #endif

  bool hitLanded = alwaysHit ? true : Util::Rolld100(hitChance);
  if (!hitLanded)
  {
    Application::Instance().DisplayAttack(what,
                                          GlobalConstants::DisplayAttackDelayMs,
                                          "You missed",
                                          Colors::WhiteColor);
  }
  else
  {
    Application::Instance().DisplayAttack(what,
                                          GlobalConstants::DisplayAttackDelayMs,
                                          std::string(),
                                          Colors::RedColor);

    ItemComponent* weapon = Equipment->EquipmentByCategory[EquipmentCategory::WEAPON][0];

    bool isRanged = false;
    if (weapon != nullptr)
    {
      isRanged = (weapon->Data.ItemType_ == ItemType::RANGED_WEAPON
               || weapon->Data.ItemType_ == ItemType::WAND);
    }

    AIComponent* aic = what->GetComponent<AIComponent>();

    if (aic != nullptr
     && weapon != nullptr
     && weapon->Data.HasBonus(ItemBonusType::KNOCKBACK))
    {
      _knockBackDir = _attackDir;
      int tiles = weapon->Data.GetBonus(ItemBonusType::KNOCKBACK)->BonusValue;
      Util::KnockBack(this, what, _knockBackDir, tiles);
    }

    //
    // KnockBack() could set receiver HP to 0,
    // so to prevent double kill and double loot roll
    // check IsAlive().
    //
    // Check for Type is for ability to attack walls
    // (with a pickaxe to mine, for example).
    //
    if ((what->Type == GameObjectType::PICKAXEABLE) || what->HasNonZeroHP())
    {
      int dmg = Util::CalculateDamageValue(this, what, weapon, isRanged);

      if (what->Corporeal == false
       && weapon != nullptr
       && weapon->Data.Prefix != ItemPrefix::BLESSED)
      {
        dmg = 0;
      }

      ProcessMeleeAttack(weapon, what, dmg);
    }
  }

  Attrs.Hunger += Attrs.HungerSpeed.Get() * 2;
}

// =============================================================================

void Player::ProcessMeleeAttack(ItemComponent* weapon,
                                GameObject* defender,
                                int damageToInflict)
{
  bool shouldTearDownWall = false;
  bool hasLeech = false;
  bool ignoreArmor = false;
  bool pickaxeEquipped = false;

  //
  // GameObject default type is HARMLESS to avoid it being pickaxed away.
  //
  bool isPickaxeable = (defender->Type == GameObjectType::PICKAXEABLE);
  bool isOnBorder    = IsGameObjectBorder(defender);

  if (weapon != nullptr)
  {
    //
    // Melee attack with ranged weapon in hand ignores durability,
    // since it is considered a punch.
    //
    bool isRanged = (weapon->Data.ItemType_ == ItemType::RANGED_WEAPON
                  || weapon->Data.ItemType_ == ItemType::WAND);

    ignoreArmor = weapon->Data.HasBonus(ItemBonusType::IGNORE_ARMOR);

    pickaxeEquipped = (weapon->Data.WeaponType_ == WeaponType::PICKAXE);

    if (!isRanged)
    {
      if (weapon->Data.HasBonus(ItemBonusType::LEECH))
      {
        hasLeech = true;
      }

      bool defHasArmor = false;

      EquipmentComponent* ec = defender->GetComponent<EquipmentComponent>();
      if (ec != nullptr)
      {
        ItemComponent* armor = ec->EquipmentByCategory[EquipmentCategory::TORSO][0];
        defHasArmor = (armor != nullptr);
      }

      //
      // Do not damage weapon unless you hit armor, using a pickaxe
      // or doing dumb shit.
      //
      bool shouldDamageWeapon = (defHasArmor
                             || (pickaxeEquipped && isPickaxeable)
                             //
                             // Use function from different domain to check
                             // against specific set of item types.
                             //
                             || !Util::ShouldAwardExp(defender->Type));

      if (shouldDamageWeapon)
      {
        std::string logMsg = Util::TryToDamageEquipment(this, weapon, -1);
        if (!logMsg.empty())
        {
          Printer::Instance().AddMessage(logMsg);
        }
      }
    }
  }

  shouldTearDownWall = (pickaxeEquipped
                     && isPickaxeable
                     && !isOnBorder
                     && !defender->Attrs.Indestructible);

  if (shouldTearDownWall)
  {
    defender->Attrs.HP.SetMin(0);
    defender->IsDestroyed = true;

    auto msg = Util::StringFormat("You tear down the %s", defender->ObjectName.data());
    Printer::Instance().AddMessage(msg);
  }
  else
  {
    bool succ = defender->ReceiveDamage(this,
                                        damageToInflict,
                                        false,
                                        ignoreArmor,
                                        false,
                                        false);

    if (succ && hasLeech && defender->IsLiving)
    {
      auto b = weapon->Data.GetBonus(ItemBonusType::LEECH);
      double fract = (double)b->BonusValue * 0.01;
      int dmgToHp = (int)((double)damageToInflict * fract);
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

// =============================================================================

bool Player::IsGameObjectBorder(GameObject* go)
{
  auto& lvl = Map::Instance().CurrentLevel;

  return (go->PosX == 0
       || go->PosY == 0
       || go->PosX == lvl->MapSize.X - 2
       || go->PosY == lvl->MapSize.Y - 2);
}

// =============================================================================

bool Player::ReceiveDamage(GameObject* from,
                           int amount,
                           bool isMagical,
                           bool ignoreArmor,
                           bool directDamage,
                           bool suppressLog)
{
#ifdef DEBUG_BUILD
  if (GodMode)
  {
    return false;
  }
#endif

  if (directDamage)
  {
    Attrs.HP.AddMin(-amount);
    return true;
  }

  std::vector<std::string> logMsgs;

  int dmgReturned = 0;

  if (isMagical)
  {
    auto str = Util::ProcessMagicalDamage(this, from, amount);
    if (!str.empty())
    {
      logMsgs.push_back(str);
    }
  }
  else
  {
    logMsgs = Util::ProcessPhysicalDamage(this, from, amount, ignoreArmor);
    dmgReturned = Util::ProcessThorns(this, amount);
  }

  if (!suppressLog && !logMsgs.empty())
  {
    for (auto& m : logMsgs)
    {
      Printer::Instance().AddMessage(m);
    }
  }

  if (dmgReturned != 0 && from != nullptr)
  {
    auto thornsLogMsg = Util::StringFormat("@ => %s (%i)",
                                           Util::GetGameObjectDisplayCharacter(from).data(),
                                           dmgReturned);

    Printer::Instance().AddMessage(thornsLogMsg);

    from->ReceiveDamage(this, dmgReturned, true, true, true, false);
  }

  return true;
}

// =============================================================================

void Player::AwardExperience(int amount)
{
  int amnt = amount;

  std::string msg;

  if (amount > 0)
  {
    amnt = amount * (Attrs.Exp.Talents + 1);
    msg = Util::StringFormat("+%i EXP", amount);
  }
  else
  {
    msg = Util::StringFormat("%i EXP", amount);
  }

  Printer::Instance().AddMessage(msg);

  Attrs.Exp.AddMin(amnt);

  if (Attrs.Exp.Min().Get() >= Attrs.Exp.Max().Get())
  {
    int overflow = amnt - Attrs.Exp.Max().Get();

    LevelUp();

    int expToLvlUp = Util::GetExpForNextLevel(Attrs.Lvl.Get());

    overflow = Util::Clamp(overflow, 0, expToLvlUp - 1);

    Attrs.Exp.SetMin(overflow);
    Attrs.Exp.SetMax(expToLvlUp);
  }
  else if (amnt < 0
        && Attrs.Lvl.Get() != 1
        && Attrs.Exp.Min().Get() <= 0)
  {
    LevelDown();

    int expToLvlUp = Util::GetExpForNextLevel(Attrs.Lvl.Get());

    int underflow = expToLvlUp + amnt;

    underflow = Util::Clamp(underflow, 0, expToLvlUp - 1);

    Attrs.Exp.SetMin(underflow);
    Attrs.Exp.SetMax(expToLvlUp);
  }
  //
  // Player becomes ghost himself.
  //
  else if (amnt < 0
        && Attrs.Lvl.Get() == 1
        && Attrs.Exp.Min().Get() == 0)
  {
    Attrs.HP.Reset(0);
  }
}

// =============================================================================

void Player::LevelUpSilent()
{
  GameObject::LevelUp();
}

// =============================================================================

void Player::LevelUp(int baseHpOverride)
{
  GameObject::LevelUp();

  for (auto& kvp : _statRaisesMap)
  {
    kvp.second.second = 0;
  }

  PrintLevelUpResultsToLog(true);

  auto res = GetPrettyLevelUpText();
  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT,
                                         "Level Up!",
                                         res,
                                         0x888800,
                                         0x000044);
}

// =============================================================================

void Player::LevelDown()
{
  GameObject::LevelDown();

  PrintLevelUpResultsToLog(false);

  auto res = GetPrettyLevelUpText();
  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT,
                                         "Level DOWN!",
                                         res,
                                         Colors::RedColor,
                                         0x000044);
}

// =============================================================================

void Player::LevelDownSilent()
{
  GameObject::LevelDown();
}

// =============================================================================

void Player::PrintLevelUpResultsToLog(bool reallyUp)
{
  int getDataFrom = reallyUp
                  ? Attrs.Lvl.Get()
                  : (Attrs.Lvl.Get() + 1);

  auto& data = _levelUpHistory[getDataFrom];

  _statRaisesMap[PlayerStats::STR].second = reallyUp
                        ? data[PlayerStats::STR]
                        : -data[PlayerStats::STR];

  _statRaisesMap[PlayerStats::DEF].second = reallyUp
                        ? data[PlayerStats::DEF]
                        : -data[PlayerStats::DEF];

  _statRaisesMap[PlayerStats::MAG].second = reallyUp
                        ? data[PlayerStats::MAG]
                        : -data[PlayerStats::MAG];

  _statRaisesMap[PlayerStats::RES].second = reallyUp
                        ? data[PlayerStats::RES]
                        : -data[PlayerStats::RES];

  _statRaisesMap[PlayerStats::SPD].second = reallyUp
                        ? data[PlayerStats::SPD]
                        : -data[PlayerStats::SPD];

  _statRaisesMap[PlayerStats::SKL].second = reallyUp
                        ? data[PlayerStats::SKL]
                        : -data[PlayerStats::SKL];

  _statRaisesMap[PlayerStats::HP].second = reallyUp
                        ? data[PlayerStats::HP]
                        : -data[PlayerStats::HP];

  _statRaisesMap[PlayerStats::MP].second = reallyUp
                        ? data[PlayerStats::MP]
                        : -data[PlayerStats::MP];

  std::vector<std::string> messages;

  messages.reserve(10);

  if (reallyUp)
  {
    messages.push_back("You have gained a level!");
  }

  for (auto& kvp : _statRaisesMap)
  {
    if (kvp.second.second != 0)
    {
      std::string num = (kvp.second.second > 0)
                       ? Util::StringFormat("+%i", kvp.second.second)
                       : Util::StringFormat("%i", kvp.second.second);

      messages.push_back(Util::StringFormat("%s %s", kvp.second.first.data(), num.data()));
    }
  }

  for (auto& msg : messages)
  {
    Printer::Instance().AddMessage(msg);
  }

  if (!reallyUp)
  {
    Printer::Instance().AddMessage("You have LOST a level!");
  }
}

// =============================================================================

void Player::ProcessKill(GameObject* monster)
{
  //
  // Try to generate loot from the kill itself.
  //
  if (Util::IsFunctionValid(monster->GenerateLootFunction))
  {
    monster->GenerateLootFunction();
  }

  //
  // Do not award experience for nothing.
  //
  if (Util::ShouldAwardExp(monster->Type))
  {
    int exp = monster->Attrs.Rating();
    exp = Util::Clamp(exp, 0, GlobalConstants::AwardedExpMax);

    if (exp != 0)
    {
      AwardExperience(exp);
    }

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

// =============================================================================

void Player::WaitForTurn()
{
  GameObject::WaitForTurn();
  ProcessEffectsPlayer();
}

// =============================================================================

void Player::SetDestroyed()
{
  Image = '%';
  FgColor = Colors::PlayerColor;
  BgColor = Colors::RedColor;
  IsDestroyed = true;
}

// =============================================================================

std::vector<std::string> Player::GetPrettyLevelUpText()
{
  std::vector<std::string> levelUpResults;

  std::string mbStr;
  for (auto& i : _mainAttributesByStatName)
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

  //
  // Try to make everything look pretty
  //
  // NOTE: probably lots of shitcode anyway
  //

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
  for (auto& i : _mainAttributesByStatName)
  {
    auto kvp = i.second;

    std::string addition;

    if (_statRaisesMap[i.first].second >= 0)
    {
      addition = Util::StringFormat("  +%i", _statRaisesMap[i.first].second);
    }
    else
    {
      addition = Util::StringFormat("  -%i", std::abs(_statRaisesMap[i.first].second));
    }

    levelUpResults[index] += addition;
    index++;
  }

  //
  // Take into account empty string between stats and hitpoints.
  //
  index++;

  std::string addition;

  if (_statRaisesMap[PlayerStats::HP].second >= 0)
  {
    addition = Util::StringFormat("  +%i", _statRaisesMap[PlayerStats::HP].second);
  }
  else
  {
    addition = Util::StringFormat("  -%i", std::abs(_statRaisesMap[PlayerStats::HP].second));
  }

  levelUpResults[index] += addition;
  index++;

  if (_statRaisesMap[PlayerStats::MP].second >= 0)
  {
    addition = Util::StringFormat("  +%i", _statRaisesMap[PlayerStats::MP].second);
  }
  else
  {
    addition = Util::StringFormat("  -%i", std::abs(_statRaisesMap[PlayerStats::MP].second));
  }

  levelUpResults[index] += addition;
  index++;

  return levelUpResults;
}

// =============================================================================

void Player::FinishTurn()
{
  ConsumeEnergy();
  ProcessNaturalRegenMP();
  ProcessHunger();
  ProcessStarvation();
  ProcessEffectsPlayer();
  ProcessItemsEffects();

  if (IsOnDangerousTile())
  {
    Attrs.HP.Reset(0);
  }

  //
  // So that triggers condition check happens
  // regardless of player's SPD.
  //
  Map::Instance().UpdateTriggers(TriggerUpdateType::FINISH_TURN);

  //
  // If player killed an enemy but can still make another turn,
  // we must check and remove objects marked for deletion
  // or Application::DrawCurrentState() won't reflect that visually,
  // and we might also still attack / interact with objects
  // that should've already been destroyed, which is a bug.
  //
  // Also removes dead triggers.
  //
  // Probably bad design anyway but fuck it.
  //
  Map::Instance().RemoveDestroyed();

  Application::Instance().PlayerTurnsPassed++;
}

// =============================================================================

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
      for (size_t i = 0; i < Inventory->Contents.size(); i++)
      {
        ItemComponent* ic = Inventory->Contents[i]->GetComponent<ItemComponent>();
        if (ic != nullptr)
        {
          if (ic->Data.IsBurnable)
          {
            std::string objName = ic->Data.IsIdentified ? ic->Data.IdentifiedName : ic->Data.UnidentifiedName;
            auto str = Util::StringFormat("%s burns up!", objName.data());
            Printer::Instance().AddMessage(str);
            Inventory->Contents.erase(Inventory->Contents.begin() + i);
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

// =============================================================================

void Player::ProcessStarvation()
{
  //
  // No starving in town.
  //
  if (Map::Instance().CurrentLevel->MapType_ == MapType::TOWN)
  {
    return;
  }

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

// =============================================================================

void Player::ProcessHunger()
{
#ifdef DEBUG_BUILD
  if (GodMode)
  {
    return;
  }
#endif

  //
  // No starving in town.
  //
  if (Map::Instance().CurrentLevel->MapType_ == MapType::TOWN)
  {
    return;
  }

  bool noHunger = Equipment->HasBonus(ItemBonusType::REMOVE_HUNGER);

  if (noHunger && !IsStarving)
  {
    return;
  }

  Attrs.Hunger += Attrs.HungerSpeed.Get();

  //
  // HungerRate's CurrentValue is equal to OriginalValue
  //
  int maxHunger = Attrs.HungerRate.Get();

  Attrs.Hunger = Util::Clamp(Attrs.Hunger, 0, maxHunger);

  if (Attrs.Hunger == maxHunger)
  {
    ApplyStarvingPenalties();

    if (_starvingTimeout > GlobalConstants::StarvationDamageTimeout - 1)
    {
      Printer::Instance().AddMessage("You are starving!");
      Attrs.HP.AddMin(-1);
    }

    IsStarving = true;
  }
  else
  {
    UnapplyStarvingPenalties();

    _starvingTimeout = 0;
    IsStarving = false;
  }
}

// =============================================================================

void Player::ApplyStarvingPenalties()
{
  if (!HasEffect(ItemBonusType::WEAKNESS))
  {
    ItemBonusStruct bs;

    bs.Type     = ItemBonusType::WEAKNESS;
    bs.Id       = _objectId;
    bs.Duration = -1;
    bs.Period   = -1;

    AddEffect(bs);
  }
}

// =============================================================================

void Player::UnapplyStarvingPenalties()
{
  RemoveEffect(ItemBonusType::WEAKNESS, _objectId);
}

// =============================================================================

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

// =============================================================================

void Player::SetSoldierDefaultItems()
{
  Money = 250;

  auto go = ItemsFactory::Instance().CreateHealingPotion(ItemPrefix::UNCURSED);
  ItemComponent* ic = go->GetComponent<ItemComponent>();
  ic->Data.Amount = 3;

  Inventory->Add(go);

  go = ItemsFactory::Instance().CreateFood(0, 0, FoodType::IRON_RATIONS, ItemPrefix::UNCURSED);
  ic = go->GetComponent<ItemComponent>();
  ic->Data.Amount = 1;
  ic->Data.IsIdentified = true;

  Inventory->Add(go);

  go = ItemsFactory::Instance().CreateRepairKit(0, 0, 30, ItemPrefix::BLESSED);

  Inventory->Add(go);
}

// =============================================================================

void Player::SetThiefDefaultItems()
{
  Money = 500;

  auto go = ItemsFactory::Instance().CreateHealingPotion(ItemPrefix::UNCURSED);
  Inventory->Add(go);

  go = ItemsFactory::Instance().CreateManaPotion(ItemPrefix::UNCURSED);
  Inventory->Add(go);
}

// =============================================================================

void Player::SetArcanistDefaultItems()
{
  Money = 0;

  auto go = ItemsFactory::Instance().CreateManaPotion(ItemPrefix::BLESSED);
  ItemComponent* ic = go->GetComponent<ItemComponent>();
  ic->Data.Amount = 5;

  Inventory->Add(go);

  go = ItemsFactory::Instance().CreateReturner(0, 0, 3, ItemPrefix::UNCURSED);
  Inventory->Add(go);

  go = ItemsFactory::Instance().CreateScroll(0, 0, SpellType::MANA_SHIELD, ItemPrefix::BLESSED);
  Inventory->Add(go);
}

// =============================================================================

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

// =============================================================================

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

// =============================================================================

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
      Inventory->Add(go);
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
      Inventory->Add(go);
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
      Inventory->Add(go);
    }
    break;
  }
}

// =============================================================================

PlayerClass Player::GetClass()
{
  return (PlayerClass)SelectedClass;
}

// =============================================================================

const std::string& Player::GetClassName()
{
  return GlobalConstants::PlayerClassNameByType.at((PlayerClass)SelectedClass);
}

// =============================================================================

bool Player::HasSkill(PlayerSkills skillToCheck)
{
  return (SkillLevelBySkill.count(skillToCheck) == 1);
}
