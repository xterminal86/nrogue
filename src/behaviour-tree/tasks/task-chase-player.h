#ifndef TASKCHASEPLAYER_H
#define TASKCHASEPLAYER_H

#include "behaviour-tree.h"

#include "pathfinder.h"
#include "map.h"

class TaskChasePlayer : public Node
{
  public:
    TaskChasePlayer(GameObject* objectToControl, bool pfLimit = true)
      : Node(objectToControl)
    {
      int tw = Printer::Instance().TerminalWidth;
      int th = Printer::Instance().TerminalHeight;
      _pfLimit = pfLimit ? (tw * th) / 2 : -1;
    }

    void TryToFindPath()
    {      
      _path = _pf.BuildRoad(Map::Instance().CurrentLevel,
                           { _objectToControl->PosX, _objectToControl->PosY },
                           _playerPos,
                            true,
                            _pfLimit);

      _lastPlayerPos = _playerPos;
    }

    bool TryToMove()
    {
      _lastPos = _path.top();
      if (!_objectToControl->MoveTo(_lastPos))
      {
        // If path became invalid, abort
        _path = std::stack<Position>();
        _lastPos = { -1, -1 };
        return false;
      }

      _objectToControl->FinishTurn();
      _path.pop();

      return true;
    }

    bool IsPlayerVisible(const Position& playerPos)
    {
      Position objPos = { _objectToControl->PosX, _objectToControl->PosY };

      return Map::Instance().IsObjectVisible(objPos, playerPos);
    }

    bool Run() override
    {
      _playerPos = { _playerRef->PosX, _playerRef->PosY };

      // Recalculate path only if there's no path in the first place,
      // or if player changed his position but is still visible.
      if (_path.empty()
       || (_lastPlayerPos != _playerPos && IsPlayerVisible(_playerPos)))
      {
        TryToFindPath();

        // Player is unreachable
        if (_path.empty())
        {
          _lastPos = { -1, -1 };
          return false;
        }        
      }

      return TryToMove();
    }

  private:
    Pathfinder _pf;

    std::stack<Position> _path;

    Position _playerPos;
    Position _lastPlayerPos;

    Position _lastPos = { -1, -1 };

    int _pfLimit = -1;    
};

#endif // TASKCHASEPLAYER_H
