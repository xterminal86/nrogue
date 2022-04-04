#include "task-attack-ranged.h"

#include "game-object.h"
#include "map.h"
#include "player.h"
#include "logger.h"

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
  if (ec != nullptr)
  {
    weapon = ec->EquipmentByCategory[EquipmentCategory::WEAPON][0];
  }

  int chanceToHit = 0;

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
    int damageDone = 1;
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
