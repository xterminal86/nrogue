#ifndef LEVELBUILDER_H
#define LEVELBUILDER_H

#include <stack>
#include <queue>

#include "util.h"
#include "room-helper.h"

struct MapCell
{
  int Marker = -1;
  char Image = '#';
  Position Coordinates;
  bool Visited = false;
};

enum class RoomBuildDirection
{
  NE = 0, SE, SW, NW
};

class LevelBuilder
{
  public:
    void BuildLevelFromLayouts(std::vector<RoomForLevel>& possibleRooms, int startX, int startY, int mapSizeX, int mapSizeY);
    void RecursiveBacktracker(Position mapSize, Position startingPoint = { -1, -1 });
    void Tunneler(Position mapSize, int maxTunnels, Position tunnelLengthMinMax, Position start = { -1, -1 } );
    void BacktrackingTunneler(Position mapSize, Position tunnelLengthMinMax, bool additionalTweaks, Position start = { - 1, -1 } );
    void CellularAutomata(Position mapSize, int initialWallChance, int birthThreshold, int deathThreshold, int maxIterations);
    void FeatureRooms(Position mapSize, Position roomSizes, int maxIterations);
    void PrintMapChunks();
    void PrintMapRaw();
    void LogPrintMapRaw();
    void Reset();

    std::vector<RoomHelper> MapChunks;
    std::vector<std::vector<char>> MapLayout;
    std::vector<std::vector<char>> MapRaw;

  private:
    std::vector<std::vector<MapCell>> _map;
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

    std::map<int, std::vector<Position>> _areasByMarker;

    int _roomsCount = 0;
    int _markerValue = 0;

    Position _mapSize;
    Position _startingPoint;

    std::string GetMapRawString();

    std::vector<RoomHelper> GetRoomsForLayout(RoomLayout& layout, RoomEdgeEnum side);
    std::vector<std::vector<MapCell>> CreateEmptyMap(int w, int h);
    std::vector<std::vector<MapCell>> CreateFilledMap(int w, int h);
    std::vector<std::vector<MapCell>> CreateRandomlyFilledMap(int w, int h, int chance);
    std::vector<Position> GetRandomCell(Position p);
    std::vector<Position> GetRandomDir(Position pos);

    void TryToAddRoomTo(RoomHelper& currentRoom, RoomEdgeEnum side);
    void PrintChunks();
    void VisitCells(RoomHelper& room);
    void PrintVisitedCells();
    void ConvertChunksToLayout();
    void CutProblemCorners();
    void FillDeadEnds();
    void FillMapRaw();
    void ConnectIsolatedAreas();
    void FloodFill(Position start);
    void TryToMarkCell(Position p, std::queue<Position>& visitedCells);
    void ConnectPoints(Position p1, Position p2);
    void CreateRoom(Position upperLeftCorner, Position size, RoomBuildDirection buildDir = RoomBuildDirection::SE);

    bool CheckLimits(Position& start, int roomSize);
    bool IsInsideMap(Position pos);
    bool IsAreaVisited(Position& start, int roomSize);
    bool IsDeadEnd(Position p);
    bool IsAreaWalls(Position corner, Position size, RoomBuildDirection dir);

    int CountAround(int x, int y, char ch);

    void CheckIfProblemCorner(Position p);

    Position GetRandomPerpendicularDir(Position dir);    
    std::vector<Position> TryToGetPerpendicularDir(Position pos, Position lastDir);

    RoomLayout CreateSquareLayout(int size, chtype ch);

    RoomLayout SelectRoom();
    std::vector<RoomLayout> SelectRooms();

    std::vector<Position> FindNonMarkedCell();
    std::pair<Position, Position> FindClosestPointsToArea(int areaMarker);

    Position GetOffsetsForDirection(RoomBuildDirection buildDir);
    RoomEdgeEnum GetCarveDirectionForDeadend(Position pos);
};

#endif // LEVELBUILDER_H
