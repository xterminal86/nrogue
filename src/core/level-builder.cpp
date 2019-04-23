#include "level-builder.h"

#include "rng.h"

#include "feature-rooms.h"
#include "recursive-backtracker.h"
#include "tunneler.h"
#include "cellular-automata.h"
#include "from-layouts.h"
#include "rooms.h"

void LevelBuilder::FeatureRoomsMethod(const Position& mapSize,
                                      const Position& roomSizes,                                      
                                      const FeatureRoomsWeights& weightsMap,
                                      int doorPlacementChance,
                                      int maxIterations)
{
  _generator.reset(new FeatureRooms());

  FeatureRooms* fr = static_cast<FeatureRooms*>(_generator.get());
  fr->Generate(mapSize, roomSizes, weightsMap, doorPlacementChance, maxIterations);

  MapRaw = fr->MapRaw;
}

void LevelBuilder::CellularAutomataMethod(const Position& mapSize, int initialWallChance,
                                    int birthThreshold, int deathThreshold,
                                    int maxIterations)
{
  _generator.reset(new CellularAutomata());

  CellularAutomata* ca = static_cast<CellularAutomata*>(_generator.get());
  ca->Generate(mapSize, initialWallChance, birthThreshold, deathThreshold, maxIterations);

  MapRaw = ca->MapRaw;
}

void LevelBuilder::BacktrackingTunnelerMethod(const Position& mapSize, const Position& tunnelMinMax, const Position& start, bool additionalTweaks)
{
  _generator.reset(new Tunneler());

  Tunneler* t = static_cast<Tunneler*>(_generator.get());
  t->Backtracking(mapSize, tunnelMinMax, start, additionalTweaks);

  MapRaw = t->MapRaw;
}

void LevelBuilder::TunnelerMethod(const Position& mapSize, int maxIterations, const Position& tunnelLengthMinMax, const Position& start)
{
  _generator.reset(new Tunneler());

  Tunneler* t = static_cast<Tunneler*>(_generator.get());
  t->Normal(mapSize, tunnelLengthMinMax, start, maxIterations, true);

  MapRaw = t->MapRaw;
}

void LevelBuilder::RecursiveBacktrackerMethod(const Position& mapSize, const Position& startingPoint)
{
  _generator.reset(new RecursiveBacktracker());

  RecursiveBacktracker* rb = static_cast<RecursiveBacktracker*>(_generator.get());
  rb->Generate(mapSize, startingPoint);

  MapRaw = rb->MapRaw;
}

// FIXME: build from layouts needs improvements
// (see comments in Generate() method)
// Use carefully, better avoid using altogether.
void LevelBuilder::BuildLevelFromLayouts(std::vector<RoomForLevel>& possibleRooms, int startX, int startY, int mapSizeX, int mapSizeY)
{
  _generator.reset(new FromLayouts());

  FromLayouts* fl = static_cast<FromLayouts*>(_generator.get());
  fl->Generate(possibleRooms, startX, startY, mapSizeX, mapSizeY);

  MapRaw = fl->MapRaw;
}

void LevelBuilder::RoomsMethod(const Position& mapSize,
                               const Position& splitRatio,
                               int minRoomSize)
{
  _generator.reset(new Rooms());

  Rooms* fl = static_cast<Rooms*>(_generator.get());
  fl->Generate(mapSize, splitRatio, minRoomSize);

  MapRaw = fl->MapRaw;
}

void LevelBuilder::PrintMapRaw()
{
  if (_generator)
  {
    _generator->PrintMapRaw();
  }
}

void LevelBuilder::LogPrintMapRaw()
{
  if (_generator)
  {
    _generator->LogPrintMapRaw();
  }
}

const std::map<Position, ShrineType>& LevelBuilder::ShrinesByPosition()
{
  return _generator->ShrinesByPosition;
}
