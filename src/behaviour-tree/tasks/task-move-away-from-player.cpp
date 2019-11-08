#include "task-move-away-from-player.h"

#include "map.h"
#include "player.h"

BTResult TaskMoveAwayFromPlayer::Run()
{
  //printf("[TaskMoveAwayFromPlayer]\n");

  // FIXME: improve, try to figure out how to
  // go away from the player without just randomly picking
  // a tile with distance greater that 1.
  // Right now enemy can move to the wrong tile
  // or immediately go to chase task thus moving TO the player instead
  // of avoiding him.

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

  // posToWalk cell should be empty so MoveTo() should succeed
  _objectToControl->MoveTo(posToWalk);
  _objectToControl->FinishTurn();

  return BTResult::Success;
}
