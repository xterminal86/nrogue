#ifndef TASKMOVEAWAYFROMPLAYER_H
#define TASKMOVEAWAYFROMPLAYER_H

#include "behaviour-tree.h"

class TaskMoveAwayFromPlayer : public Node
{
  using Node::Node;

  public:
    BTResult Run() override;
};

#endif // TASKMOVEAWAYFROMPLAYER_H
