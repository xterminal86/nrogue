#ifndef TASKMOVESMART_H
#define TASKMOVESMART_H

#include "behaviour-tree.h"

class TaskMoveSmart : public Node
{
  using Node::Node;

  public:
    BTResult Run() override;
};

#endif // TASKMOVESMART_H
