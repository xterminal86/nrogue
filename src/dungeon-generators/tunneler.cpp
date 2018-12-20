#include "tunneler.h"

/// Builds tunnels perpendicular to previous direction,
/// backtracks to previous position if failed.
///
/// Additional tweaks include removal of deadends and
/// ....
/// ..#.
/// .#..
/// ....
/// situations
void Tunneler::Backtracking(Position mapSize, Position tunnelLengthMinMax, Position start, bool additionalTweaks)
{
  _mapSize = mapSize;

  _map = CreateFilledMap(mapSize.X, mapSize.Y);

  int sx, sy = 0;

  if (start.X == -1 || start.Y == -1)
  {
    sx = RNG::Instance().RandomRange(1, mapSize.X - 1);
    sy = RNG::Instance().RandomRange(1, mapSize.Y - 1);
  }
  else
  {
    sx = start.X;
    sy = start.Y;
  }

  _startingPoint.Set(sx, sy);

  _map[sx][sy].Image = '.';
  _map[sx][sy].Visited = true;

  // Contains corridor start position and direction
  std::stack<std::pair<Position, Position>> nodePoints;

  Position rndDir = GetRandomDir(_startingPoint)[0];

  nodePoints.push({ _startingPoint, rndDir });

  while (!nodePoints.empty())
  {
    auto currentNode = nodePoints.top();

    Position pos = currentNode.first;
    Position dir = currentNode.second;

    int x = pos.X;
    int y = pos.Y;

    _map[x][y].Image = '.';
    _map[x][y].Visited = true;

    int tunMin = tunnelLengthMinMax.X;
    int tunMax = tunnelLengthMinMax.Y;

    int length = RNG::Instance().RandomRange(tunMin, tunMax);

    bool wasBuilt = true;

    while (length > 0)
    {
      if (IsInsideMap({ x + dir.X, y + dir.Y})
       && IsDeadEnd({ x + dir.X, y + dir.Y })
       && !_map[x + dir.X][y + dir.Y].Visited)
      {
        x += dir.X;
        y += dir.Y;

        _map[x][y].Image = '.';
        _map[x][y].Visited = true;
      }
      else
      {
        wasBuilt = false;
        break;
      }

      length--;
    }

    if (wasBuilt)
    {
      //auto str = Util::StringFormat("\tFinished building, adding node point %i %i dir %i %i", x, y, dir.X, dir.Y);
      //Logger::Instance().Print(str);

      nodePoints.push({ { x, y }, dir });
    }

    auto res = TryToGetPerpendicularDir({ x, y }, dir);
    if (res.size() == 0)
    {
      nodePoints.pop();
    }
    else
    {
      nodePoints.push({ { x, y }, res[0] });
    }
  }

  // Starting point might become wall if it was near the corner
  if (additionalTweaks)
  {
    FillDeadEnds();
    CutProblemCorners();
  }

  FillMapRaw();
}

/// Builds tunnels in all directions until maxTunnels are built.
void Tunneler::Normal(Position mapSize, Position tunnelLengthMinMax, Position start, int maxIterations, bool additionalTweaks)
{
  int tunnelsMax = maxIterations;

  _mapSize = mapSize;

  _map = CreateFilledMap(mapSize.X, mapSize.Y);

  int sx, sy = 0;

  if (start.X == -1 || start.Y == -1)
  {
    sx = RNG::Instance().RandomRange(1, mapSize.X - 1);
    sy = RNG::Instance().RandomRange(1, mapSize.Y - 1);
  }
  else
  {
    sx = start.X;
    sy = start.Y;
  }

  _startingPoint.Set(sx, sy);

  std::vector<Position> directions =
  {
    { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 }
  };

  Position mapPos(sx, sy);

  int index = RNG::Instance().RandomRange(0, directions.size());
  Position lastDir = directions[index];

  while (tunnelsMax > 0)
  {
    Position newDir = GetRandomPerpendicularDir(lastDir);

    int currentLength = 0;
    int rndLength = RNG::Instance().RandomRange(tunnelLengthMinMax.X, tunnelLengthMinMax.Y);

    std::vector<Position> corridor;

    while (currentLength <= rndLength && IsInsideMap(mapPos))
    {
      corridor.push_back(mapPos);

      _map[mapPos.X][mapPos.Y].Image = '.';

      mapPos.X += newDir.X;
      mapPos.Y += newDir.Y;

      currentLength++;
    }

    lastDir.Set(newDir);

    index = RNG::Instance().RandomRange(0, corridor.size());

    mapPos = corridor[index];

    tunnelsMax--;
  }

  // Starting point might become wall if it was near the corner
  if (additionalTweaks)
  {
    FillDeadEnds();
    CutProblemCorners();
  }

  FillMapRaw();
}

std::vector<Position> Tunneler::GetRandomDir(Position pos)
{
  std::vector<Position> res;

  std::vector<Position> directions =
  {
    { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 }
  };

  while (!directions.empty())
  {
    int index = RNG::Instance().RandomRange(0, directions.size());
    Position newDir = directions[index];
    int nx = pos.X + newDir.X;
    int ny = pos.Y + newDir.Y;

    if (IsInsideMap({ nx, ny })
     && IsDeadEnd({ nx, ny }))
    {
      res.push_back(newDir);
      break;
    }

    directions.erase(directions.begin() + index);
  }

  return res;
}

std::vector<Position> Tunneler::TryToGetPerpendicularDir(Position pos, Position lastDir)
{
  std::vector<Position> res;

  std::vector<Position> directions =
  {
    { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 }
  };

  std::vector<std::vector<Position>> choicesByDir =
  {
    { { 0, 1 }, { 0, -1 } },
    { { 0, 1 }, { 0, -1 } },
    { { 1, 0 }, { -1, 0 } },
    { { 1, 0 }, { -1, 0 } }
  };

  std::vector<Position> selectedPair;

  //auto str = Util::StringFormat("Trying to get perpendicular dir to %i %i", lastDir.X, lastDir.Y);
  //Logger::Instance().Print(str);

  for (int i = 0; i < directions.size(); i++)
  {
    if (lastDir.X == directions[i].X && lastDir.Y == directions[i].Y)
    {
      selectedPair = choicesByDir[i];

      /*
      Logger::Instance().Print("\tFound pairs:");

      for (auto& i : selectedPair)
      {
        auto str = Util::StringFormat("\t%i %i", i.X, i.Y);
        Logger::Instance().Print(str);
      }
      */

      break;
    }
  }

  while (selectedPair.size() != 0)
  {
    int index = RNG::Instance().RandomRange(0, selectedPair.size());

    int x = pos.X + selectedPair[index].X;
    int y = pos.Y + selectedPair[index].Y;

    //auto str = Util::StringFormat("\t Checking %i %i...", x, y);
    //Logger::Instance().Print(str);

    //str = Util::StringFormat("\t Index %i", index);
    //Logger::Instance().Print(str);

    if (IsInsideMap({ x, y })
     && IsDeadEnd({ x, y })
     && !_map[x][y].Visited)
    {
      //auto str = Util::StringFormat("\t Selected dir %i %i", selectedPair[index].X, selectedPair[index].Y);
      //Logger::Instance().Print(str);

      res.push_back(selectedPair[index]);
      break;
    }

    selectedPair.erase(selectedPair.begin() + index);
  }

  return res;
}

Position Tunneler::GetRandomPerpendicularDir(Position dir)
{
  Position res;

  std::vector<std::vector<Position>> choicesByDir =
  {
    { { 0, 1 }, { 0, -1 } },
    { { 0, 1 }, { 0, -1 } },
    { { 1, 0 }, { -1, 0 } },
    { { 1, 0 }, { -1, 0 } }
  };

  std::vector<Position> directions =
  {
    { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 }
  };

  for (int i = 0; i < directions.size(); i++)
  {
    if (dir.X == directions[i].X && dir.Y == directions[i].Y)
    {
      int index = RNG::Instance().RandomRange(0, 2);
      res = choicesByDir[i][index];
      break;
    }
  }

  return res;
}
