#include "task-chase-player.h"

#include "game-object.h"
#include "application.h"
#include "pathfinder.h"
#include "map.h"

TaskChasePlayer::TaskChasePlayer(GameObject *objectToControl)
  : Node(objectToControl)
{
  _playerRef = &Application::Instance().PlayerInstance;
}

BTResult TaskChasePlayer::Run()
{
  Position playerPos = { _playerRef->PosX, _playerRef->PosY };
  Position objPos = { _objectToControl->PosX, _objectToControl->PosY };

  Pathfinder pf;
  auto path = pf.BuildRoad(Map::Instance().CurrentLevel,
                           objPos,
                           playerPos,
                           std::vector<char>(),
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
