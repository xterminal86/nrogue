#ifndef DG_BASE_H
#define DG_BASE_H

#include <vector>
#include <map>
#include <string>

#include "constants.h"
#include "position.h"

enum class FeatureRoomType
{
  EMPTY = 0,
  SHRINE,
  GARDEN,
  FLOODED,
  DIAMOND,
  ROUND,
  PILLARS,
  POND,
  FOUNTAIN
};

// {
//   <room type>,
//   {
//     <probability weight>,
//     <maximum number of rooms allowed to be generated>
//   }
// }
//
// -1 if there is no limit to amount of rooms
using FeatureRoomsWeights = std::map<FeatureRoomType, std::pair<int, int>>;

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
    virtual ~DGBase() = default;

    void PrintMapRaw();
    void LogPrintMapRaw();

    std::vector<std::vector<char>> MapRaw;

    // Since we cannot specify shrine type inside text map array
    // during generation, we save all shrines into this map and
    // use it at level instantiation stage.
    std::map<Position, ShrineType> ShrinesByPosition;

  protected:
    bool CheckLimits(const Position& start, int roomSize);
    bool IsInsideMap(const Position& pos);
    bool IsDeadEnd(const Position& p);

    void FillDeadEnds();
    void FillMapRaw();
    void CutProblemCorners();
    void CheckIfProblemCorner(const Position& p);

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
