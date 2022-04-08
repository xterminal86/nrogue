#include "task-attack-ranged.h"

#include "game-object.h"
#include "map.h"
#include "player.h"
#include "logger.h"

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
                                   const std::string& damageType,
                                   char projectile,
                                   const std::string& fgColor,
                                   const std::string& bgColor)
  : Node(objectToControl)
{
  _projectile        = projectile;
  _fgColor           = fgColor;
  _bgColor           = bgColor;

  if (_attackTypeByName.count(damageType) == 1)
  {
    _attackType = _attackTypeByName.at(damageType);
  }
  else
  {
    _attackType = RangedAttackType::UNDEFINED;
  }
}

BTResult TaskAttackRanged::Run()
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

  if (_attackType == RangedAttackType::PHYSICAL
   || _attackType == RangedAttackType::NO_DAMAGE)
  {
    chanceToHit = Util::CalculateHitChanceRanged(from,
                                                 to,
                                                 _objectToControl,
                                                 weapon,
                                                 false);
  }

  if (Util::Rolld100(chanceToHit) == false)
  {
    to = Util::GetRandomPointAround(_objectToControl, weapon, to);
  }

  GameObject* hit = nullptr;

  auto line = Util::BresenhamLine(from, to);
  for (auto& p : line)
  {
    if (p.X == _objectToControl->PosX && p.Y == _objectToControl->PosY)
    {
      continue;
    }

    GameObject* so = Map::Instance().GetStaticGameObjectAtPosition(p.X, p.Y);
    GameObject* ao = Map::Instance().GetActorAtPosition(p.X, p.Y);

    if (so != nullptr)
    {
      hit = so;
      break;
    }
    else if (ao != nullptr)
    {
      hit = ao;
      break;
    }
    else if (_playerRef->PosX == p.X && _playerRef->PosY == p.Y)
    {
      hit = _playerRef;
      break;
    }
  }

  if (hit != nullptr)
  {
    to = { hit->PosX, hit->PosY };
  }

  // FIXME: wand of laser or single projectile

  Util::LaunchProjectile(from, to, _projectile, _fgColor, _bgColor);

  // FIXME: damage should come from enemy weapon or magic

  bool suppressLog = (hit != _playerRef);

  BTResult res = (hit == _playerRef) ? BTResult::Success : BTResult::Failure;

  if (hit != nullptr)
  {
    if (_attackType == RangedAttackType::PHYSICAL)
    {
      damageDone += _objectToControl->Attrs.Str.Get();
      hit->ReceiveDamage(_objectToControl, damageDone, false, suppressLog);
    }
    else if (_attackType == RangedAttackType::MAGICAL)
    {
      damageDone += _objectToControl->Attrs.Mag.Get();
      hit->ReceiveDamage(_objectToControl, damageDone, true, suppressLog);
    }
  }

  // Not calling FinishTurn() so that AI update through the tree can continue

  return res;
}

BTResult TaskAttackRanged::CheckRangedWeaponValidity(ItemComponent* weapon,
                                       ItemComponent* arrows)
{
  //
  // Assuming if weapon is null, it's a magic attack
  //
  if (weapon != nullptr)
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
  }

  return BTResult::Success;
}
