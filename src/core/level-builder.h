#ifndef LEVELBUILDER_H
#define LEVELBUILDER_H

#include <stack>
#include <queue>

#include "util.h"
#include "room-helper.h"

#include "dg-base.h"

class LevelBuilder
{
  public:
    void BuildLevelFromLayouts(std::vector<RoomForLevel>& possibleRooms,
                                  int startX, int startY,
                                  int mapSizeX, int mapSizeY);

    void RecursiveBacktrackerMethod(Position mapSize,
                                    Position startingPoint = { -1, -1 });

    void TunnelerMethod(Position mapSize,
                          int maxIterations,
                          Position tunnelLengthMinMax,
                          Position start = { -1, -1 } );

    void BacktrackingTunnelerMethod(Position mapSize,
                                       Position tunnelLengthMinMax,
                                       Position start = { - 1, -1 },
                                       bool additionalTweaks = false);

    void CellularAutomataMethod(Position mapSize,
                                   int initialWallChance,
                                   int birthThreshold,
                                   int deathThreshold,
                                   int maxIterations);

    void FeatureRoomsMethod(Position mapSize, Position roomSizes, int maxIterations);
    void RoomsMethod(Position mapSize, Position splitRatio, int minRoomSize);

    void LogPrintMapRaw();
    void PrintMapRaw();

    std::vector<std::vector<char>> MapRaw;

  private:
    std::unique_ptr<DGBase> _generator;
};

#endif // LEVELBUILDER_H
