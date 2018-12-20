#ifndef FEATUREROOMS_H
#define FEATUREROOMS_H

#include "dg-base.h"

class FeatureRooms : public DGBase
{
  enum class RoomBuildDirection
  {
    NE = 0, SE, SW, NW
  };

  public:
    void Generate(Position mapSize, Position roomSizes, int maxIterations);

  private:
    void CreateRoom(Position upperLeftCorner, Position size, RoomBuildDirection buildDir = RoomBuildDirection::SE);

    bool IsAreaWalls(Position corner, Position size, RoomBuildDirection buildDir);

    Position GetOffsetsForDirection(RoomBuildDirection buildDir);
    RoomEdgeEnum GetCarveDirectionForDeadend(Position pos);
};

#endif // FEATUREROOMS_H
