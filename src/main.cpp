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

#include "level-builder.h"

#ifdef RUN_TESTS
#include "tests.h"
#endif

// BUG:
// 1) Possible bug with level up not showing / working after killing with ranged weapon
// 2) Hitting spider via move-attack acted like skipping turns; possible bug in speed system.

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

// NOTE: When building with SDL2 in Windows, main() must have "full" signature
// i.e. (int argc, char* agrv[]) or you'll get "undefined reference to SDLmain"
//
// You also need to manually "Add" -> "File" in "Projects" tab of QT Creator's
// cmake config window called SDL2_LIBRARY and point it to libSDL2.dll.a file.
//
// In Linux everything should configure and build without problems,
// provided you have distro specific ncurses-dev / SDL2-dev, SDL2_image-dev
// packages installed.
int main(int argc, char* argv[])
{
  RNG::Instance().Init();
  //RNG::Instance().SetSeed(1547020537474375167);
  //RNG::Instance().SetSeed(1);
  //RNG::Instance().SetSeed(1544714037606745311);
  //RNG::Instance().SetSeed(1545127588351497486);
  //RNG::Instance().SetSeed(1555423752080957420);

  #ifdef RELEASE_BUILD
  bool printLog = false;
  #else
  bool printLog = true;
  #endif

  Logger::Instance().Init();
  Logger::Instance().Prepare(printLog);

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
