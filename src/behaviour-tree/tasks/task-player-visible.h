#ifndef TASKPLAYERVISIBLE_H
#define TASKPLAYERVISIBLE_H

#include "behaviour-tree.h"

#include "map.h"

class TaskPlayerVisible : public Node
{
  using Node::Node;

  public:    
    bool Run() override
    {
      Position plPos = { _playerRef->PosX, _playerRef->PosY };
      Position objPos = { _objectToControl->PosX, _objectToControl->PosY };

      return Map::Instance().IsObjectVisible(objPos, plPos);
    }  
};

#endif
