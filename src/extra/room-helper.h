#ifndef ROOMHELPER_H
#define ROOMHELPER_H

#include <string>
#include <vector>
#include <map>

#include "constants.h"
#include "position.h"

using RoomLayout = std::vector<std::string>;

struct RoomForLevel
{
  RoomForLevel(int chance, const RoomLayout& l);

  int Chance = 0;
  RoomLayout Layout;
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

  bool CanAttach(const RoomHelper& r, RoomEdgeEnum side);

  bool CanBeTraversed(const std::vector<int>& e1,
                      const std::vector<int>& e2);

  bool EdgeIsAllWalls(const std::vector<int>& edge);

  void ParseLayout(const RoomLayout& layout);

  void PrintInfo();
};

#endif // ROOMHELPER_H
