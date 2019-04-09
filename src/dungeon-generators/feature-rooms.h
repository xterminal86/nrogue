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
                  int maxIterations,
                  const FeatureRoomsWeights& weightsMap);

  private:
    void CreateEmptyRoom(Position upperLeftCorner,
                         Position size,
                         RoomBuildDirection buildDir = RoomBuildDirection::SE);

    bool IsAreaWalls(Position corner, Position size, RoomBuildDirection buildDir);

    Position GetOffsetsForDirection(RoomBuildDirection buildDir);
    RoomEdgeEnum GetCarveDirectionForDeadend(Position pos);

    void FillWithEmpty(const Position& start,
                       const Position& end,
                       const Position& dir);

    void CreateShrine(const Position& start,
                      const Position& end,
                      const Position& dir,
                      ShrineType type);

    FeatureRoomsWeights _weightsMap;
};

#endif // FEATUREROOMS_H
