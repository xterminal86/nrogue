#ifndef CELLULARAUTOMATA_H
#define CELLULARAUTOMATA_H

#include <queue>
#include <map>

#include "dg-base.h"

class CellularAutomata : public DGBase
{
  public:
    void Generate(const Position& mapSize,
                  int initialWallChance,
                  int birthThreshold,
                  int deathThreshold,
                  int maxIterations);
};

#endif // CELLULARAUTOMATA_H
