#ifndef TASKRANDOMMOVEMENT_H
#define TASKRANDOMMOVEMENT_H

#include "behaviour-tree.h"

class TaskRandomMovement : public Node
{
  using Node::Node;

  public:
    BTResult Run() override;

  private:
    BTResult Smart();
    BTResult Dumb();
};

#endif // TASKRANDOMMOVEMENT_H
