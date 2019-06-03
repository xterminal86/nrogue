#include "ai-chase-player-state.h"

#include "ai-model-base.h"
#include "ai-wander-state.h"
#include "map.h"
#include "application.h"

void AIChasePlayerState::Enter()
{
  _lastPath = std::stack<Position>();
}

void AIChasePlayerState::Exit()
{
  _lastPath = std::stack<Position>();
}

void AIChasePlayerState::Run()
{
  if (_objectToControl->Attrs.ActionMeter < GlobalConstants::TurnReadyValue)
  {
    _objectToControl->WaitForTurn();
    return;
  }

  // If player is directly adjacent
  // (i.e. player was reached via path), do nothing.
  // This way we are still in ChasePlayer state and can perform
  // other logic in monster AI model (e.g. attack) via condition
  // against this state.
  if (!_lastPath.empty() && _ownerModel->IsPlayerInRange())
  {
    return;
  }

  if (_lastPath.empty())
  {
    FindPathToPlayer();

    // If player is still unreachable
    if (_lastPath.empty())
    {
      _ownerModel->ChangeAIState<AIWanderState>();
    }
  }
  else
  {
    Position posToMove = _lastPath.top();
    if (_objectToControl->MoveTo(posToMove.X, posToMove.Y))
    {
      _lastPath.pop();
      _objectToControl->FinishTurn();
    }
    else
    {
      // If path became blocked
      // (other monster stepped on it or map objects changed
      // so that path is now blocked), clear it.
      _lastPath = std::stack<Position>();
    }

    // If player position changed since the last
    // pathfinding call, path is invalid.
    if (GetPlayerPos() != _playerLastPos)
    {
      _lastPath = std::stack<Position>();
    }
  }
}

Position AIChasePlayerState::GetPlayerPos()
{
  return
  {
    Application::Instance().PlayerInstance.PosX,
    Application::Instance().PlayerInstance.PosY
  };
}

void AIChasePlayerState::FindPathToPlayer()
{
  _objectPos = { _objectToControl->PosX, _objectToControl->PosY };
  _playerPos = GetPlayerPos();

  _playerLastPos = _playerPos;

  int searchLimit = -1; // _ownerModel->AgroRadius;

  _lastPath = _pathFinder.BuildRoad(Map::Instance().CurrentLevel,
                                    _objectPos,
                                    _playerPos,
                                    true,
                                    searchLimit);
}
