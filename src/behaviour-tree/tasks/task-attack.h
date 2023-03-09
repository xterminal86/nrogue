#ifndef TASKATTACK_H
#define TASKATTACK_H

#include "behaviour-tree.h"

using DamageRoll = std::pair<int, int>;
using AttackResult = std::pair<bool, int>;

class TaskAttack : public Node
{
  public:
    TaskAttack(GameObject* objectToControl,
                    bool alwaysHitOverride = false);

    BTResult Run() override;

  protected:
    bool _alwaysHitOverride = false;

    bool AttackWithWeapon();
    AttackResult AttackUnarmed(const DamageRoll& damageRoll = DamageRoll());

    bool PlayerHasArmor();

    Player* _playerRef = nullptr;

    #if DEBUG_BUILD
    void LogAttackData(int hitChance);
    #endif
};

#endif // TASKATTACK_H
