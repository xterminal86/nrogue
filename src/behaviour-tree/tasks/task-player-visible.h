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
      auto line = Util::BresenhamLine(_objectToControl->PosX, _objectToControl->PosY,
                                      _playerRef->PosX, _playerRef->PosY);
      for (auto& c : line)
      {
        auto& cell = Map::Instance().CurrentLevel->MapArray[c.X][c.Y];
        if (cell->Blocking || cell->BlocksSight)
        {
          return false;
        }
      }

      return true;
    }  
};

#endif
