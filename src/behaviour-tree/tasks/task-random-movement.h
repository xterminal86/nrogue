#ifndef TASKRANDOMMOVEMENT_H
#define TASKRANDOMMOVEMENT_H

#include "behaviour-tree.h"

#include "rng.h"

class TaskRandomMovement : public Node
{
  using Node::Node;

  public:
    bool Run() override
    {
      int dx = RNG::Instance().Random() % 2;
      int dy = RNG::Instance().Random() % 2;

      int signX = (RNG::Instance().Random() % 2) == 0 ? -1 : 1;
      int signY = (RNG::Instance().Random() % 2) == 0 ? -1 : 1;

      dx *= signX;
      dy *= signY;

      return _objectToControl->Move(dx, dy);
    }
};

#endif // TASKRANDOMMOVEMENT_H
