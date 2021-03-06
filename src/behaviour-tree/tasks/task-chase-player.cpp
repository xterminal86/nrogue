#include "task-chase-player.h"

#include "game-object.h"
#include "application.h"
#include "pathfinder.h"
#include "map.h"

BTResult TaskChasePlayer::Run()
{
  //DebugLog("[TaskChasePlayer]\n");

  Position playerPos = { _playerRef->PosX, _playerRef->PosY };
  Position objPos = { _objectToControl->PosX, _objectToControl->PosY };

  Pathfinder pf;

  auto path = pf.BuildRoad(Map::Instance().CurrentLevel,
                           objPos,
                           playerPos,
                           std::vector<char>(),
                           false,
                           true);

  /*
  auto copy = path;
  while (!copy.empty())
  {
    auto pos = copy.top();
    DebugLog("[%i %i] ", pos.X, pos.Y);
    copy.pop();
  }

  DebugLog("\n");
  */

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
