#ifndef DG_BASE_H
#define DG_BASE_H

#include "util.h"

struct MapCell
{
  int Marker = -1;
  char Image = '#';
  Position Coordinates;
  bool Visited = false;
};

class DGBase
{
  public:
    virtual ~DGBase() {}

    void PrintMapRaw();
    void LogPrintMapRaw();

    std::vector<std::vector<char>> MapRaw;

  protected:
    bool CheckLimits(Position& start, int roomSize);
    bool IsInsideMap(Position pos);
    bool IsDeadEnd(Position p);

    void FillDeadEnds();
    void FillMapRaw();
    void CutProblemCorners();
    void CheckIfProblemCorner(Position p);

    int CountAround(int x, int y, char ch);

    std::string GetMapRawString();

    std::vector<std::vector<MapCell>> CreateEmptyMap(int w, int h);
    std::vector<std::vector<MapCell>> CreateFilledMap(int w, int h);
    std::vector<std::vector<MapCell>> CreateRandomlyFilledMap(int w, int h, int chance);

    std::vector<std::vector<MapCell>> _map;

    Position _mapSize;
    Position _startingPoint;
};

#endif
