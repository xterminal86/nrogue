#ifndef TASKATTACKRANGED_H
#define TASKATTACKRANGED_H

#include "behaviour-tree.h"

enum class RangedAttackType
{
  UNDEFINED = 0,
  PHYSICAL,
  MAGICAL
};

class TaskAttackRanged : public Node
{
  public:
    TaskAttackRanged(GameObject* objectToControl,
                     const std::string& damageType,
                     char projectile,
                     const std::string& fgColor,
                     const std::string& bgColor);

    BTResult Run() override;

  private:
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
      { "str", RangedAttackType::PHYSICAL },
      { "mag", RangedAttackType::MAGICAL  }
    };
};

#endif // TASKATTACKRANGED_H