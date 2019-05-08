#include "ai-patrol-state.h"

#include "game-object.h"
#include "rng.h"

void AIPatrolState::Run()
{
  if (_objectToControl->Attrs.ActionMeter < GlobalConstants::TurnReadyValue)
  {
    _objectToControl->WaitForTurn();
    return;
  }

  int dx = RNG::Instance().Random() % 2;
  int dy = RNG::Instance().Random() % 2;

  int signX = (RNG::Instance().Random() % 2) == 0 ? -1 : 1;
  int signY = (RNG::Instance().Random() % 2) == 0 ? -1 : 1;

  dx *= signX;
  dy *= signY;

  if (_objectToControl->Move(dx, dy))
  {
    _objectToControl->FinishTurn();
  }
}

void AIPatrolState::Exit()
{
}

void AIPatrolState::Enter()
{
}
