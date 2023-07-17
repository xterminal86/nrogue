#ifndef DG_BASE_H
#define DG_BASE_H

#include <vector>
#include <map>
#include <string>
#include <stack>

#include "constants.h"
#include "position.h"
#include "rect.h"

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

enum class CornerType
{
  UL = 0,
  UR,
  DL,
  DR
};

//
// {
//   <room type>,
//   {
//     <probability weight>,
//     <maximum number of rooms allowed to be generated>
//   }
// }
//
// 0 if there is no limit to the amount of rooms
//
using FeatureRoomsWeights = std::map<FeatureRoomType, std::pair<int, int>>;

using Tile = std::vector<std::string>;
using Tiles = std::vector<Tile>;
using Tileset = std::vector<Tiles>;

struct MapCell
{
  //
  // Used in connecting isolated regions during maze creation.
  //
  int AreaMarker = -1;

  //
  // For storing additional meta information for a cell.
  //
  int ZoneMarker = -1;

  char Image = '#';
  Position Coordinates;
  bool Visited = false;
};

struct RemovalParams
{
  int EmptyCellsAroundMin = 3;
  int EmptyCellsAroundMax = 8;
  int Passes = 0;
};

class DGBase
{
  public:
    virtual ~DGBase() = default;

    void PrintMapRaw();
    void LogPrintMapRaw();

    double GetEmptyOverWallsRatio();
    double GetEmptyPercent();

    MapCell* GetCell(int x, int y);

    const std::vector<Rect>& GetEmptyRooms();

    CharV2 MapRaw;

    //
    // Since we cannot specify shrine type inside text map array
    // during generation, we save all shrines into this map and
    // use it at level instantiation stage.
    //
    std::map<Position, ShrineType> ShrinesByPosition;

    std::string GetMapRawString();

    virtual void ForCustomDebugStuff();

  protected:
    bool CheckLimits(const Position& start, int roomSize);
    bool IsInsideMap(const Position& pos);
    bool IsInBounds(int x, int y);
    bool IsDeadEnd(const Position& p);

    void FillDeadEnds();
    void FillMapRaw();
    void CutProblemCorners();
    void RemoveEndWalls();
    void RemoveSingleWalls();
    void FillSingleCells();
    void CheckIfProblemCorner(const Position& p);
    void ConnectIsolatedAreas();
    void AddRow(const MapCell& cell);
    void AddColumn(const MapCell& cell);
    void CreateMapBorders();

    int CountAround(int x, int y, char ch);

    const StringV& ExtractMapChunk(int x, int y, int w, int h);
    bool FillMapChunk(int x, int y, int w, int h, char with);
    bool AreChunksEqual(const StringV& chunk1, const StringV& chunk2);
    bool IsCorner(int x, int y, CornerType cornerType);
    bool IsAreaEmpty(int x1, int y1, int x2, int y2);
    Position* FindCorner(int x, int y, CornerType cornerToFind);

    std::vector<std::vector<MapCell>> CreateFilledMap(int w, int h, char image = '#');
    std::vector<std::vector<MapCell>> CreateRandomlyFilledMap(int w, int h, int chance);

    void PlaceDoors();
    bool IsSpotValidForDoor(const Position& p);
    std::vector<Position> FindPlaceForDoor();

    std::vector<std::vector<MapCell>> _map;

    std::vector<Rect> _emptyRooms;

    Position _mapSize;
    Position _startingPoint;

    RemovalParams _endWallsRemovalParams;

    StringV _mapChunk;

    //
    // Algorithm specific data and methods, no need to expose these
    //
  private:
    void AddCellToProcess(const Position& from,
                          Direction dir,
                          std::stack<Position>& addTo);

    int MarkRegions();

    void UnmarkRegions();

    Position* FindNonMarkedCell();

    void ConnectPoints(const Position& p1, const Position& p2);

    Position _nonMarkedCell;
    Position _cornerPos;

    std::map<int, std::vector<Position>> _areaPointsByMarker;
};

#endif
