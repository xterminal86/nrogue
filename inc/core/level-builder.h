#ifndef LEVELBUILDER_H
#define LEVELBUILDER_H

#include <stack>

#include "util.h"
#include "room-helper.h"

struct MapCell
{
  Position Coordinates;
  bool Visited = false;
};

class LevelBuilder
{
  public:
    void BuildLevelFromLayouts(std::vector<RoomForLevel> possibleRooms, int startX, int startY, int mapSizeX, int mapSizeY);
    void PrintResult();

    std::vector<RoomHelper> MapChunks;
    std::vector<std::vector<char>> MapLayout;

  private:
    std::vector<std::vector<MapCell>> _visitedCells;

    std::stack<RoomHelper> _rooms;

    std::vector<RoomForLevel> _roomsForLevel;

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

    int _roomsCount = 0;

    Position _mapSize;
    Position _startingPoint;

    std::vector<RoomHelper> GetRoomsForLayout(RoomLayout& layout, RoomEdgeEnum side);

    void TryToAddRoomTo(RoomHelper& currentRoom, RoomEdgeEnum side);
    void PrintChunks();
    void VisitCells(RoomHelper& room);
    void PrintVisitedCells();
    void ConvertChunksToLayout();

    bool CheckLimits(Position& start, int roomSize);
    bool IsAreaVisited(Position& start, int roomSize);

    RoomLayout CreateSquareLayout(int size, chtype ch);

    RoomLayout SelectRoom();
    std::vector<RoomLayout> SelectRooms();
};

#endif // LEVELBUILDER_H