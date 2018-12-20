#include "pathfinder.h"

std::vector<PathNode> Pathfinder::BuildRoad(const std::vector<std::vector<char>>& map,
                                            Position mapSize,
                                            Position start,
                                            Position end)
{
  _mapSize = mapSize;

  _start = start;
  _end = end;

  std::vector<PathNode> path;

  std::vector<PathNode> openList;
  std::vector<PathNode> closedList;

  PathNode node(start);
  node.CostH = Util::BlockDistance(start, end);
  node.CostF = node.CostG + node.CostH;

  openList.push_back(node);

  bool exit = false;
  while (!exit)
  {
    int index = FindCheapestElement(openList);
    auto closedNode = openList[index];

    openList.erase(openList.begin() + index);

    LookAround(closedNode, openList, closedList);

    exit = (openList.size() == 0 || IsNodePresent(end, closedList));
  }

  return path;
}

void Pathfinder::LookAround(PathNode& node,
                            std::vector<PathNode>& openList,
                            std::vector<PathNode>& closedList)
{
  std::vector<Position> direction =
  {
    { 0, -1 },
    { 1, 0 },
    { 0, 1 },
    { -1, 0 }
  };

  Position coordinate;
  for (int i = 0; i < 4; i++)
  {
    coordinate.X = node.Coordinate.X + direction[i].X;
    coordinate.Y = node.Coordinate.Y + direction[i].Y;

    coordinate.X = Util::Clamp(coordinate.X, 1, _mapSize.Y - 2);
    coordinate.Y = Util::Clamp(coordinate.Y, 1, _mapSize.X - 2);

    bool isInClosedList = IsNodePresent(coordinate, closedList);

    bool condition = !isInClosedList;

    // FIXME: what is this?
    //bool condition = (_map[coordinate.X][coordinate.Y] == null) && !isInClosedList;

    //Debug.Log("Current cell " + node.Coordinate + " Next cell " + coordinate);

    if (condition)
    {
      bool isInOpenList = IsNodePresent(coordinate, openList);

      if (!isInOpenList)
      {
        PathNode newNode({ coordinate.X, coordinate.Y }, &node);
        newNode.CostG = node.CostG + TraverseCost(node.Coordinate, newNode.Coordinate);
        newNode.CostH = Util::BlockDistance(newNode.Coordinate, _end);
        newNode.CostF = newNode.CostG + newNode.CostH;

        openList.push_back(newNode);
      }
    }
  }
}

int Pathfinder::FindCheapestElement(const std::vector<PathNode>& list)
{
  int f = INT_MAX;
  int index = -1;
  int count = 0;

  for (auto& item : list)
  {
    if (item.CostF < f)
    {
      f = item.CostF;
      index = count;
    }

    count++;
  }

  //Debug.Log("Cheapest element " + list[index].Coordinate + " " + list[index].CostF);

  return index;
}

int Pathfinder::TraverseCost(Position p1, Position p2)
{
  if (p1.X == p2.X || p1.Y == p2.Y)
  {
    return _hvCost;
  }

  return _diagonalCost;
}

bool Pathfinder::IsNodePresent(Position& p, const std::vector<PathNode>& list)
{
  for (auto& item : list)
  {
    if (p.X == item.Coordinate.X &&
        p.Y == item.Coordinate.Y)
    {
      return true;
    }
  }

  return false;
}
