#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "util.h"

struct PathNode
{
  PathNode(Position coord)
  {
    Coordinate.X = coord.X;
    Coordinate.Y = coord.Y;
  }

  PathNode(const PathNode& rhs)
  {
    Coordinate = rhs.Coordinate;
    ParentNode = rhs.ParentNode;
    CostF = rhs.CostF;
    CostG = rhs.CostG;
    CostH = rhs.CostH;
  }

  PathNode(Position coord, PathNode* parent)
  {
    Coordinate.X = coord.X;
    Coordinate.Y = coord.Y;

    ParentNode = parent;
  }

  // Map coordinate of this node
  Position Coordinate;

  // Reference to parent node
  PathNode* ParentNode = nullptr;

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
    std::vector<PathNode> BuildRoad(const std::vector<std::vector<char>>& map,
                                    Position mapSize,
                                    Position start,
                                    Position end);

  private:
    Position _mapSize;

    Position _start;
    Position _end;

    int _hvCost = 10;
    int _diagonalCost = 20;

    int FindCheapestElement(const std::vector<PathNode>& list);
    int TraverseCost(Position p1, Position p2);

    void LookAround(PathNode& node,
                    std::vector<PathNode>& openList,
                    std::vector<PathNode>& closedList);

    bool IsNodePresent(Position& p, const std::vector<PathNode>& list);
};

#endif // PATHFINDER_H
