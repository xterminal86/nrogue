#include "task-chase-player.h"

#include "game-object.h"
#include "application.h"
#include "pathfinder.h"
#include "map.h"

BTResult TaskChasePlayer::Run()
{
  //DebugLog("[TaskChasePlayer]\n");

  Pathfinder pf;

  auto path = pf.BuildRoad(Map::Instance().CurrentLevel,
                           _objectToControl->GetPosition(),
                           _playerRef->GetPosition(),
                           std::vector<char>(),
                           false,
                           true);

  if (path.empty())
  {
    return BTResult::Failure;
  }

  auto moveTo = path.top();
  if (_objectToControl->MoveTo(moveTo))
  {
    _objectToControl->FinishTurn();
    return BTResult::Success;
  }

  return BTResult::Failure;
}
