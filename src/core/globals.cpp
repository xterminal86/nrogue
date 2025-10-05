#include "globals.h"

// =============================================================================
#ifdef DEBUG_BUILD
//#include "game-object.h"

std::unordered_map<void*, void*> AnyObjectByAddr;
std::unordered_map<uint64_t, GameObject*> GameObjectsById;
#endif
// =============================================================================

#include "gid-generator.h"
#include "rng.h"
#include "blackboard.h"
#include "timer.h"

#ifdef DEBUG_BUILD
#include "logger.h"
#endif

#include "bts-decompiler.h"
#include "printer.h"
#include "application.h"
#include "game-objects-factory.h"
#include "items-factory.h"
#include "monsters-inc.h"
#include "spells-database.h"
#include "spells-processor.h"
#include "map.h"

namespace
{
  //
  // According to information:
  //
  // "Global variables in a single translation unit (source file) are
  // initialized in the order in which they are defined. "
  //
  // So there should be no problem.
  //
  GID        GidInst;
  RNG        RngInst;
  Blackboard BlackboardInst;
  Timer      TimerInst;

#ifdef DEBUG_BUILD
  Logger LoggerInst;
#endif

  BTSDecompiler      BtsInst;
  Printer            PrntInst;
  Application        AppInst;
  GameObjectsFactory GOFInst;
  ItemsFactory       IFInst;
  MonstersInc        MIInst;
  SpellsDatabase     SDInst;
  SpellsProcessor    SPInst;
  Map                MapInst;
}

namespace Game
{
  GID               &gGid = GidInst;
  RNG               &gRng = RngInst;
  Blackboard         &gBB = BlackboardInst;
  Timer           &gTimer = TimerInst;

#ifdef DEBUG_BUILD
  Logger& gLogger = LoggerInst;
#endif

  BTSDecompiler      &gBts  = BtsInst;
  Printer            &gPrnt = PrntInst;
  Application        &gApp  = AppInst;
  GameObjectsFactory &gGOF  = GOFInst;
  ItemsFactory       &gIF   = IFInst;
  MonstersInc        &gMI   = MIInst;
  SpellsDatabase     &gSD   = SDInst;
  SpellsProcessor    &gSP   = SPInst;
  Map                &gMap  = MapInst;

  bool Init(MapType levelToLoad)
  {
    // Initialization order (if applicable):
    //
    // GID
    // RNG
    // Blackboard
    // Timer
    // ifdef DEBUG_BUILD
    //   Logger
    //   Logger.Prepare(true)
    // endif
    // BTSDecompiler
    // Application
    //
    // if (!Application.Ready())
    //   exit
    //
    // GameObjectsFactory
    // ItemsFactory
    // MonstersInc
    // SpellsDatabase
    // SpellsProcessor
    // Map
    //
    // App.Run()
    //

    gGid.Init();
    gRng.Init();

#ifdef DEBUG_BUILD
    gLogger.Prepare(true);
#endif

    gBts.Init();
    gApp.Init((levelToLoad != MapType::NOWHERE));

    if (!gApp.IsAppReady())
    {
      ConsoleLog("There was an error during application initialization - "
                 "no sense in continuing");
      return false;
    }

    gGOF.Init();
    gIF.Init();
    gSD.Init();
    gSP.Init();
    gMap.Init();

#ifdef BUILD_TESTS
    if (levelToLoad != MapType::NOWHERE)
    {
      gMap.LoadLevel(levelToLoad);
    }
#endif

    return true;
  }

  void Run()
  {
    if (gApp.IsAppReady())
    {
      gApp.Run();
    }
    else
    {
      ConsoleLog("Game was not initialized - call gApp.Init() first!");
    }
  }

  void Shutdown()
  {
    gApp.Cleanup();
  }
}

