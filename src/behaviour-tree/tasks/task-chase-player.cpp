#include "task-chase-player.h"

#include "game-object.h"
#include "application.h"
#include "pathfinder.h"
#include "map.h"
#include "blackboard.h"

BTResult TaskChasePlayer::Run()
{
  Position playerPos = { _playerRef->PosX, _playerRef->PosY };
  Position objPos = { _objectToControl->PosX, _objectToControl->PosY };

  Pathfinder pf;

  if (_lastPlayerPos != playerPos)
  {
    _lastPlayerPos = playerPos;

    std::string plX = std::to_string(playerPos.X);
    std::string plY = std::to_string(playerPos.Y);

    Blackboard::Instance().Set(_objectToControl->ObjectId(), { "pl_x", plX });
    Blackboard::Instance().Set(_objectToControl->ObjectId(), { "pl_y", plY });

    _path = pf.BuildRoad(Map::Instance().CurrentLevel,
                         objPos,
                         playerPos,
                         std::vector<char>(),
                         true);

    if (_path.empty())
    {
      return BTResult::Failure;
    }
  }

  if (!_path.empty())
  {
    auto moveTo = _path.top();
    if (_objectToControl->MoveTo(moveTo))
    {
      _path.pop();
      _objectToControl->FinishTurn();
      return BTResult::Success;
    }
  }

  return BTResult::Failure;
}
