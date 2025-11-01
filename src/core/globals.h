#ifndef GLOBALS_H
#define GLOBALS_H

// =============================================================================
#ifdef DEBUG_BUILD
#include <unordered_map>
#include <string>
#include <cstdint>

class GameObject;

extern std::unordered_map<void*, void*> AnyObjectByAddr;
extern std::unordered_map<uint64_t, GameObject*> GameObjectsById;
#endif
// =============================================================================

//
// Github's MSYS2 pipeline failes with "undefined reference to SDL_main"
// unless we include this first. My guess is that it's because of recent
// restructuring (commit 68a7532): before that every main.cpp basically
// included SDL.h along the chain of includes that were needed for singletons,
// but after I removed them, main.cpp includes just this file which has no
// "major" includes. So during linking phase linker can't resolve SDL_main
// bullshit for some reason.
// On Linux and Windows with MinGW everything works fine though.
// The usual: another day - same shit.
//
#ifdef USE_SDL
#include "SDL2/SDL.h"
#endif

#include "enumerations.h"

class Timer;
class GID;
class RNG;
class Blackboard;

#ifdef DEBUG_BUILD
class Logger;
#endif

class BTSDecompiler;
class Printer;
class Application;
class GameObjectsFactory;
class ItemsFactory;
class MonstersInc;
class SpellsDatabase;
class SpellsProcessor;
class Map;

namespace Game
{
  extern Timer      &gTimer;
  extern GID        &gGid;
  extern RNG        &gRng;
  extern Blackboard &gBB;
  extern Timer      &gTimer;

#ifdef DEBUG_BUILD
  extern Logger& gLogger;
#endif

  extern BTSDecompiler      &gBts;
  extern Printer            &gPrnt;
  extern Application        &gApp;
  extern GameObjectsFactory &gGOF;
  extern ItemsFactory       &gIF;
  extern MonstersInc        &gMI;
  extern SpellsDatabase     &gSD;
  extern SpellsProcessor    &gSP;
  extern Map                &gMap;

  extern bool Init(MapType levelToLoad = MapType::NOWHERE);
  extern void Run();
  extern void Shutdown();
}

#endif // GLOBALS_H
