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

  while (!openList.empty())
  {
    int index = FindCheapestElement(openList);
    if (index == -1)
    {
      break;
    }

    PathNode currentNode = openList[index];

    auto dbg = Util::StringFormat("Open node %i %i", currentNode.Coordinate.X, currentNode.Coordinate.Y);
    Logger::Instance().Print(dbg);

    openList.erase(openList.begin() + index);

    if (currentNode.Coordinate == end)
    {
      // FIXME:
      break;

      path.push_back(currentNode);

      PathNode* parent = currentNode.ParentNode;
      while (parent)
      {
        path.push_back(*parent);
        parent = parent->ParentNode;
      }

      break;
    }

    LookAround(map, currentNode, openList, closedList);

    closedList.push_back(currentNode);
  }

  return path;
}

void Pathfinder::LookAround(const std::vector<std::vector<char>>& map,
                            PathNode& node,
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

  std::vector<PathNode> nodesAround;

  for (int i = 0; i < 4; i++)
  {
    Position coordinate;

    coordinate.X = node.Coordinate.X + direction[i].X;
    coordinate.Y = node.Coordinate.Y + direction[i].Y;

    if (!IsInsideMap(coordinate)
      || map[coordinate.X][coordinate.Y] == '#')
    {
      continue;
    }

    PathNode newNode(coordinate);
    nodesAround.push_back(newNode);
  }

  for (auto& c : nodesAround)
  {
    if (c.Coordinate == _end)
    {
      return;
    }

    c.CostG = node.CostG + TraverseCost(node.Coordinate, c.Coordinate);
    c.CostH = Util::BlockDistance(c.Coordinate, _end);
    c.CostF = c.CostH + c.CostG;
    c.ParentNode = &node;

    bool isInOpenList = false;
    for (auto& n : openList)
    {
      if (n.Coordinate == c.Coordinate
       && n.CostF < c.CostF)
      {
        isInOpenList = true;
        break;
      }
    }

    if (isInOpenList)
    {
      continue;
    }

    bool isInClosedList = false;
    for (auto& n : closedList)
    {
      if (n.Coordinate == c.Coordinate
       && n.CostF < c.CostF)
      {
        isInClosedList = true;
        break;
      }
    }

    if (isInClosedList)
    {
      continue;
    }

    openList.push_back(c);

    auto dbg = Util::StringFormat("\tAdding to open list: %i %i, costF %i, parent %i %i", c.Coordinate.X, c.Coordinate.Y, c.CostF, node.Coordinate.X, node.Coordinate.Y);
    Logger::Instance().Print(dbg);
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

  auto dbg = Util::StringFormat("\tCheapest element %i %i - %i", list[index].Coordinate.X, list[index].Coordinate.Y, list[index].CostF);
  Logger::Instance().Print(dbg);

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

bool Pathfinder::IsNodePresent(PathNode& n, const std::vector<PathNode>& list)
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

bool Pathfinder::IsInsideMap(Position c)
{
  bool cond = (c.X >= 1
            && c.Y >= 1
            && c.X < _mapSize.X - 1
            && c.Y < _mapSize.Y - 1);

  return cond;
}
