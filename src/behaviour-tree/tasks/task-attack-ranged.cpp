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
  _projectile = projectile;
  _fgColor    = fgColor;
  _bgColor    = bgColor;

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

  int chanceToHit = 100;

  if (_attackType == RangedAttackType::PHYSICAL)
  {
    chanceToHit = CalculateChance(from, to, kBaseChanceToHit);
  }

  if (Util::Rolld100(chanceToHit) == false)
  {
    to = GetRandomPointAround(to);
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

  Util::LaunchProjectile(from, to, _projectile, _fgColor, _bgColor);

  // FIXME: damage should come from enemy weapon or magic

  bool suppressLog = (hit != _playerRef);

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

  _objectToControl->FinishTurn();

  return BTResult::Success;
}

int TaskAttackRanged::CalculateChance(const Position& startPoint,
                                      const Position& endPoint,
                                      int baseChance)
{
  int attackChanceScale = 5;

  int chance = baseChance;

  int skl = _objectToControl->Attrs.Skl.Get();
  chance += (attackChanceScale * skl);

  int distanceChanceDrop = 2;

  int d = (int)Util::LinearDistance(startPoint, endPoint);
  chance -= (distanceChanceDrop * d);

  auto str = Util::StringFormat("%s %s - Calculated hit chance: %i (SKL: %i, SKL bonus: %i, distance: -%i)",
                                _objectToControl->ObjectName.data(),
                                __PRETTY_FUNCTION__,
                                chance,
                                skl,
                                (attackChanceScale * skl),
                                (distanceChanceDrop * d));
  Logger::Instance().Print(str);

  /*
  DebugLog("%s\n%i + %i - %i = %i\n", __PRETTY_FUNCTION__,
                                     baseChance,
                                     (attackChanceScale * skl),
                                     (distanceChanceDrop * d),
                                     chance);
  */

  return chance;
}

Position TaskAttackRanged::GetRandomPointAround(const Position& from)
{
  Position res = { 0, 0 };

  auto rect = Util::GetEightPointsAround(from, Map::Instance().CurrentLevel->MapSize);
  for (size_t i = 0; i < rect.size(); i++)
  {
    bool targetSelf = (rect[i].X == _objectToControl->PosX
                    && rect[i].Y == _objectToControl->PosY);

    if (targetSelf)
    {
      rect.erase(rect.begin() + i);
      break;
    }
  }

  if (!rect.empty())
  {
    int index = RNG::Instance().RandomRange(0, rect.size());
    res = rect[index];
  }

  return res;
}
