#ifndef CELLULARAUTOMATA_H
#define CELLULARAUTOMATA_H

#include <queue>
#include <map>

#include "dg-base.h"

class CellularAutomata : public DGBase
{
  public:
    void Generate(Position mapSize,
                  int initialWallChance,
                  int birthThreshold,
                  int deathThreshold,
                  int maxIterations);

  private:
    int _markerValue = 0;

    std::map<int, std::vector<Position>> _areasByMarker;

    void ConnectIsolatedAreas();
    void TryToMarkCell(Position p, std::queue<Position>& visitedCells);
    void FloodFill(Position start);
    void ConnectPoints(Position p1, Position p2);

    std::vector<Position> FindNonMarkedCell();

    std::pair<Position, Position> FindClosestPointsToArea(int areaMarker);
};

#endif // CELLULARAUTOMATA_H
