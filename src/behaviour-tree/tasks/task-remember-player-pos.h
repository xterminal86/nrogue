#ifndef TASKREMEMBERPLAYERPOS_H
#define TASKREMEMBERPLAYERPOS_H

#include "behaviour-tree.h"

class TaskRememberPlayerPos : public Node
{
  using Node::Node;

  public:
    BTResult Run() override;
};

#endif // TASKREMEMBERPLAYERPOS_H
