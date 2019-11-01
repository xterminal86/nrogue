#ifndef TASKGOTOLASTPLAYERPOS_H
#define TASKGOTOLASTPLAYERPOS_H

#include "behaviour-tree.h"

#include "pathfinder.h"

class Player;

class TaskGotoLastPlayerPos : public Node
{
  using Node::Node;

  public:
    BTResult Run() override;

  private:
    std::stack<Position> _path;
};

#endif // TASKGOTOLASTPLAYERPOS_H
