#ifndef LEVELBUILDER_H
#define LEVELBUILDER_H

#include <memory>

#include "dg-base.h"

class LevelBuilder
{
  public:
    void FromBlobTiles(int mapSizeX, int mapSizeY,
                       int tileSizeFactor,
                       int wallsSizeFactor,
                       bool postProcess = false);

    void RecursiveBacktrackerMethod(const Position& mapSize,
                                    const Position& startingPoint = { -1, -1 },
                                    const RemovalParams& endWallsRemovalParams = RemovalParams());

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

    void BSPRoomsMethod(const Position& mapSize,
                        const Position& splitRatio,
                        int minRoomSize);

    void RoomsMethod(const Position& mapSize,
                     const Position& roomSizes,
                     int maxIterations);

    void FromPermutationTilesMethod(const Position& mapSize,
                                    int tileSetIndex = -1,
                                    bool postProcess = false,
                                    bool removeBias = false);

    // -------------------------------------------------------------------------

    void PlaceShrineLayout(const Position& start,
                           const StringV& layout);

    void LogPrintMapRaw();
    void PrintMapRaw();

    double GetEmptyPercent();
    double GetEmptyOverWallsRatio();

    void UpdateMapRawFromCurrentGenerator();

    std::string GetMapRawString();

    MapCell* GetMapCell(int x, int y);

    const std::vector<Rect>& GetEmptyRooms();
    void TransformRooms(const TransformedRoomsWeights& weights);

    void PrintCustomDebugStuff();

    const std::vector<std::vector<MapCell>>& GeneratedMap();

    CharV2 MapRaw;

    //
    // Cannot replace with unordered_map bc of key hash.
    //
    std::map<Position, ShrineType>& ShrinesByPosition();

  private:
    std::unique_ptr<DGBase> _generator;

    MapCell _cellInfo;

    std::vector<Rect> _emptyRoomsStub;

    std::vector<std::vector<MapCell>> _emptyGeneratedMapStub;
};

#endif // LEVELBUILDER_H
