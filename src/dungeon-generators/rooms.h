#ifndef ROOMS_H
#define ROOMS_H

#include "dg-base.h"
#include "constants.h"

class Rooms : public DGBase
{
  public:
    void Generate(const Position& mapSize,
                  const Position& roomSizes,
                  int maxIterations);
};

#endif // ROOMS_H
