#ifndef DGBASE_H
#define DGBASE_H

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <stack>
#include <variant>
#include <random>

#include "enumerations.h"
#include "constants.h"
#include "position.h"
#include "rect.h"

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
using FeatureRoomsWeights     = std::unordered_map<FeatureRoomType, std::pair<int, int>>;
using TransformedRoomsWeights = std::unordered_map<TransformedRoom, std::pair<int, int>>;

using Tile    = std::vector<std::string>;
using Tiles   = std::vector<Tile>;
using Tileset = std::vector<Tiles>;
using PairII  = std::pair<int, int>;

struct MapCell
{
  //
  // Used in connecting isolated regions during maze creation.
  //
  int AreaMarker = -1;

  //
  // For marking cell to be part of a region during rooms transformation.
  //
  TransformedRoom ZoneMarker = TransformedRoom::UNMARKED;

  //
  // Tile visual.
  //
  char Image = '#';

  //
  // Something to create on this tile.
  //
  std::variant<GameObjectType, ItemType, ShrineType> ObjectHere;

  //
  // Some pompous name for the zone, like "Murder Hollace" or something.
  // Unused for now.
  //
  std::string ZoneName;

  //
  // Doesn't seem to be used now, but may come in handy at some point
  // if for some reason we need to find out cell's map coordinates
  // when the only thing we have is cell object (pointer or reference).
  // So let's have it.
  //
  Position Coordinates;

  //
  // Used in generation algorithms.
  //
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
    DGBase();
    virtual ~DGBase() = default;

    void PrintMapRaw();
    void LogPrintMapRaw();

    double GetEmptyOverWallsRatio();
    double GetEmptyPercent();

    MapCell* GetCell(int x, int y);

    const std::vector<Rect>& GetEmptyRooms();

    void TransformRooms(const TransformedRoomsWeights& weights);

    CharV2 MapRaw;

    //
    // Since we cannot specify shrine type inside text map array
    // during generation, we save all shrines into this map and
    // use it at level instantiation stage.
    //
    // NOTE: cannot replace with unordered_map
    // because there is not hash function for Position.
    // Compiler gives weird error during compilation of player.cpp though.
    //
    std::map<Position, ShrineType> ShrinesByPosition;

    std::string GetMapRawString();

    const std::vector<std::vector<MapCell>>& GeneratedMap();

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
    bool FillMapChunk(int x, int y, int w, int h, char with, bool markVisited = false);
    bool VisitArea(int x, int y, int w, int h);
    bool AreChunksEqual(const StringV& chunk1, const StringV& chunk2);
    bool IsCorner(int x, int y, CornerType cornerType);
    bool IsAreaEmpty(int x1, int y1, int x2, int y2);
    Position* FindCorner(int x, int y, CornerType cornerToFind);

    std::vector<std::vector<MapCell>> CreateFilledMap(int w, int h, char image = '#');
    std::vector<std::vector<MapCell>> CreateRandomlyFilledMap(int w, int h, int chance);

    void PlaceDoors(bool useAdditionalLayout = false);
    bool IsSpotValidForDoor(const Position& p);
    std::vector<Position> FindPlaceForDoor();

    std::vector<std::vector<MapCell>> _map;

    std::vector<Rect> _emptyRooms;

    Position _mapSize;
    Position _startingPoint;

    RemovalParams _endWallsRemovalParams;

    StringV _mapChunk;

    bool _useAdditionalLayoutForDoors = false;

    //
    // Will use its own RNG as well to decouple dungeon generation from
    // any code changes.
    //
    std::mt19937_64 _rng;

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

    bool TransformArea(TransformedRoom type, size_t emptyRoomIndex);
    bool DoesAreaFit(const Rect& area, int minSize, int maxSize);
    bool DoesAreaFitExactly(const Rect& area, const PairII& size);

    void MarkZone(const Rect& area, TransformedRoom zoneType);

    void MarkAreaEmpty(const Rect& area);
    void MarkAreaDebug(const Rect& area, char c);

    void PlaceTreasury(const Rect& area);
    void PlaceShrine(const Rect& area);
    void PlaceStorage(const Rect& area);
    void PlaceChestroom(const Rect& area);

    bool TryToPlaceRoom(int minSize, int maxSize,
                        const Rect& area,
                        size_t emptyRoomIndex,
                        TransformedRoom roomType,
                        const std::function<void(const Rect&)>& fn);

    bool TryToPlaceRoom(const std::vector<PairII>& exactSizes,
                        const Rect& area,
                        size_t emptyRoomIndex,
                        TransformedRoom roomType,
                        const std::function<void(const Rect&)>& fn);

    void TryToPlaceLayout(const Rect& area, const StringV& layout,
                          int offsetX, int offsetY);

    void CheckBlockedPassages(const Rect& area, const StringV& layout,
                              int offsetX, int offsetY);

    Position _nonMarkedCell;
    Position _cornerPos;

    //
    // Trying to find room that fits into:
    //
    // width  >= minSize and <= maxSize,
    // height >= minSize and <= maxSize
    //
    std::vector<size_t> TryToFindSuitableRooms(int minSize, int maxSize,
                                               size_t skipRoomIndex);

    //
    // Trying to find room that satisfies:
    //
    // width  == exactSizes[0].first,
    // height == exactSizes[0].second
    //
    // among any of the exactSize vector elements.
    //
    std::vector<size_t> TryToFindSuitableRooms(const std::vector<PairII>& exactSizes,
                                               size_t skipRoomIndex);

    //
    // Must be sorted.
    //
    std::map<int, std::vector<Position>> _areaPointsByMarker;

    //
    // Rooms that cannot be placed for this map due to no suitable room found
    // and thus shouldn't be checked again if rolled.
    //
    std::unordered_map<TransformedRoom, bool> _failures;
};

#endif
