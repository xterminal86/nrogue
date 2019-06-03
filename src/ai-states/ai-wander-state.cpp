#include "ai-wander-state.h"
#include "ai-model-base.h"

#include "game-object.h"
#include "rng.h"

void AIWanderState::Enter()
{
}

void AIWanderState::Exit()
{
}

void AIWanderState::Run()
{
  if (_objectToControl->Attrs.ActionMeter < GlobalConstants::TurnReadyValue)
  {
    _objectToControl->WaitForTurn();
    return;
  }

  if (_ownerModel->RandomMovement())
  {
    _objectToControl->FinishTurn();
  }
}
