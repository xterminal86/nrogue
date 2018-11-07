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

  Position UpperLeftCorner;

  std::vector<std::vector<int>> OccupyMap;

  std::vector<int> NorthEdge;
  std::vector<int> EastEdge;
  std::vector<int> SouthEdge;
  std::vector<int> WestEdge;

  std::map<RoomEdgeEnum, std::vector<int>> RoomEdgesByType;

  std::map<RoomEdgeEnum, bool> EdgesStatus()
  {
    std::map<RoomEdgeEnum, bool> res;

    res[RoomEdgeEnum::NORTH] = NorthEdgeLocked;
    res[RoomEdgeEnum::EAST] = EastEdgeLocked;
    res[RoomEdgeEnum::SOUTH] = SouthEdgeLocked;
    res[RoomEdgeEnum::WEST] = WestEdgeLocked;

    return res;
  }

  void SetEdgeLockStatus(RoomEdgeEnum type, bool status)
  {
    if (type == RoomEdgeEnum::NORTH)
    {
      NorthEdgeLocked = status;
    }
    else if (type == RoomEdgeEnum::EAST)
    {
      EastEdgeLocked = status;
    }
    else if (type == RoomEdgeEnum::SOUTH)
    {
      SouthEdgeLocked = status;
    }
    else if (type == RoomEdgeEnum::WEST)
    {
      WestEdgeLocked = status;
    }
  }

  bool NorthEdgeLocked = false;
  bool EastEdgeLocked = false;
  bool SouthEdgeLocked = false;
  bool WestEdgeLocked = false;

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

    bool res = false;

    for (int i = 0; i < r.Layout.size(); i++)
    {
      if (RoomEdgesByType[side][i] == 1
     && r.RoomEdgesByType[oppositeEdgeByType[side]][i] == 1)
      {
        res = true;
        break;
      }
    }

    return res;
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

class LevelBuilder
{
  public:
    LevelBuilder();
    void BuildLevel(int mapSizeX, int mapSizeY);
    void PrintResult();

    int MaxRooms = 4;

    std::map<RoomEdgeEnum, std::vector<RoomHelper>> GetRoomsForLayout(RoomLayout layout, RoomEdgeEnum side, std::vector<RoomLayout> roomsList);

    std::vector<RoomHelper> MapChunks;

  private:
    std::queue<RoomHelper> _rooms;

    std::vector<RoomEdgeEnum> _edgeTypes =
    {
      RoomEdgeEnum::NORTH,
      RoomEdgeEnum::EAST,
      RoomEdgeEnum::SOUTH,
      RoomEdgeEnum::WEST
    };

    std::map<RoomEdgeEnum, RoomEdgeEnum> _oppositeEdgeByType =
    {
      { RoomEdgeEnum::NORTH, RoomEdgeEnum::SOUTH },
      { RoomEdgeEnum::EAST, RoomEdgeEnum::WEST },
      { RoomEdgeEnum::SOUTH, RoomEdgeEnum::NORTH },
      { RoomEdgeEnum::WEST, RoomEdgeEnum::EAST }
    };

    std::vector<RoomLayoutRotation> _allDegrees =
    {
      RoomLayoutRotation::NONE,
      RoomLayoutRotation::CCW_90,
      RoomLayoutRotation::CCW_180,
      RoomLayoutRotation::CCW_270
    };

    std::map<RoomEdgeEnum, std::string> _edgeNameByType =
    {
      { RoomEdgeEnum::NORTH, "North Edge" },
      { RoomEdgeEnum::EAST,  "East Edge " },
      { RoomEdgeEnum::SOUTH, "South Edge" },
      { RoomEdgeEnum::WEST,  "West Edge " }
    };

    Position _mapSize;

    int _roomsCount = 0;

    void TryToAddRoomTo(RoomHelper& currentRoom);
    void PrintChunks();
    bool CheckLimits(RoomHelper& room);
};

#endif // LEVELBUILDER_H
