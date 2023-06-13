#include "task-print-message.h"

#include "game-object.h"
#include "printer.h"

TaskPrintMessage::TaskPrintMessage(GameObject *objectToControl, const std::string& debugMessage)
  : Node(objectToControl)
{
  _message = debugMessage;
}

// =============================================================================

BTResult TaskPrintMessage::Run()
{
  Printer::Instance().AddMessage(_message);

  DebugLog("%s", _message.data());

  return BTResult::Success;
}
