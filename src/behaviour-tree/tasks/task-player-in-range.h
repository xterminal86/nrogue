#ifndef TASKPLAYERINRANGE_H
#define TASKPLAYERINRANGE_H

#include "behaviour-tree.h"

class TaskPlayerInRange : public Node
{
  public:
    TaskPlayerInRange(GameObject* objectToControl, int range = 1)
      : Node(objectToControl)
    {
      _range = range;
    }

    bool Run() override
    {
      int px = _playerRef->PosX;
      int py = _playerRef->PosY;

      int x = _objectToControl->PosX;
      int y = _objectToControl->PosY;

      int lx = x - _range;
      int ly = y - _range;
      int hx = x + _range;
      int hy = y + _range;

      return (px >= lx && px <= hx
           && py >= ly && py <= hy);
    }

  private:
    int _range = 0;
};

#endif // TASKPLAYERINRANGE_H
