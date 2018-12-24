#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "util.h"

struct PathNode
{
  PathNode() {}

  PathNode(Position coord)
  {
    Coordinate.X = coord.X;
    Coordinate.Y = coord.Y;
  }

  PathNode(const PathNode& rhs)
  {
    Coordinate = rhs.Coordinate;
    ParentNodePosition = rhs.ParentNodePosition;
    CostF = rhs.CostF;
    CostG = rhs.CostG;
    CostH = rhs.CostH;
  }

  PathNode(Position coord, Position parentNodePos)
  {
    Coordinate.X = coord.X;
    Coordinate.Y = coord.Y;

    ParentNodePosition = parentNodePos;
  }

  // Map coordinate of this node
  Position Coordinate;

  // Can't use PathNode* here, beacuse it's not C# and .NET
  // and we can't just assign "reference to previous node"
  // and assume it will stay there no matter the containers.
  Position ParentNodePosition = { -1, -1 };

  // Total cost
  int CostF = 0;

  // Cost of traversal here from the starting point
  // with regard to already traversed path
  int CostG = 0;

  // Heuristic cost
  int CostH = 0;
};

class Pathfinder
{
  public:
    std::vector<Position> BuildRoad(const std::vector<std::vector<char>>& map,
                                    Position mapSize,
                                    Position start,
                                    Position end,
                                    std::vector<char> obstacles,
                                    bool eightDirs = false);

  private:
    Position _mapSize;

    Position _start;
    Position _end;

    int _hvCost = 10;
    int _diagonalCost = 20;

    int FindCheapestElement(const std::vector<PathNode>& list);
    int TraverseCost(Position p1, Position p2);

    void LookAround(const std::vector<std::vector<char>>& map,
                    PathNode& node,
                    std::vector<PathNode>& openList,
                    std::vector<PathNode>& closedList,
                    std::vector<char>& obstacles,
                    bool eightDirs);

    bool IsNodePresent(PathNode& n, const std::vector<PathNode>& list);
    bool IsInsideMap(Position c);

    PathNode FindNodeWithPosition(std::vector<PathNode>& list, Position p);
};

#endif // PATHFINDER_H
