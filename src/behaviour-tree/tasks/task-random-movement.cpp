#include "task-random-movement.h"

#include "map.h"
#include "rng.h"
#include "game-object.h"

BTResult TaskRandomMovement::Run()
{
  //printf("[TaskRandomMovement]\n");

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
