#ifndef TASKPLAYERINRANGE_H
#define TASKPLAYERINRANGE_H

#include "behaviour-tree.h"

#include "util.h"

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
      Position plPos = { _playerRef->PosX, _playerRef->PosY };
      Position objPos = { _objectToControl->PosX, _objectToControl->PosY };

      return Util::IsObjectInRange(plPos, objPos, _range, _range);
    }

  private:
    int _range = 0;
};

#endif // TASKPLAYERINRANGE_H
