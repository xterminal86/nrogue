#include "task-move-away-from-player.h"

#include "map.h"
#include "player.h"

BTResult TaskMoveAwayFromPlayer::Run()
{
  //DebugLog("[TaskMoveAwayFromPlayer]\n");

  Position objPos    = _objectToControl->GetPosition();
  Position playerPos = _playerRef->GetPosition();

  auto res = Map::Instance().GetWalkableCellsAround(objPos);

  // In case when actor moved himself into a corner,
  // corner will be the cell with maximum cost, so this
  // prevents frantic movement from and to the corner.
  res.push_back(objPos);

  if (_playerRef->DistanceField.IsDirty())
  {
    _playerRef->DistanceField.Emanate();
  }

  PotentialField::Cell* cellFound = nullptr;

  int maxCost = 0;
  for (auto& p : res)
  {
    PotentialField::Cell* c = _playerRef->DistanceField.GetCell(p.X, p.Y);
    if (c != nullptr &&
        c->Cost > maxCost &&
        c->Cost != PotentialField::kBlockedCellCost)
    {
      maxCost = c->Cost;
      cellFound = c;
    }
  }

  if (cellFound == nullptr)
  {
    return BTResult::Failure;
  }

  bool playerCanAttackThere = Util::Instance().IsObjectInRange(cellFound->MapPos, playerPos, 1, 1);
  if (playerCanAttackThere)
  {
    return BTResult::Failure;
  }

  _objectToControl->MoveTo(cellFound->MapPos);
  _objectToControl->FinishTurn();

  return BTResult::Success;
}
