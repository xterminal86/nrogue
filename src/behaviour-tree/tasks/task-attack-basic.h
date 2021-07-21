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
  using Node::Node;

  public:
    BTResult Run() override;

  protected:
    AttackResult _attackResult;
};

#endif // TASKATTACKBASIC_H
