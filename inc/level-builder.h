#ifndef LEVELBUILDER_H
#define LEVELBUILDER_H

#include <string>
#include <vector>
#include <queue>
#include <map>

#include "util.h"

using AsciiMap = std::vector<std::vector<std::string>>;
using RoomLayout = std::vector<std::string>;

enum class RoomEdgeEnum
{
  NORTH = 0,
  EAST,
  SOUTH,
  WEST
};

struct RoomHelper
{
  RoomLayout Layout;
  std::vector<std::vector<bool>> OccupyMap;

  std::vector<bool> NorthEdge;
  std::vector<bool> EastEdge;
  std::vector<bool> SouthEdge;
  std::vector<bool> WestEdge;

  bool NorthEdgeLocked;
  bool EastEdgeLocked;
  bool SouthEdgeLocked;
  bool WestEdgeLocked;

  std::map<RoomEdgeEnum, bool> Edges()
  {
    std::map<RoomEdgeEnum, bool> res;

    res[RoomEdgeEnum::NORTH] = NorthEdgeLocked;
    res[RoomEdgeEnum::EAST] = EastEdgeLocked;
    res[RoomEdgeEnum::SOUTH] = SouthEdgeLocked;
    res[RoomEdgeEnum::WEST] = WestEdgeLocked;

    return res;
  }

  void ParseLayout(RoomLayout layout)
  {
    Layout = layout;

    for (int x = 0; x < Layout.size(); x++)
    {
      std::vector<bool> row;
      for (int y = 0; y < Layout[x].length(); y++)
      {
        bool res = (Layout[x][y] == '.') ? true : false;
        row.push_back(res);
      }

      OccupyMap.push_back(row);
    }

    // Assuming rooms are square

    int len = OccupyMap[0].size();
    for (int y = 0; y < len; y++)
    {
      NorthEdge.push_back(OccupyMap[0][y]);
      SouthEdge.push_back(OccupyMap[len - 1][y]);

      EastEdge.push_back(OccupyMap[y][0]);
      WestEdge.push_back(OccupyMap[y][len - 1]);
    }
  }
};

using MapChunks = std::map<Position, RoomHelper>;

class LevelBuilder
{
  public:
    void BuildLevel(int sizeX, int sizeY);

    std::string Result;

  private:
    AsciiMap _map;
    MapChunks _mapChunks;

    Position _cursor;

    std::queue<RoomLayout> _rooms;

    void TryToAddRoom(RoomHelper& currentRoom);
};

#endif // LEVELBUILDER_H
