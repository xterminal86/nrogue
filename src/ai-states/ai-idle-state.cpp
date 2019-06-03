#include "ai-idle-state.h"

#include "game-object.h"

void AIIdleState::Enter()
{
}

void AIIdleState::Exit()
{
}

void AIIdleState::Run()
{
  _objectToControl->FinishTurn();
}

