#include "ai-idle-state.h"

#include "game-object.h"

void AIIdleState::Run()
{
  _objectToControl->FinishTurn();
}

void AIIdleState::Exit()
{
}

void AIIdleState::Enter()
{
}
