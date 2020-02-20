#ifndef LEVELBUILDER_H
#define LEVELBUILDER_H

#include <stack>
#include <queue>
#include <memory>

#include "room-helper.h"

#include "dg-base.h"

class LevelBuilder
{
  public:
    void BuildLevelFromLayouts(std::vector<RoomForLevel>& possibleRooms,
                               int startX, int startY,
                               int mapSizeX, int mapSizeY);

    void RecursiveBacktrackerMethod(const Position& mapSize,
                                    const Position& startingPoint = { -1, -1 });

    void TunnelerMethod(const Position& mapSize,
                        int maxIterations,
                        const Position& tunnelLengthMinMax,
                        const Position& start = { -1, -1 } );

    void BacktrackingTunnelerMethod(const Position& mapSize,
                                    const Position& tunnelLengthMinMax,
                                    const Position& start = { - 1, -1 },
                                    bool additionalTweaks = false);

    void CellularAutomataMethod(const Position& mapSize,
                                int initialWallChance,
                                int birthThreshold,
                                int deathThreshold,
                                int maxIterations);

    void FeatureRoomsMethod(const Position& mapSize,
                            const Position& roomSizes,                            
                            const FeatureRoomsWeights& weightsMap,
                            int doorPlacementChance,
                            int maxIterations);

    void RoomsMethod(const Position& mapSize,
                     const Position& splitRatio,
                     int minRoomSize);

    void PlaceLayout(const Position& start,
                     const StringsArray2D& layout,
                     const std::vector<char>& tilesToIgnore = std::vector<char>());

    void LogPrintMapRaw();
    void PrintMapRaw();

    bool WasUsed();

    std::string GetMapRawString();

    std::vector<std::vector<char>> MapRaw;

    std::map<Position, ShrineType>& ShrinesByPosition();

  private:
    std::unique_ptr<DGBase> _generator;
};

#endif // LEVELBUILDER_H
