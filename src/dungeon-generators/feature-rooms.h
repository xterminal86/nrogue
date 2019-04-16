#ifndef FEATUREROOMS_H
#define FEATUREROOMS_H

#include "dg-base.h"
#include "constants.h"

class FeatureRooms : public DGBase
{  
  using RoomLayout = std::vector<std::vector<std::string>>;

  enum class RoomBuildDirection
  {
    NE = 0, SE, SW, NW
  };

  public:
    void Generate(Position mapSize,
                  Position roomSizes,                  
                  const FeatureRoomsWeights& weightsMap,
                  int maxIterations);

  private:
    bool CreateEmptyRoom(Position start, bool centered);

    bool IsAreaWalls(const Position& start,
                     const Position& size,
                     RoomBuildDirection buildDir);

    bool IsAreaWallsCentered(const Position& start,
                             const Position& size);

    Position GetRandomRoomSize();
    Position GetOffsetsForDirection(RoomBuildDirection buildDir);
    RoomEdgeEnum GetCarveDirectionForDeadend(Position pos);

    void FillWithEmpty(const Position& start,
                       const Position& end,
                       const Position& dir);

    void CreateShrine(const Position& start,
                      RoomBuildDirection buildDir,
                      ShrineType type);

    void CreateDiamondRoom(const Position& start,
                           const Position& roomSize,
                           RoomBuildDirection buildDir);

    std::vector<Position> GetValidCellsToCarveFrom();

    Position GetCarveOffsetsForDir(RoomEdgeEnum carveDir);

    bool TryToCreateRoom(const Position& doorPos,
                         const Position& newRoomStartPos,
                         FeatureRoomType roomType);

    FeatureRoomsWeights _weightsMap;

    Position _roomSizes;

    std::map<FeatureRoomType, int> _generatedSoFar;
    std::map<FeatureRoomType, int> _roomWeightByType;
};

#endif // FEATUREROOMS_H
