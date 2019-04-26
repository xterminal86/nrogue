#include "room-helper.h"

RoomForLevel::RoomForLevel(int chance, const RoomLayout& l)
{
  Chance = chance;
  Layout = l;
}

// *********************************

bool RoomHelper::CanAttach(const RoomHelper &r, RoomEdgeEnum side)
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

  bool res = CanBeTraversed(RoomEdgesByType.at(side), r.RoomEdgesByType.at(oppositeEdgeByType[side]));

  //return ( (e1 == true && e2 == true) || res);
  return res;
}

bool RoomHelper::CanBeTraversed(const std::vector<int> &e1,
                                const std::vector<int> &e2)
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

bool RoomHelper::EdgeIsAllWalls(const std::vector<int> &edge)
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

void RoomHelper::ParseLayout(const RoomLayout &layout)
{
  // Assuming rooms are square

  RoomSize = layout.size();

  Layout = layout;

  for (int x = 0; x < Layout.size(); x++)
  {
    std::vector<int> row;
    for (int y = 0; y < Layout[x].length(); y++)
    {
      int res = (Layout[x][y] == '.' || Layout[x][y] == '+') ? 1 : 0;
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

void RoomHelper::PrintInfo()
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
