#include "trigger-handlers.h"
#include "printer.h"
#include "util.h"

namespace TriggerHandlers
{
  void Report()
  {
    Printer::Instance().AddMessage("Trigger test", "#FF0000");
    Printer::Instance().DrawExplosion({ 10, 10 }, 3);
  }
}
