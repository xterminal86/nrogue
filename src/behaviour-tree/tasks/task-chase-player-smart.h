#ifndef TASKCHASEPLAYERSMART_H
#define TASKCHASEPLAYERSMART_H

#include "behaviour-tree.h"
#include "task-chase-player.h"

#include "map.h"

class TaskChasePlayerSmart : public TaskChasePlayer
{
  public:
    TaskChasePlayerSmart(GameObject* objectToControl, int agroRadius, bool pfLimit = true)
      : TaskChasePlayer(objectToControl, pfLimit)
    {
      _agroRadius = agroRadius;
    }

    bool IsPlayerVisible()
    {
      Position objPos = { _objectToControl->PosX, _objectToControl->PosY };
      return Map::Instance().IsObjectVisible(objPos, _playerPos);
    }

    bool IsPlayerInRange()
    {
      Position objPos = { _objectToControl->PosX, _objectToControl->PosY };
      return Util::IsObjectInRange(objPos, _playerPos, _agroRadius,_agroRadius);
    }

    bool Run() override
    {
      _playerPos = { _playerRef->PosX, _playerRef->PosY };

      if (IsPlayerInRange())
      {
        if (IsPlayerVisible())
        {
          if (_path.empty())
          {
            TryToFindPath();

            if (_path.empty())
            {
              _lastPos = { -1, -1 };
              return false;
            }

            return TryToMove();
          }
          else
          {
            if (_playerPos != _lastPlayerPos)
            {
              TryToFindPath();

              if (_path.empty())
              {
                _lastPos = { -1, -1 };
                return false;
              }
            }

            return TryToMove();
          }
        }
        else
        {
          if (!_path.empty())
          {
            return TryToMove();
          }
        }
      }

      if (!_path.empty())
      {
        _path = std::stack<Position>();
        _lastPos = { -1, -1 };
      }

      return false;
    }

  private:
    int _agroRadius = 0;
};

#endif // TASKCHASEPLAYERSMART_H
