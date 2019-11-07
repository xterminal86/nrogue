#include "task-idle.h"

#include "game-object.h"

BTResult TaskIdle::Run()
{
  //printf("[TaskIdle]\n");

  _objectToControl->FinishTurn();
  return BTResult::Success;
}
