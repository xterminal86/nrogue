#ifndef ROOMS_H
#define ROOMS_H

#include "dg-base.h"

class Rooms : public DGBase
{
  public:
    void Generate(Position mapSize, Position roomSize, int maxIterations);

  private:
    void CreateRoom(Position start, Position end);

    bool IsAreaWalls(Position start, Position end);
};

#endif // ROOMS_H
