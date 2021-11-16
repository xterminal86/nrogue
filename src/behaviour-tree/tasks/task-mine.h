#ifndef TASKMINE_H
#define TASKMINE_H

#include "behaviour-tree.h"

class TaskMine : public Node
{
  using Node::Node;

  public:
    BTResult Run() override;
};

#endif // TASKMINE_H
