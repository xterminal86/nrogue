#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <stack>

#include "util.h"

class MapLevelBase;

struct PathNode
{
  PathNode() = default;
  PathNode(const Position& coord);
  PathNode(const Position& coord, const Position& parentNodePos);

  //
  // Map coordinate of this node
  //
  Position Coordinate;

  //
  // Can't use PathNode* here, beacuse it's not C# and .NET
  // and we can't just assign "reference to previous node"
  // and assume it will stay there no matter the containers.
  //
  Position ParentNodePosition = { -1, -1 };

  //
  // Total cost
  //
  int CostF = 0;

  //
  // Cost of traversal here from the starting point
  // with regard to already traversed path
  //
  int CostG = 0;

  //
  // Heuristic cost
  //
  int CostH = 0;
};

// ***********************************************

class Pathfinder
{
  public:
    std::vector<Position> BuildRoad(const std::vector<std::vector<char>>& map,
                                    const Position& mapSize,
                                    const Position& start,
                                    const Position& end,
                                    const std::vector<char>& mapTilesToIgnore,
                                    bool eightDirs = false);

    std::stack<Position> BuildRoad(MapLevelBase* mapRef,
                                    const Position& start,
                                    const Position& end,
                                    const std::vector<char>& mapTilesToIgnore,
                                    bool ignoreActors = true,
                                    bool eightDirs = false,
                                    size_t maxPathLength = 0);

  private:
    Position _mapSize;

    Position _start;
    Position _end;

    int _hvCost = 10;
    int _diagonalCost = 20;

    int FindCheapestElement(const std::vector<PathNode>& list);
    int TraverseCost(const Position& p1, const Position& p2);

    void LookAround(const std::vector<std::vector<char>>& map,
                    const PathNode& node,
                    std::vector<PathNode>& openList,
                    std::vector<PathNode>& closedList,
                    const std::vector<char>& mapTilesToIgnore,
                    bool eightDirs);

    void LookAround(MapLevelBase* mapRef,
                    const PathNode& node,
                    std::vector<PathNode>& openList,
                    std::vector<PathNode>& closedList,
                    const std::vector<char>& mapTilesToIgnore,
                    bool ignoreActors,
                    bool eightDirs);

    bool IsNodePresent(const PathNode& n, const std::vector<PathNode>& list);
    bool IsInsideMap(const Position& c);

    PathNode FindNodeWithPosition(const std::vector<PathNode>& list, const Position& p);

    std::vector<PathNode> _openList;
    std::vector<PathNode> _closedList;

    std::vector<Position> _pathVector;
    std::stack<Position>  _pathStack;

    const std::vector<Position> _eightDirs =
    {
      { -1, -1 },
      { -1,  0 },
      { -1,  1 },
      {  0, -1 },
      {  0,  1 },
      {  1, -1 },
      {  1,  0 },
      {  1,  1 }
    };

    const std::vector<Position> _fourDirs =
    {
      { -1,  0 },
      {  0, -1 },
      {  0,  1 },
      {  1,  0 }
    };
};

#endif // PATHFINDER_H
