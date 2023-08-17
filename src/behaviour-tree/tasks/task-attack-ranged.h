#ifndef TASKATTACKRANGED_H
#define TASKATTACKRANGED_H

#include "behaviour-tree.h"

enum class RangedAttackType
{
  UNDEFINED = 0,
  WEAPON,
  MAGIC
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
                     const std::string& attackType,
                     const std::string& spellType,
                     char projectile,
                     const uint32_t& fgColor,
                     const uint32_t& bgColor);

    BTResult Run() override;

  protected:
    BTResult CheckRangedWeaponValidity(ItemComponent* weapon, ItemComponent* arrows);
    BTResult ProcessWeaponAttack();
    BTResult ProcessSpellAttack();

    void ProcessBows(ItemComponent* weapon,
                     ItemComponent* arrows,
                     GameObject* what);

    void ProcessWand(ItemComponent* wand,
                     GameObject* what);

    void ProcessAoEDamage(GameObject* target,
                          ItemComponent* weapon,
                          int centralDamage,
                          bool againstRes);

    void ProcessWandDamage(GameObject* target,
                           ItemComponent* weapon,
                           int damage,
                           bool againstRes);

    char _projectile;
    RangedAttackType _attackType = RangedAttackType::UNDEFINED;
    SpellType _spellType = SpellType::NONE;
    uint32_t _fgColor;
    uint32_t _bgColor;

    const std::unordered_map<std::string, RangedAttackType> _attackTypeByName =
    {
      { "WPN", RangedAttackType::WEAPON },
      { "MAG", RangedAttackType::MAGIC  }
    };
};

#endif // TASKATTACKRANGED_H
