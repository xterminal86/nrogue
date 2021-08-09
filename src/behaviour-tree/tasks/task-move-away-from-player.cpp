#include "task-move-away-from-player.h"

#include "map.h"
#include "player.h"

BTResult TaskMoveAwayFromPlayer::Run()
{
  //DebugLog("[TaskMoveAwayFromPlayer]\n");

  Position objPos    = { _objectToControl->PosX, _objectToControl->PosY };
  Position playerPos = { _playerRef->PosX, _playerRef->PosY };

  auto res = Map::Instance().GetWalkableCellsAround(objPos);
  if (res.empty())
  {
    return BTResult::Failure;
  }

  if (_playerRef->DistanceField.IsDirty())
  {
    _playerRef->DistanceField.Emanate();
  }

  DijkstraMap::Cell* cellFound = nullptr;

  int maxCost = 0;
  for (auto& p : res)
  {
    DijkstraMap::Cell* c = _playerRef->DistanceField.GetCell(p.X, p.Y);
    if (c != nullptr && c->Cost > maxCost)
    {
      maxCost = c->Cost;
      cellFound = c;
    }
  }

  if (cellFound == nullptr)
  {
    return BTResult::Failure;
  }

  bool playerCanAttackThere = Util::IsObjectInRange(cellFound->MapPos, playerPos, 1, 1);
  if (playerCanAttackThere)
  {
    return BTResult::Failure;
  }

  _objectToControl->MoveTo(cellFound->MapPos);
  _objectToControl->FinishTurn();

  return BTResult::Success;
}
