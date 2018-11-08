#ifndef ROOMHELPER_H
#define ROOMHELPER_H

#include <string>
#include <vector>
#include <queue>
#include <map>

#include "util.h"

enum class RoomEdgeEnum
{
  NORTH = 0,
  EAST,
  SOUTH,
  WEST
};

using RoomLayout = std::vector<std::string>;

struct RoomForLevel
{
  RoomForLevel(int chance, RoomLayout l)
  {
    Chance = chance;
    Room = l;
  }

  int Chance = 0;
  RoomLayout Room;
};

struct RoomHelper
{
  RoomLayout Layout;

  Position UpperLeftCorner;

  std::vector<std::vector<int>> OccupyMap;

  std::vector<int> NorthEdge;
  std::vector<int> EastEdge;
  std::vector<int> SouthEdge;
  std::vector<int> WestEdge;

  std::map<RoomEdgeEnum, std::vector<int>> RoomEdgesByType;

  int RoomSize;

  bool CanAttach(RoomHelper& r, RoomEdgeEnum side)
  {
    std::map<RoomEdgeEnum, RoomEdgeEnum> oppositeEdgeByType =
    {
      { RoomEdgeEnum::NORTH, RoomEdgeEnum::SOUTH },
      { RoomEdgeEnum::EAST, RoomEdgeEnum::WEST },
      { RoomEdgeEnum::SOUTH, RoomEdgeEnum::NORTH },
      { RoomEdgeEnum::WEST, RoomEdgeEnum::EAST }
    };

    //bool e1 = EdgeIsAllWalls(RoomEdgesByType[side]);
    //bool e2 = EdgeIsAllWalls(r.RoomEdgesByType[oppositeEdgeByType[side]]);

    bool res = CanBeTraversed(RoomEdgesByType[side], r.RoomEdgesByType[oppositeEdgeByType[side]]);

    //return ( (e1 == true && e2 == true) || res);
    return res;
  }

  bool CanBeTraversed(std::vector<int>& e1, std::vector<int>& e2)
  {
    int size = e1.size();

    for (int i = 0; i < size; i++)
    {
      if (e1[i] == 1 && e2[i] == 1)
      {
        return true;
      }
    }

    return false;
  }

  bool EdgeIsAllWalls(std::vector<int>& edge)
  {
    for (int i = 0; i < edge.size(); i++)
    {
      if (edge[i] == 1)
      {
        return false;
      }
    }

    return true;
  }

  void ParseLayout(RoomLayout layout)
  {
    // Assuming rooms are square

    RoomSize = layout.size();

    Layout = layout;

    for (int x = 0; x < Layout.size(); x++)
    {
      std::vector<int> row;
      for (int y = 0; y < Layout[x].length(); y++)
      {
        int res = (Layout[x][y] == '.') ? 1 : 0;
        row.push_back(res);
      }

      OccupyMap.push_back(row);
    }

    int len = OccupyMap.size();
    for (int y = 0; y < len; y++)
    {
      NorthEdge.push_back(OccupyMap[0][y]);
      SouthEdge.push_back(OccupyMap[len - 1][y]);

      EastEdge.push_back(OccupyMap[y][len - 1]);
      WestEdge.push_back(OccupyMap[y][0]);
    }

    RoomEdgesByType[RoomEdgeEnum::NORTH] = NorthEdge;
    RoomEdgesByType[RoomEdgeEnum::EAST] = EastEdge;
    RoomEdgesByType[RoomEdgeEnum::SOUTH] = SouthEdge;
    RoomEdgesByType[RoomEdgeEnum::WEST] = WestEdge;

    // PrintInfo();
  }

  void PrintInfo()
  {
    printf("Parsed layout:\n");

    printf("[\n");

    for (int x = 0; x < Layout.size(); x++)
    {
      for (int y = 0; y < Layout[x].length(); y++)
      {
        printf("%c", Layout[x][y]);
      }

      printf("\n");
    }
    printf("]\n");

    printf("\n");

    printf("Edges:\n");

    for (auto& kvp : RoomEdgesByType)
    {
      printf("%i: ", kvp.first);

      for (int i = 0; i < kvp.second.size(); i++)
      {
        printf("%i", RoomEdgesByType[kvp.first][i]);
      }

      printf("\n");
    }

    printf("\n");
  }
};

#endif // ROOMHELPER_H
