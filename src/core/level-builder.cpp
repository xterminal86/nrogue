#include "level-builder.h"

#include "rng.h"

#include "feature-rooms.h"
#include "recursive-backtracker.h"
#include "tunneler.h"
#include "cellular-automata.h"
#include "blob-tiles.h"
#include "from-permutation-tiles.h"
#include "bsp-rooms.h"
#include "rooms.h"
#include "util.h"

void LevelBuilder::FeatureRoomsMethod(const Position& mapSize,
                                      const Position& roomSizes,
                                      const FeatureRoomsWeights& weightsMap,
                                      uint8_t doorPlacementChance,
                                      int maxIterations)
{
  _generator.reset(new FeatureRooms());

  FeatureRooms* fr = static_cast<FeatureRooms*>(_generator.get());
  fr->Generate(mapSize, roomSizes, weightsMap, doorPlacementChance, maxIterations);

  MapRaw = fr->MapRaw;
}

// =============================================================================

void LevelBuilder::CellularAutomataMethod(const Position& mapSize,
                                          int initialWallChance,
                                          int birthThreshold,
                                          int deathThreshold,
                                          int maxIterations)
{
  _generator.reset(new CellularAutomata());

  CellularAutomata* ca = static_cast<CellularAutomata*>(_generator.get());
  ca->Generate(mapSize, initialWallChance, birthThreshold, deathThreshold, maxIterations);

  MapRaw = ca->MapRaw;
}

// =============================================================================

void LevelBuilder::BacktrackingTunnelerMethod(const Position& mapSize,
                                              const Position& tunnelMinMax,
                                              const Position& start,
                                              bool additionalTweaks)
{
  _generator.reset(new Tunneler());

  Tunneler* t = static_cast<Tunneler*>(_generator.get());
  t->Backtracking(mapSize, tunnelMinMax, start, additionalTweaks);

  MapRaw = t->MapRaw;
}

// =============================================================================

void LevelBuilder::TunnelerMethod(const Position& mapSize,
                                  int maxIterations,
                                  const Position& tunnelLengthMinMax,
                                  const Position& start)
{
  _generator.reset(new Tunneler());

  Tunneler* t = static_cast<Tunneler*>(_generator.get());
  t->Normal(mapSize, tunnelLengthMinMax, start, maxIterations, true);

  MapRaw = t->MapRaw;
}

// =============================================================================

void LevelBuilder::RecursiveBacktrackerMethod(const Position& mapSize,
                                              const Position& startingPoint,
                                              const RemovalParams& endWallsRemovalParams)
{
  _generator.reset(new RecursiveBacktracker());

  RecursiveBacktracker* rb = static_cast<RecursiveBacktracker*>(_generator.get());
  rb->Generate(mapSize, startingPoint, endWallsRemovalParams);

  MapRaw = rb->MapRaw;
}

// =============================================================================

void LevelBuilder::FromBlobTiles(int mapSizeX, int mapSizeY,
                                 int tileSizeFactor,
                                 int wallsSizeFactor,
                                 bool postProcess)
{
  _generator.reset(new BlobTiles());

  BlobTiles* bt = static_cast<BlobTiles*>(_generator.get());
  bt->Generate(mapSizeX, mapSizeY, tileSizeFactor, wallsSizeFactor, postProcess);

  MapRaw = bt->MapRaw;
}

// =============================================================================

void LevelBuilder::BSPRoomsMethod(const Position& mapSize,
                                  const Position& splitRatio,
                                  int minRoomSize)
{
  _generator.reset(new BSPRooms());

  BSPRooms* fl = static_cast<BSPRooms*>(_generator.get());
  fl->Generate(mapSize, splitRatio, minRoomSize);

  MapRaw = fl->MapRaw;
}

// =============================================================================

void LevelBuilder::RoomsMethod(const Position& mapSize,
                               const Position& roomSizes,
                               int maxIterations)
{
  _generator.reset(new Rooms());

  Rooms* r = static_cast<Rooms*>(_generator.get());
  r->Generate(mapSize, roomSizes, maxIterations);

  MapRaw = r->MapRaw;
}

// =============================================================================

void LevelBuilder::FromPermutationTilesMethod(const Position& mapSize,
                                              int tileSetIndex,
                                              bool postProcess,
                                              bool removeBias)
{
  _generator.reset(new FromPermutationTiles());

  FromPermutationTiles* ft = static_cast<FromPermutationTiles*>(_generator.get());
  ft->Generate(mapSize, tileSetIndex, postProcess, removeBias);

  MapRaw = ft->MapRaw;
}

// =============================================================================

void LevelBuilder::PlaceShrineLayout(const Position& start,
                               const StringV& layout)
{
  int sx = start.X;
  int sy = start.Y;
  int ex = sx + layout.size();
  int ey = sy + layout[0].size();

  int lx = 0;
  int ly = 0;

  for (int x = sx; x < ex; x++)
  {
    for (int y = sy; y < ey; y++)
    {
      const char& l = layout[ly][lx];

      MapRaw[x][y] = l;

      ly++;
    }

    lx++;
    ly = 0;
  }

  //
  // Ensure shrine is always accessible
  // by constructing perimeter of empty cells around it.
  //
  // Kinda cheating, since it won't always look fitting
  // to the generated level design but I don't want
  // to mull over other methods and it's better than
  // accidental blocking of level paths.
  //
  for (int x = sx - 1; x < ex + 1; x++)
  {
    MapRaw[x][sy - 1] = '.';
    MapRaw[x][ey]     = '.';
  }

  for (int y = sy - 1; y < ey + 1; y++)
  {
    MapRaw[sx - 1][y] = '.';
    MapRaw[ex][y]     = '.';
  }
}

// =============================================================================

void LevelBuilder::PrintMapRaw()
{
  if (_generator)
  {
    _generator->PrintMapRaw();
  }
}

// =============================================================================

void LevelBuilder::LogPrintMapRaw()
{
  if (_generator)
  {
    _generator->LogPrintMapRaw();
  }
}

// =============================================================================

std::map<Position, ShrineType>& LevelBuilder::ShrinesByPosition()
{
  return _generator->ShrinesByPosition;
}

// =============================================================================

void LevelBuilder::UpdateMapRawFromCurrentGenerator()
{
  if (_generator)
  {
    MapRaw = _generator->MapRaw;
  }
}

// =============================================================================

std::string LevelBuilder::GetMapRawString()
{
  return _generator ? _generator->GetMapRawString() : "_generator is null\n";
}

// =============================================================================

MapCell* LevelBuilder::GetMapCell(int x, int y)
{
  MapCell* res = nullptr;

  if (_generator)
  {
    res = _generator->GetCell(x, y);
  }

  return res;
}

// =============================================================================

const std::vector<Rect>& LevelBuilder::GetEmptyRooms()
{
  if (_generator)
  {
    return _generator->GetEmptyRooms();
  }

  return _emptyRoomsStub;
}

// =============================================================================

void LevelBuilder::TransformRooms(const TransformedRoomsWeights& weights)
{
  if (_generator)
  {
    _generator->GetEmptyRooms();
    _generator->TransformRooms(weights);

    UpdateMapRawFromCurrentGenerator();
  }
}

// =============================================================================

const std::vector<std::vector<MapCell>>& LevelBuilder::GeneratedMap()
{
  return _generator ? _generator->GeneratedMap() : _emptyGeneratedMapStub;
}

// =============================================================================

void LevelBuilder::PrintCustomDebugStuff()
{
  if (_generator)
  {
    _generator->ForCustomDebugStuff();
  }
}

// =============================================================================

double LevelBuilder::GetEmptyPercent()
{
  return _generator ? _generator->GetEmptyPercent() : 0.0;
}

// =============================================================================

double LevelBuilder::GetEmptyOverWallsRatio()
{
  return _generator ? _generator->GetEmptyPercent() : 0.0;
}
