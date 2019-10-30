#include "task-idle.h"

#include "game-object.h"

BTResult TaskIdle::Run()
{
  _objectToControl->FinishTurn();
  return BTResult::Success;
}
