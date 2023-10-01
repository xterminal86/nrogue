#include "application.h"
#include "bts-decompiler.h"
#include "spells-processor.h"
#include "game-objects-factory.h"
#include "monsters-inc.h"
#include "items-factory.h"
#include "spells-database.h"
#include "map.h"
#include "util.h"
#include "rng.h"
#include "blackboard.h"
#include "timer.h"

#ifdef DEBUG_BUILD
#include "logger.h"
#endif

//
// NOTE: When building with SDL2 in Windows,
// main() must have "full" signature,
// i.e. 'int main (int argc, char* agrv[])'
// or you'll get "undefined reference to SDLmain"
// because SDL wraps main() under the hood,
// so the signature must match.
// By the way, IINM, it should be exactly 'char* argv[]',
// not 'char**' or anything.
//
// Also you need to manually "Add" -> "File" in "Projects" tab of QT Creator's
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
// since even such simpleminded person such as myself managed to figure it out.
// You'll figure it out too.
//
// In Linux everything should configure and build out of the box,
// provided you have distro specific ncurses-dev / SDL2-dev
// packages installed.
//

int main(int argc, char* argv[])
{
  RNG::Instance().Init();
  Blackboard::Instance().Init();
  Timer::Instance().Init();

#ifdef DEBUG_BUILD
  Logger::Instance().Init();
  Logger::Instance().Prepare(true);

  auto str = Util::StringFormat("World seed is 0x%lX", RNG::Instance().Seed);
  DebugLog("%s\n\n", str.data());
  LogPrint(str);
#endif

  BTSDecompiler::Instance().Init();

  GameObjectsFactory::Instance().Init();
  ItemsFactory::Instance().Init();
  MonstersInc::Instance().Init();

  SpellsDatabase::Instance().Init();
  SpellsProcessor::Instance().Init();

  Application::Instance().Init();

  if (!Application::Instance().IsAppReady())
  {
    ConsoleLog("There was an error during application initialization - no sense in continuing");
    return 1;
  }

  Map::Instance().Init();

  Application::Instance().Run();
  Application::Instance().Cleanup();

  return 0;
}
