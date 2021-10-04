#include "trigger-handlers.h"
#include "printer.h"
#include "util.h"

namespace TriggerHandlers
{
  void Report()
  {
    Printer::Instance().AddMessage("I'M TRIGGERED", "#000000", "#FFFF00");
    Printer::Instance().DrawExplosion({ 10, 10 }, 3);
  }
}
