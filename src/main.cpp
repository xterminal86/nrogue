#include "application.h"
#include "spells-processor.h"
#include "game-objects-factory.h"
#include "monsters-inc.h"
#include "items-factory.h"
#include "spells-database.h"
#include "map.h"
#include "util.h"
#include "rng.h"
#include "blackboard.h"
#include "logger.h"
#include "timer.h"

#ifdef RUN_TESTS
#include "tests.h"
#endif

// NOTE: When building with SDL2 in Windows, main() must have "full" signature
// i.e. 'int main (int argc, char* agrv[])'
// or you'll get "undefined reference to SDLmain" because SDL wraps main()
// under the hood, so the signature must match.
// By the way, it should be exactly 'char* argv[]', not 'char**' or anything.
//
// You also need to manually "Add" -> "File" in "Projects" tab of QT Creator's
// cmake config window called SDL2_LIBRARY and point it to libSDL2.dll.a file.
//
// Or, as it seems nowadays, there is an 'sdl2-config.cmake' provided with
// development version of SDL2, so it will be picked up by cmake automatically
// and it will tell you what to do.
//
// The only problem I had is that I had to manually specify unconfigured
// cmake variables in QT Creator's "Projects" tab
// (i.e. SDL2_LIBRARY, SDL2_DIR and so on).
// Apparently cmake won't add them into project configuration as unset variables,
// so I had to look into the cmake output log to see what variable I need to add
// and what kind of (i.e. directory for SDL2_DIR, file for SDL2_LIBRARY etc).
//
// Well, anyway, I don't think it will be much of a problem,
// since even such generic person such as myself managed to figure it out.
// You'll figure it out too.
//
// In Linux everything should configure and build out of the box,
// provided you have distro specific ncurses-dev / SDL2-dev
// packages installed.
//

//
// TODO: obfuscate every important stuff when development is finished (lol)
//
int main(int argc, char* argv[])
{
  RNG::Instance().Init();
  //RNG::Instance().SetSeed("Hello World!");
  //RNG::Instance().SetSeed("DEADBEEF");
  //RNG::Instance().SetSeed(1547020537474375167);
  //RNG::Instance().SetSeed(1);
  //RNG::Instance().SetSeed(1544714037606745311);
  //RNG::Instance().SetSeed(1545127588351497486);
  //RNG::Instance().SetSeed(1555423752080957420);

  Blackboard::Instance().Init();
  Timer::Instance().Init();

#ifdef RELEASE_BUILD
  bool printLog = false;
#else
  bool printLog = true;
#endif

  Logger::Instance().Init();
  Logger::Instance().Prepare(printLog);

  auto str = Util::StringFormat("World seed is %lu", RNG::Instance().Seed);
  DebugLog("%s", str.data());
  Logger::Instance().Print(str);

  GameObjectsFactory::Instance().Init();
  ItemsFactory::Instance().Init();
  MonstersInc::Instance().Init();

  SpellsDatabase::Instance().Init();
  SpellsProcessor::Instance().Init();

  Application::Instance().Init();

  Map::Instance().Init();

  Application::Instance().Run();
  Application::Instance().Cleanup();

#ifdef RUN_TESTS
  DebugLog("Running tests, this may take a while...\n");
  Tests::Run();
#endif

  return 0;
}
