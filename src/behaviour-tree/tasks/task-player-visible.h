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
      auto& mapRef = Map::Instance().CurrentLevel;

      auto line = Util::BresenhamLine(_objectToControl->PosX, _objectToControl->PosY,
                                      _playerRef->PosX, _playerRef->PosY);
      for (auto& c : line)
      {
        // Object can be blocking but not block sight (e.g. lava, chasm)
        // so check against BlocksSight only is needed.

        bool groundBlock = mapRef->MapArray[c.X][c.Y]->BlocksSight;
        bool staticBlock = false;

        if (mapRef->StaticMapObjects[c.X][c.Y] != nullptr)
        {
          staticBlock = mapRef->StaticMapObjects[c.X][c.Y]->BlocksSight;
        }

        if (groundBlock || staticBlock)
        {
          return false;
        }
      }

      return true;
    }  
};

#endif
