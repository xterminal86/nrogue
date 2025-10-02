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
