#include "task-move-away-from-player.h"

#include "map.h"
#include "player.h"

BTResult TaskMoveAwayFromPlayer::Run()
{
  int px = _playerRef->PosX;
  int py = _playerRef->PosY;

  auto res = Map::Instance().GetWalkableCellsAround({ _objectToControl->PosX, _objectToControl->PosY });
  if (res.empty())
  {
    return BTResult::Failure;
  }

  int maxDistance = 0;
  Position posToWalk;

  for (auto& c : res)
  {
    int d = Util::BlockDistance(c.X, c.Y, px, py);
    if (d > maxDistance)
    {
      maxDistance = d;
      posToWalk = c;
    }
  }

  // posToWalk cell should be empty so MoveTo() should succeed
  _objectToControl->MoveTo(posToWalk);
  _objectToControl->FinishTurn();

  return BTResult::Success;
}