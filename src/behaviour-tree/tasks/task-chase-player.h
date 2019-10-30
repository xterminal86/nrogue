#ifndef TASKCHASEPLAYER_H
#define TASKCHASEPLAYER_H

#include "behaviour-tree.h"

#include "pathfinder.h"

class Player;

class TaskChasePlayer : public Node
{
  using Node::Node;

  public:
    BTResult Run() override;
};

#endif // TASKCHASEPLAYER_H
