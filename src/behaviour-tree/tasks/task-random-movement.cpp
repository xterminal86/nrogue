#include "task-random-movement.h"

#include "map.h"
#include "rng.h"
#include "game-object.h"

BTResult TaskRandomMovement::Run()
{
  //DebugLog("[TaskRandomMovement]\n");

  return Smart();
  //return Dumb();
}

// =============================================================================

BTResult TaskRandomMovement::Smart()
{
  auto cells =
      Util::GetEightPointsAround(_objectToControl->GetPosition(),
                                 Game::gMap.CurrentLevel->MapSize);

  std::vector<Position> cellsToMove;
  for (auto& c : cells)
  {
    MapLevelBase* curLvl = Game::gMap.CurrentLevel;

    bool isOk       = !curLvl->MapArray[c.X][c.Y]->Special;
    bool isOccupied = curLvl->MapArray[c.X][c.Y]->Occupied;

    if (isOk && !isOccupied)
    {
      cellsToMove.push_back(c);
    }
  }

  if (!cellsToMove.empty())
  {
    int index = Game::gRng.RandomRange(0, cellsToMove.size());
    _objectToControl->MoveTo(cellsToMove[index].X, cellsToMove[index].Y);
    _objectToControl->FinishTurn();
    return BTResult::Success;
  }

  return BTResult::Failure;
}

// =============================================================================

//
// Doesn't check if cell is actually can be walked into.
//
BTResult TaskRandomMovement::Dumb()
{
  int dx = Game::gRng.Random() % 2;
  int dy = Game::gRng.Random() % 2;

  int signX = (Game::gRng.Random() % 2) == 0 ? -1 : 1;
  int signY = (Game::gRng.Random() % 2) == 0 ? -1 : 1;

  dx *= signX;
  dy *= signY;

  Position newPos =
  {
    _objectToControl->PosX + dx,
    _objectToControl->PosY + dy
  };

  MapLevelBase* curLvl = Game::gMap.CurrentLevel;

  bool isOk = !curLvl->MapArray[newPos.X][newPos.Y]->Special;

  if (isOk && _objectToControl->Move(dx, dy))
  {
    _objectToControl->FinishTurn();
    return BTResult::Success;
  }

  return BTResult::Failure;
}
