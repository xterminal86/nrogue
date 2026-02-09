#include "trigger-handlers.h"
#include "printer.h"
#include "util.h"

namespace TriggerHandlers
{
  void Report()
  {
    Game::gPrnt.AddMessage("I'M TRIGGERED",
                           Colors::Black,
                           Colors::Yellow);
    Game::gPrnt.DrawExplosion({ 10, 10 }, 3);
  }
}
