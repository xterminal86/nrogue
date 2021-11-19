#ifndef TASKATTACKBASIC_H
#define TASKATTACKBASIC_H

#include "behaviour-tree.h"

struct AttackResult
{
  bool Success = false;
  bool HasArmor = false;
  int DamageDone = 0;
};

class TaskAttackBasic : public Node
{
  public:
    TaskAttackBasic(GameObject* objectToControl,
                    bool alwaysHitOverride = false);

    BTResult Run() override;

  protected:
    AttackResult _attackResult;

    bool _alwaysHitOverride = false;
};

#endif // TASKATTACKBASIC_H
