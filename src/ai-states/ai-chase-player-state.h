#ifndef AICHASEPLAYERSTATE_H
#define AICHASEPLAYERSTATE_H

#include "ai-state-base.h"

#include "pathfinder.h"

class AIChasePlayerState : public AIStateBase
{
  using AIStateBase::AIStateBase;

  public:
    void Run() override;
    void Exit() override;
    void Enter() override;

  private:
    Pathfinder _pathFinder;

    Position _objectPos = { -1, -1 };
    Position _playerPos = { -1, -1 };
    Position _playerLastPos = { -1, -1 };

    std::stack<Position> _lastPath;

    Position GetPlayerPos();
    void FindPathToPlayer();
};

#endif // AICHASEPLAYERSTATE_H
