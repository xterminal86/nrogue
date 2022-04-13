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

BTResult TaskRandomMovement::Smart()
{
  Position objPos = { _objectToControl->PosX, _objectToControl->PosY };
  auto cells = Util::GetEightPointsAround(objPos, Map::Instance().CurrentLevel->MapSize);
  std::vector<Position> cellsToMove;
  for (auto& c : cells)
  {
    bool isOk = !Map::Instance().CurrentLevel->MapArray[c.X][c.Y]->Special;
    bool isOccupied = Map::Instance().CurrentLevel->MapArray[c.X][c.Y]->Occupied;

    if (isOk && !isOccupied)
    {
      cellsToMove.push_back(c);
    }
  }

  if (!cellsToMove.empty())
  {
    int index = RNG::Instance().RandomRange(0, cellsToMove.size());
    _objectToControl->MoveTo(cellsToMove[index].X, cellsToMove[index].Y);
    _objectToControl->FinishTurn();
    return BTResult::Success;
  }

  return BTResult::Failure;
}

//
// Doesn't check if cell is actually can be walked into
//
BTResult TaskRandomMovement::Dumb()
{
  int dx = RNG::Instance().Random() % 2;
  int dy = RNG::Instance().Random() % 2;

  int signX = (RNG::Instance().Random() % 2) == 0 ? -1 : 1;
  int signY = (RNG::Instance().Random() % 2) == 0 ? -1 : 1;

  dx *= signX;
  dy *= signY;

  Position newPos = { _objectToControl->PosX + dx, _objectToControl->PosY + dy };
  bool isOk = !Map::Instance().CurrentLevel->MapArray[newPos.X][newPos.Y]->Special;

  if (isOk && _objectToControl->Move(dx, dy))
  {
    _objectToControl->FinishTurn();
    return BTResult::Success;
  }

  return BTResult::Failure;
}
