#ifndef TASKGOTOLASTMINEDPOS_H
#define TASKGOTOLASTMINEDPOS_H

#include "behaviour-tree.h"

#include "pathfinder.h"

class Player;

class TaskGotoLastMinedPos : public Node
{
  using Node::Node;

  public:
    BTResult Run() override;
};

#endif // TASKGOTOLASTMINEDPOS_H
