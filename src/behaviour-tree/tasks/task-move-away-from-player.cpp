#include "task-move-away-from-player.h"

#include "map.h"
#include "player.h"

BTResult TaskMoveAwayFromPlayer::Run()
{
  //DebugLog("[TaskMoveAwayFromPlayer]\n");

  // FIXME: rewrite to use potential field map emanating from player

  int px = _playerRef->PosX;
  int py = _playerRef->PosY;

  auto res = Map::Instance().GetWalkableCellsAround({ _objectToControl->PosX, _objectToControl->PosY });
  if (res.empty())
  {
    return BTResult::Failure;
  }

  int maxDistance = 1;
  Position posToWalk;

  bool found = false;
  for (auto& c : res)
  {
    int d = Util::BlockDistance(c.X, c.Y, px, py);
    if (d > maxDistance)
    {
      maxDistance = d;
      posToWalk = c;
      found = true;
    }
  }

  if (!found)
  {
    return BTResult::Failure;
  }

  //DebugLog("Found [%i %i] (%i)\n", posToWalk.X, posToWalk.Y, maxDistance);

  // posToWalk cell should be empty so MoveTo() should succeed
  _objectToControl->MoveTo(posToWalk);
  _objectToControl->FinishTurn();

  return BTResult::Success;
}
