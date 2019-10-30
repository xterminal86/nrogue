#ifndef TASKIDLE_H
#define TASKIDLE_H

#include "behaviour-tree.h"

class TaskIdle : public Node
{
  using Node::Node;

  public:
    BTResult Run() override;
};

#endif // TASKIDLE_H
