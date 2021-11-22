#ifndef TASKATTACKRANGED_H
#define TASKATTACKRANGED_H

#include "behaviour-tree.h"

enum class RangedAttackType
{
  UNDEFINED = 0,
  PHYSICAL,
  NO_DAMAGE,
  MAGICAL
};

///
/// Ranged attack task returns success if player was hit.
/// Otherwise it returns BTResult::Failure.
///
/// Please note, that ranged attack doesn't consume the turn,
/// so you can rely on its return result status to code-in
/// some additional logic (see shelob script).
///
class TaskAttackRanged : public Node
{
  public:
    TaskAttackRanged(GameObject* objectToControl,
                     const std::string& damageType,
                     char projectile,
                     const std::string& fgColor,
                     const std::string& bgColor);

    BTResult Run() override;

  protected:
    int CalculateChance(const Position& startPoint,
                        const Position& endPoint,
                        int baseChance);

    Position GetRandomPointAround(const Position& from);

    char _projectile;
    RangedAttackType _attackType;
    std::string _fgColor;
    std::string _bgColor;

    const int kBaseChanceToHit = 50;

    const std::map<std::string, RangedAttackType> _attackTypeByName =
    {
      { "STR", RangedAttackType::PHYSICAL  },
      { "MAG", RangedAttackType::MAGICAL   },
      { "NA",  RangedAttackType::NO_DAMAGE }
    };
};

#endif // TASKATTACKRANGED_H
