#ifndef TASKATTACKBASIC_H
#define TASKATTACKBASIC_H

#include "behaviour-tree.h"

class TaskAttackBasic : public Node
{
  using Node::Node;

  public:
    BTResult Run() override;

  protected:

    // first means successful hit, second means if player has armor
    std::pair<bool, bool> _attackResultPlayerHasArmor;
};

#endif // TASKATTACKBASIC_H
