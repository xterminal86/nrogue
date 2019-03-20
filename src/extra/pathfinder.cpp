#include "pathfinder.h"

std::vector<Position> Pathfinder::BuildRoad(const std::vector<std::vector<char>>& map,
                                            const Position& mapSize,
                                            const Position& start,
                                            const Position& end,
                                            const std::vector<char>& obstacles,
                                            bool eightDirs)
{
  _mapSize = mapSize;

  _start = start;
  _end = end;

  std::vector<Position> path;

  std::vector<PathNode> openList;
  std::vector<PathNode> closedList;

  PathNode node(start);
  node.CostH = Util::BlockDistance(start, end);
  node.CostF = node.CostG + node.CostH;

  openList.push_back(node);

  while (!openList.empty())
  {
    int index = FindCheapestElement(openList);

    PathNode currentNode = openList[index];

    closedList.push_back(openList[index]);

    if (openList[index].Coordinate == end)
    {
      PathNode n = FindNodeWithPosition(closedList, end);
      while (n.ParentNodePosition.X != -1 && n.ParentNodePosition.Y != -1)
      {
        path.push_back(n.Coordinate);
        n = FindNodeWithPosition(closedList, n.ParentNodePosition);
      }

      path.push_back(start);

      std::reverse(path.begin(), path.end());

      break;
    }

    LookAround(map, openList[index], openList, closedList, obstacles, eightDirs);

    openList.erase(openList.begin() + index);
  }

  return path;
}

void Pathfinder::LookAround(const std::vector<std::vector<char>>& map,
                            const PathNode& currentNode,
                            std::vector<PathNode>& openList,
                            std::vector<PathNode>& closedList,
                            const std::vector<char>& obstacles,
                            bool eightDirs)
{
  std::vector<Position> directions;

  if (eightDirs)
  {
    directions =
    {
      { -1, -1 },
      { -1, 0 },
      { -1, 1 },
      { 0, -1 },
      { 0, 1 },
      { 1, -1 },
      { 1, 0 },
      { 1, 1 }
    };
  }
  else
  {
    directions =
    {
      { -1, 0 },
      { 0, -1 },
      { 0, 1 },
      { 1, 0 }
    };
  }

  std::vector<PathNode> nodesAround;

  for (auto& p : directions)
  {
    Position coordinate;

    coordinate.X = currentNode.Coordinate.X + p.X;
    coordinate.Y = currentNode.Coordinate.Y + p.Y;

    bool walkable = true;
    for (auto& o : obstacles)
    {
      if (map[coordinate.X][coordinate.Y] == o)
      {
        walkable = false;
        break;
      }
    }

    if (!IsInsideMap(coordinate) || !walkable)
    {
      continue;
    }

    PathNode newNode(coordinate, currentNode.Coordinate);
    nodesAround.push_back(newNode);
  }

  for (auto& nodeAround : nodesAround)
  {    
    if (IsNodePresent(nodeAround, closedList))
    {
      continue;
    }

    if (IsNodePresent(nodeAround, openList))
    {
      int newG = nodeAround.CostG + TraverseCost(nodeAround.Coordinate, currentNode.Coordinate);
      if (newG < nodeAround.CostG)
      {
        nodeAround.CostG = newG;
        nodeAround.CostH = Util::BlockDistance(nodeAround.Coordinate, _end);
        nodeAround.CostF = nodeAround.CostG + nodeAround.CostH;
        nodeAround.ParentNodePosition = currentNode.Coordinate;
      }
    }
    else
    {
      nodeAround.CostG = currentNode.CostG + TraverseCost(currentNode.Coordinate, nodeAround.Coordinate);
      nodeAround.CostH = Util::BlockDistance(nodeAround.Coordinate, _end);
      nodeAround.CostF = nodeAround.CostG + nodeAround.CostH;

      openList.push_back(nodeAround);
    }
  }
}

int Pathfinder::FindCheapestElement(const std::vector<PathNode>& list)
{
  int f = INT_MAX;
  int index = -1;

  for (int i = 0; i < list.size(); i++)
  {
    if (list[i].CostF < f)
    {
      f = list[i].CostF;
      index = i;
    }    
  }

  return index;
}

int Pathfinder::TraverseCost(const Position& p1, const Position& p2)
{
  if (p1.X == p2.X || p1.Y == p2.Y)
  {
    return _hvCost;
  }

  return _diagonalCost;
}

bool Pathfinder::IsNodePresent(const PathNode& n, const std::vector<PathNode>& list)
{
  for (auto& item : list)
  {
    if (n.Coordinate.X == item.Coordinate.X
     && n.Coordinate.Y == item.Coordinate.Y)
    {
      return true;
    }
  }

  return false;
}

bool Pathfinder::IsInsideMap(const Position& c)
{
  bool cond = (c.X >= 1
            && c.Y >= 1
            && c.X < _mapSize.X - 1
            && c.Y < _mapSize.Y - 1);

  return cond;
}

PathNode Pathfinder::FindNodeWithPosition(const std::vector<PathNode>& list, const Position& p)
{
  PathNode res;

  for (auto& i : list)
  {
    if (i.Coordinate.X == p.X && i.Coordinate.Y == p.Y)
    {
      res = i;
      break;
    }
  }

  return res;
}
