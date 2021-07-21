#include "task-debug.h"

#include "game-object.h"

TaskDebug::TaskDebug(GameObject *objectToControl, const std::string& debugMessage)
  : Node(objectToControl)
{
  _message = debugMessage;
}

BTResult TaskDebug::Run()
{
  DebugLog("%s", _message.data());

  return BTResult::Success;
}
