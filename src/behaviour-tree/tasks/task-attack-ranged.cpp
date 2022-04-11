#include "task-attack-ranged.h"

#include "game-object.h"
#include "game-objects-factory.h"
#include "map.h"
#include "player.h"
#include "logger.h"
#include "printer.h"

//
// NOTE:
//
// It is probably better to kinda duplicate code for throwing,
// using ranged weapons, and using wands and magic for
// monsters instead of trying to make all of this functionality
// general purpose, since we will have to move lots of stuff
// into already overloaded Util class and do lots of message
// related shitcoding.
//
// We still need to create additional AI task types to handle all
// of this, so having player related checks to be in one place
// and monster related in another kinda beats it.
//
TaskAttackRanged::TaskAttackRanged(GameObject* objectToControl,
                                   const std::string& attackType,
                                   const std::string& spellType,
                                   char projectile,
                                   const std::string& fgColor,
                                   const std::string& bgColor)
  : Node(objectToControl)
{
  _projectile = projectile;
  _fgColor    = fgColor;
  _bgColor    = bgColor;

  if (_attackTypeByName.count(attackType) == 1)
  {
    _attackType = _attackTypeByName.at(attackType);
  }
  else
  {
    _attackType = RangedAttackType::UNDEFINED;
  }

  auto f = Util::FlipMap(GlobalConstants::SpellShortNameByType);
  if (f.count(spellType) == 1)
  {
    _spellType = f.at(spellType);
  }
  else
  {
    _spellType = SpellType::NONE;
  }
}

BTResult TaskAttackRanged::Run()
{
  BTResult res = BTResult::Failure;

  if (_attackType == RangedAttackType::WEAPON)
  {
    res = ProcessWeaponAttack();
  }
  else
  {
    res = ProcessSpellAttack();
  }

  return res;

#if 0
  Position from = { _objectToControl->PosX, _objectToControl->PosY };
  Position to   = { _playerRef->PosX, _playerRef->PosY };

  EquipmentComponent* ec = _objectToControl->GetComponent<EquipmentComponent>();
  ItemComponent* weapon = nullptr;
  ItemComponent* arrows = nullptr;
  if (ec != nullptr)
  {
    weapon = ec->EquipmentByCategory[EquipmentCategory::WEAPON][0];
    arrows = ec->EquipmentByCategory[EquipmentCategory::SHIELD][0];
  }

  //
  // Check if bow has arrows, wand has charges etc.
  //
  BTResult r = CheckRangedWeaponValidity(weapon, arrows);
  if (r == BTResult::Failure)
  {
    return r;
  }

  int chanceToHit = 0;
  int damageDone = 1;

  chanceToHit = Util::CalculateHitChanceRanged(from,
                                               to,
                                               _objectToControl,
                                               weapon,
                                               false);

  if (Util::Rolld100(chanceToHit) == false)
  {
    to = Util::GetRandomPointAround(_objectToControl, weapon, to);
  }

  GameObject* hit = nullptr;

  auto line = Util::BresenhamLine(from, to);

  bool isLaser = (weapon != nullptr
               && weapon->Data.SpellHeld.SpellType_ == SpellType::LASER);

  std::vector<Position> laserLine;

  if (weapon != nullptr)
  {
    switch (weapon->Data.ItemType_)
    {
      case ItemType::WAND:
      {
        switch (weapon->Data.SpellHeld.SpellType_)
        {
          case SpellType::LASER:
            laserLine = Util::ProcessLaserAttack(_objectToControl, weapon, to);
            break;
        }
      }
      break;

      case ItemType::RANGED_WEAPON:
      {
        hit = Util::GetFirstObjectOnTheLine(line);
      }
      break;
    }
  }
  else
  {
    hit = Util::GetFirstObjectOnTheLine(line);
  }

  if (hit != nullptr)
  {
    to = { hit->PosX, hit->PosY };
  }

  if (!isLaser)
  {
    Util::LaunchProjectile(line, _projectile, _fgColor, _bgColor);
  }
  else
  {
    Util::DrawLaserAttack(laserLine);
  }

  /*
  auto line = Util::BresenhamLine(from, to);
  hit = Util::GetFirstObjectOnTheLine(line);

  if (hit != nullptr)
  {
    to = { hit->PosX, hit->PosY };
  }

  // FIXME: wand of laser or single projectile

  Util::LaunchProjectile(from, to, _projectile, _fgColor, _bgColor);

  // FIXME: damage should come from enemy weapon or magic
  */

  // Not calling FinishTurn() so that AI update through the tree can continue

  BTResult res = (hit == _playerRef) ? BTResult::Success : BTResult::Failure;

  return res;
#endif
}

BTResult TaskAttackRanged::ProcessSpellAttack()
{
  Position from = { _objectToControl->PosX, _objectToControl->PosY };
  Position to   = { _playerRef->PosX, _playerRef->PosY };

  int chanceToHit = Util::CalculateHitChanceRanged(from,
                                                   to,
                                                   _objectToControl,
                                                   nullptr,
                                                   false);

  // TODO: spell range?

  if (Util::Rolld100(chanceToHit) == false)
  {
    to = Util::GetRandomPointAround(_objectToControl, nullptr, to);
  }

  // TODO: handle spell

  return BTResult::Success;
}

BTResult TaskAttackRanged::ProcessWeaponAttack()
{
  Position from = { _objectToControl->PosX, _objectToControl->PosY };
  Position to   = { _playerRef->PosX, _playerRef->PosY };

  EquipmentComponent* ec = _objectToControl->GetComponent<EquipmentComponent>();
  ItemComponent* weapon = nullptr;
  ItemComponent* arrows = nullptr;
  if (ec != nullptr)
  {
    weapon = ec->EquipmentByCategory[EquipmentCategory::WEAPON][0];
    arrows = ec->EquipmentByCategory[EquipmentCategory::SHIELD][0];
  }

  if (weapon == nullptr)
  {
    return BTResult::Failure;
  }

  //
  // Check if bow has arrows, wand has charges etc.
  //
  BTResult r = CheckRangedWeaponValidity(weapon, arrows);
  if (r == BTResult::Failure)
  {
    return r;
  }

  int chanceToHit = Util::CalculateHitChanceRanged(from,
                                                   to,
                                                   _objectToControl,
                                                   weapon,
                                                   false);

  if (Util::Rolld100(chanceToHit) == false)
  {
    to = Util::GetRandomPointAround(_objectToControl, weapon, to);
  }

  auto line = Util::BresenhamLine(from, to);
  GameObject* hit = Util::GetFirstObjectOnTheLine(line);

  //
  // If something is hit, launch projectile up to this point
  //
  if (hit != nullptr)
  {
    to = { hit->PosX, hit->PosY };
  }
  else
  {
    //
    // Otherwise to the cursor position
    //
    hit = Map::Instance().CurrentLevel->MapArray[to.X][to.Y].get();
  }

  if (weapon->Data.ItemType_ == ItemType::RANGED_WEAPON)
  {
    Util::LaunchProjectile(from, to, _projectile, _fgColor, _bgColor);
    ProcessBows(weapon, arrows, hit);
  }
  else if (weapon->Data.ItemType_ == ItemType::WAND)
  {
    if (weapon->Data.SpellHeld.SpellType_ == SpellType::LASER)
    {
      Util::DrawLaserAttack(line);
    }
    else
    {
      Util::LaunchProjectile(from, to, _projectile, _fgColor, _bgColor);
    }

    ProcessWand(weapon, hit);
  }

  return Util::IsPlayer(hit) ? BTResult::Success : BTResult::Failure;
}

void TaskAttackRanged::ProcessBows(ItemComponent* weapon,
                                   ItemComponent* arrows,
                                   GameObject* what)
{
  int dmg = Util::CalculateDamageValue(_objectToControl,
                                       what,
                                       weapon,
                                       false);

  //
  // If it's not the ground GameObject
  //
  if (what->Type != GameObjectType::GROUND)
  {
    if (what->IsAlive())
    {
      bool succ = what->ReceiveDamage(_objectToControl, dmg, false, false);
      if (succ
       && weapon->Data.HasBonus(ItemBonusType::LEECH)
       && what->IsLiving)
      {
        what->Attrs.HP.AddMin(dmg);
      }
    }
  }
  else if (what->Type != GameObjectType::DEEP_WATER
        || what->Type != GameObjectType::LAVA
        || what->Type != GameObjectType::CHASM)
  {
    //
    // Create arrow object on the cell where it landed
    //
    ItemComponent* arrow = GameObjectsFactory::Instance().CloneItem(arrows);
    arrow->OwnerGameObject->PosX = what->PosX;
    arrow->OwnerGameObject->PosY = what->PosY;
    arrow->Data.Amount = 1;
    arrow->Data.IsEquipped = false;
    Map::Instance().PlaceGameObject(arrow->OwnerGameObject);
  }

  arrows->Data.Amount--;

  if (arrows->Data.Amount <= 0)
  {
    arrows->Break(_objectToControl);
  }

  Util::TryToDamageEquipment(_objectToControl, weapon, -1);
}

void TaskAttackRanged::ProcessWand(ItemComponent* wand, GameObject* what)
{
  wand->Data.Amount--;

  SpellInfo si = wand->Data.SpellHeld;

  auto baseDamagePair = si.SpellBaseDamage;

  int bonus = _objectToControl->Attrs.Mag.Get();

  int centralDamage = Util::RollDamage(baseDamagePair.first,
                                       baseDamagePair.second);

  centralDamage += bonus;

  switch (wand->Data.SpellHeld.SpellType_)
  {
    case SpellType::STRIKE:
      ProcessWandDamage(what, wand, centralDamage, false);
      break;

    case SpellType::MAGIC_MISSILE:
    case SpellType::FROST:
      ProcessWandDamage(what, wand, centralDamage, true);
      break;

    case SpellType::FIREBALL:
      ProcessAoEDamage(what, wand, centralDamage, true);
      break;

    case SpellType::TELEPORT:
      Util::ProcessTeleport(what);
      break;
  }
}

void TaskAttackRanged::ProcessWandDamage(GameObject* target,
                                         ItemComponent* weapon,
                                         int damage,
                                         bool againstRes)
{
  Position p = { target->PosX, target->PosY };

  GameObject* actor = nullptr;
  if (_playerRef->PosX == p.X
   && _playerRef->PosY == p.Y)
  {
    actor = _playerRef;
  }

  if (actor == nullptr)
  {
    actor = Map::Instance().GetActorAtPosition(p.X, p.Y);
  }

  if (actor != nullptr)
  {
    auto ret = Util::TryToDamageObject(actor,
                                       _objectToControl,
                                       damage,
                                       againstRes);

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
  }
  else
  {
    // Magic attack should probably hit everything,
    // since it's, well, magical. You can't really control it.

    // Check items first
    auto mapObjs = Map::Instance().GetGameObjectsAtPosition(p.X, p.Y);
    for (auto& obj : mapObjs)
    {
      Util::TryToDamageObject(obj,
                              _objectToControl,
                              damage,
                              againstRes);
    }

    // If nothing is lying in the doorway, for example, damage door
    if (mapObjs.empty())
    {
      auto so = Map::Instance().GetStaticGameObjectAtPosition(p.X, p.Y);
      if (so != nullptr)
      {
        Util::TryToDamageObject(so,
                                _objectToControl,
                                damage,
                                againstRes);
      }
    }
  }
}

void TaskAttackRanged::ProcessAoEDamage(GameObject* target,
                                        ItemComponent* weapon,
                                        int centralDamage,
                                        bool againstRes)
{
  auto pointsAffected = Printer::Instance().DrawExplosion({ target->PosX, target->PosY }, 3);

  //Util::PrintVector("points affected", pointsAffected);

  GameObject* from = (weapon == nullptr) ?
                     nullptr :
                     weapon->OwnerGameObject;

  for (auto& p : pointsAffected)
  {
    int d = Util::LinearDistance({ target->PosX, target->PosY }, p);
    if (d == 0)
    {
      d = 1;
    }

    int dmgHere = centralDamage / d;

    // AoE damages everything

    auto actor = Map::Instance().GetActorAtPosition(p.X, p.Y);
    Util::TryToDamageObject(actor, from, dmgHere, againstRes);

    auto mapObjs = Map::Instance().GetGameObjectsAtPosition(p.X, p.Y);
    for (auto& obj : mapObjs)
    {
      Util::TryToDamageObject(obj, from, dmgHere, againstRes);
    }

    auto so = Map::Instance().GetStaticGameObjectAtPosition(p.X, p.Y);
    if (so != nullptr)
    {
      Util::TryToDamageObject(so, from, dmgHere, againstRes);
    }

    // Check self damage
    if (_playerRef->PosX == p.X && _playerRef->PosY == p.Y)
    {
      int dmgHere = centralDamage / d;
      dmgHere -= _playerRef->Attrs.Res.Get();

      _playerRef->ReceiveDamage(from, dmgHere, true);
    }
  }
}

BTResult TaskAttackRanged::CheckRangedWeaponValidity(ItemComponent* weapon,
                                                     ItemComponent* arrows)
{
  switch (weapon->Data.ItemType_)
  {
    case ItemType::WAND:
    {
      if (weapon->Data.Amount == 0)
      {
        return BTResult::Failure;
      }
    }
    break;

    case ItemType::RANGED_WEAPON:
    {
      bool isBow = (weapon->Data.RangedWeaponType_ == RangedWeaponType::SHORT_BOW
                 || weapon->Data.RangedWeaponType_ == RangedWeaponType::LONGBOW
                 || weapon->Data.RangedWeaponType_ == RangedWeaponType::WAR_BOW);

      bool isXBow = (weapon->Data.RangedWeaponType_ == RangedWeaponType::LIGHT_XBOW
                  || weapon->Data.RangedWeaponType_ == RangedWeaponType::XBOW
                  || weapon->Data.RangedWeaponType_ == RangedWeaponType::HEAVY_XBOW);

      bool noArrows = (arrows == nullptr ||
                       (arrows != nullptr
                     && arrows->Data.ItemType_ != ItemType::ARROWS));

      if ((isBow || isXBow) && noArrows)
      {
        return BTResult::Failure;
      }

      bool notArrows = (arrows != nullptr
                     && arrows->Data.AmmoType != ArrowType::ARROWS);

      if (isBow && notArrows)
      {
        return BTResult::Failure;
      }

      bool notBolts = (arrows != nullptr
                    && arrows->Data.AmmoType != ArrowType::BOLTS);

      if (isXBow && notBolts)
      {
        return BTResult::Failure;
      }
    }
    break;
  }

  return BTResult::Success;
}
