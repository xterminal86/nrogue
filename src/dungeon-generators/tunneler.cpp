#include "tunneler.h"

#include "util.h"

///
/// \brief Builds tunnels perpendicular to previous direction,
/// backtracks to previous position if failed.
///
/// Additional tweaks include removal of deadends and
/// ....
/// .A#.
/// .#B.
/// ....
/// situations
///
void Tunneler::Backtracking(const Position& mapSize,
                            const Position& tunnelLengthMinMax,
                            const Position& start,
                            bool additionalTweaks)
{
  _mapSize = mapSize;

  _map = CreateFilledMap(mapSize.X, mapSize.Y);

  int sx, sy = 0;

  if (start.X == -1 || start.Y == -1)
  {
    sx = Util::RandomRange(1, mapSize.X - 1, _rng);
    sy = Util::RandomRange(1, mapSize.Y - 1, _rng);
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

  Position* rndDir = GetRandomDir(_startingPoint);

  nodePoints.push({ _startingPoint, (rndDir == nullptr) ? Position() : *rndDir });

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

    int length = Util::RandomRange(tunMin, tunMax, _rng);

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

    Position* res = TryToGetPerpendicularDir({ x, y }, dir);
    if (res == nullptr)
    {
      nodePoints.pop();
    }
    else
    {
      nodePoints.push({ { x, y }, *res });
    }
  }

  //
  // NOTE: starting point might become wall on printout
  // if it was near the corner or was filled during post-processing.
  //
  // Dead ends are filled to avoid accidental making of new
  // problem corners during cutting of problem corners.
  //
  if (additionalTweaks)
  {
    FillDeadEnds();
    CutProblemCorners();
  }

  FillMapRaw();
}

// =============================================================================

///
/// Builds tunnels in random directions until maxIterations.
/// Does not cross already carved tunnel.
/// maxIterations is empirically chosen but should be around
/// (mapSize.X * mapSize.Y) / 2 to cover all map area.
///
void Tunneler::Normal(const Position& mapSize,
                      const Position& tunnelLengthMinMax,
                      const Position& start,
                      int maxIterations,
                      bool additionalTweaks)
{
  int iterations = maxIterations;

  _mapSize = mapSize;

  _map = CreateFilledMap(mapSize.X, mapSize.Y);

  int sx, sy = 0;

  if (start.X == -1 || start.Y == -1)
  {
    //
    // If starting position was poorly chosen (to be [1;1] for example),
    // we might end up in situation where we started from corner,
    // chose direction towards the edge of the map,
    // and failed to generate anything because of that.
    // Also post-processing can fill back carved corridors,
    // so let's start from center for good measure.
    //
    sx = mapSize.X / 2;
    sy = mapSize.Y / 2;
  }
  else
  {
    sx = start.X;
    sy = start.Y;
  }

  _startingPoint.Set(sx, sy);

  Position mapPos(sx, sy);

  std::vector<Position> carvedPoints;

  _map[mapPos.X][mapPos.Y].Image = '.';

  while (iterations > 0)
  {
    Position* newDir = GetCorridorDir(mapPos);

    if (newDir != nullptr)
    {
      int currentLength = 0;
      int rndLength = Util::RandomRange(tunnelLengthMinMax.X,
                                        tunnelLengthMinMax.Y,
                                        _rng);

      std::vector<Position> corridor;

      mapPos.X += newDir->X;
      mapPos.Y += newDir->Y;

      while (currentLength <= rndLength
          && IsInsideMap(mapPos)
          && IsDeadEnd(mapPos))
      {
        corridor.push_back(mapPos);
        carvedPoints.push_back(mapPos);

        _map[mapPos.X][mapPos.Y].Image = '.';

        mapPos.X += newDir->X;
        mapPos.Y += newDir->Y;

        currentLength++;
      }
    }

    if (carvedPoints.size() != 0)
    {
      int index = Util::RandomRange(0, carvedPoints.size(), _rng);
      mapPos = carvedPoints[index];

      carvedPoints.erase(carvedPoints.begin() + index);
    }
    else
    {
      //
      // If we couldn't continue carving in the chosen direction
      // return to starting point and try everything again.
      // Without it we could basically fail to generate a map
      // if starting point and carve direction were poorly chosen
      // (e.g. near the map edge and carving towards the bound as well
      // while carved points were deleted in such way, that the only point
      // left was the one near the map edge).
      //
      mapPos = _startingPoint;
    }

    iterations--;
  }

  if (additionalTweaks)
  {
    FillDeadEnds();
    CutProblemCorners();
  }

  FillMapRaw();
}

// =============================================================================

Position* Tunneler::GetRandomDir(const Position& pos)
{
  Position* res = nullptr;

  std::vector<Position> directions =
  {
    { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 }
  };

  while (!directions.empty())
  {
    int index = Util::RandomRange(0, directions.size(), _rng);
    Position newDir = directions[index];
    int nx = pos.X + newDir.X;
    int ny = pos.Y + newDir.Y;

    if (IsInsideMap({ nx, ny })
     && IsDeadEnd({ nx, ny }))
    {
      _randomDir = newDir;
      res = &_randomDir;
      break;
    }

    directions.erase(directions.begin() + index);
  }

  return res;
}

// =============================================================================

Position* Tunneler::TryToGetPerpendicularDir(const Position& pos, const Position& lastDir)
{
  Position* res = nullptr;

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

  for (size_t i = 0; i < directions.size(); i++)
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
    int index = Util::RandomRange(0, selectedPair.size(), _rng);

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

      _perpendicularDir = selectedPair[index];
      res = &_perpendicularDir;
      break;
    }

    selectedPair.erase(selectedPair.begin() + index);
  }

  return res;
}

// =============================================================================

Position Tunneler::GetRandomPerpendicularDir(const Position& dir)
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

  for (size_t i = 0; i < directions.size(); i++)
  {
    if (dir.X == directions[i].X && dir.Y == directions[i].Y)
    {
      int index = Util::RandomRange(0, 2, _rng);
      res = choicesByDir[i][index];
      break;
    }
  }

  return res;
}

// =============================================================================

bool Tunneler::IsDirectionValid(const Position& pos, const Position& dir)
{
  Position newPos;

  newPos.X = pos.X + dir.X;
  newPos.Y = pos.Y + dir.Y;

  if (IsInsideMap(newPos) && _map[newPos.X][newPos.Y].Image == '#')
  {
    return true;
  }

  return false;
}

// =============================================================================

Position* Tunneler::GetCorridorDir(const Position& pos)
{
  Position* res = nullptr;

  std::vector<Position> directions =
  {
    { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 }
  };

  std::vector<Position> validDirs;

  for (auto& d : directions)
  {
    if (IsDirectionValid(pos, d))
    {
      validDirs.push_back(d);
    }
  }

  if (validDirs.size() != 0)
  {
    int index = Util::RandomRange(0, validDirs.size(), _rng);
    _corridorDir = validDirs[index];
    res = &_corridorDir;
  }

  return res;
}
