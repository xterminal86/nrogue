#ifndef USE_SDL
#include <ncurses.h>
#endif

#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <memory>

#include "application.h"
#include "game-objects-factory.h"
#include "map.h"
#include "util.h"
#include "rng.h"

#ifdef RUN_TESTS
#include "tests.h"
#endif

// TODO:
//
// Status effects
// Ranged weapons / throwing
// Wands
// Scrolls
// Level design
// Skills
// Magic / Rare / Unique items
// Traps, secrets
// Locked doors / chests

// NOTE: When building in Windows, main() must have "correct" signature
// i.e. (int argc, char* agrv[]) or you'll get "undefined reference to SDLmain"
int main(int argc, char* argv[])
{
  RNG::Instance().Init();
  RNG::Instance().SetSeed(1547020537474375167);
  //RNG::Instance().SetSeed(1);
  //RNG::Instance().SetSeed(1544714037606745311);
  //RNG::Instance().SetSeed(1545127588351497486);

  Logger::Instance().Init();
  Logger::Instance().Prepare(true);

  auto str = Util::StringFormat("World seed is %lu", RNG::Instance().Seed);
  Logger::Instance().Print(str);

  GameObjectsFactory::Instance().Init();

  Application::Instance().Init();

  Map::Instance().Init();

  Application::Instance().Run();

  Application::Instance().Cleanup();

#ifdef RUN_TESTS
  Tests::Run();
#endif

  return 0;
}
