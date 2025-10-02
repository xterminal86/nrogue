#include "trigger-handlers.h"
#include "printer.h"
#include "util.h"

namespace TriggerHandlers
{
  void Report()
  {
    Game::gPrnt.AddMessage("I'M TRIGGERED",
                           Colors::BlackColor,
                           Colors::YellowColor);
    Game::gPrnt.DrawExplosion({ 10, 10 }, 3);
  }
}
