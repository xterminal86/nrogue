#ifndef TASKRANDOMMOVEMENT_H
#define TASKRANDOMMOVEMENT_H

#include "behaviour-tree.h"

class TaskRandomMovement : public Node
{
  using Node::Node;

  public:
    BTResult Run() override;
};

#endif // TASKRANDOMMOVEMENT_H
