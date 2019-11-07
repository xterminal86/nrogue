#include "task-chase-player.h"

#include "game-object.h"
#include "application.h"
#include "pathfinder.h"
#include "map.h"
#include "blackboard.h"

BTResult TaskChasePlayer::Run()
{
  //printf("[TaskChasePlayer]\n");

  Position playerPos = { _playerRef->PosX, _playerRef->PosY };
  Position objPos = { _objectToControl->PosX, _objectToControl->PosY };

  Pathfinder pf;

  std::string plX = std::to_string(playerPos.X);
  std::string plY = std::to_string(playerPos.Y);

  Blackboard::Instance().Set(_objectToControl->ObjectId(), { "pl_x", plX });
  Blackboard::Instance().Set(_objectToControl->ObjectId(), { "pl_y", plY });

  auto path = pf.BuildRoad(Map::Instance().CurrentLevel,
                           objPos,
                           playerPos,
                           std::vector<char>(),
                           true);

  /*
  auto copy = path;
  while (!copy.empty())
  {
    auto pos = copy.top();
    printf("[%i %i] ", pos.X, pos.Y);
    copy.pop();
  }

  printf("\n");
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
