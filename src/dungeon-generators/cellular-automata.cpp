#include "cellular-automata.h"

/// Use cellular automata to generate cave-like area.
void CellularAutomata::Generate(Position mapSize, int initialWallChance,
                                    int birthThreshold, int deathThreshold,
                                    int maxIterations)
{
  _mapSize = mapSize;

  _map = CreateRandomlyFilledMap(mapSize.X, mapSize.Y, initialWallChance);

  // If we change data in-place we would mix old and
  // new results, so we should perform check on initial map
  // and store results in temporary one and copy them after
  // whole initial map was scanned.
  auto tmp = CreateEmptyMap(mapSize.X, mapSize.Y);

  for (int i = 0; i < maxIterations; i++)
  {
    // copy original map contents to temp
    for (int x = 0; x < mapSize.X; x++)
    {
      for (int y = 0; y < mapSize.Y; y++)
      {
        tmp[x][y].Image = _map[x][y].Image;
      }
    }

    for (int x = 0; x < mapSize.X; x++)
    {
      for (int y = 0; y < mapSize.Y; y++)
      {
        if (_map[x][y].Visited)
        {
          continue;
        }

        int aliveCells = CountAround(x, y, '.');
        if (_map[x][y].Image == '.')
        {
          tmp[x][y].Image = (aliveCells < deathThreshold) ? '#' : '.';
        }
        else if (_map[x][y].Image == '#')
        {
          tmp[x][y].Image = (aliveCells > birthThreshold) ? '.' : '#';
        }
      }
    }

    // copy temp array to original map
    for (int x = 0; x < mapSize.X; x++)
    {
      for (int y = 0; y < mapSize.Y; y++)
      {
        _map[x][y].Image = tmp[x][y].Image;
      }
    }
  }

  ConnectIsolatedAreas();
  CutProblemCorners();
  FillMapRaw();
}

void CellularAutomata::ConnectIsolatedAreas()
{
  _markerValue = 0;

  _areasByMarker.clear();

  while (true)
  {
    auto res = FindNonMarkedCell();
    if (res.size() == 0)
    {
      break;
    }

    FloodFill(res[0]);

    _markerValue++;
  }

  if (_areasByMarker.size() > 1)
  {
    for (int i = 0; i < _markerValue; i++)
    {
      auto res = FindClosestPointsToArea(i);
      ConnectPoints(res.first, res.second);
    }
  }
}

std::vector<Position> CellularAutomata::FindNonMarkedCell()
{
  std::vector<Position> res;

  for (int x = 0; x < _mapSize.X; x++)
  {
    for (int y = 0; y < _mapSize.Y; y++)
    {
      if (_map[x][y].Image == '.' && _map[x][y].Marker == -1)
      {
        res.push_back({ x, y });
        return res;
      }
    }
  }

  return res;
}

void CellularAutomata::FloodFill(Position start)
{
  std::queue<Position> visitedCells;

  int x = start.X;
  int y = start.Y;

  _map[x][y].Marker = _markerValue;

  visitedCells.push(start);

  while (!visitedCells.empty())
  {
    Position c = visitedCells.front();

    int lx = c.X - 1;
    int hx = c.X + 1;
    int ly = c.Y - 1;
    int hy = c.Y + 1;

    TryToMarkCell({ lx, c.Y }, visitedCells);
    TryToMarkCell({ hx, c.Y }, visitedCells);
    TryToMarkCell({ c.X, ly }, visitedCells);
    TryToMarkCell({ c.X, hy }, visitedCells);

    visitedCells.pop();
  }
}

void CellularAutomata::TryToMarkCell(Position p, std::queue<Position>& visitedCells)
{
  if (IsInsideMap(p)
   && _map[p.X][p.Y].Image == '.'
   && _map[p.X][p.Y].Marker == -1)
  {
    _map[p.X][p.Y].Marker = _markerValue;
    visitedCells.push({ p.X, p.Y });

    _areasByMarker[_markerValue].push_back({ p.X, p.Y });
  }
}

void CellularAutomata::ConnectPoints(Position p1, Position p2)
{
  int dirX = (p1.X > p2.X) ? -1 : 1;
  int dirY = (p1.Y > p2.Y) ? -1 : 1;

  Position tmp1 = p1;
  Position tmp2 = p2;

  while (tmp1.X != tmp2.X)
  {
    _map[tmp1.X][tmp1.Y].Image = '.';
    tmp1.X += dirX;
  }

  while (tmp1.Y != tmp2.Y)
  {
    _map[tmp1.X][tmp1.Y].Image = '.';
    tmp1.Y += dirY;
  }
}

std::pair<Position, Position> CellularAutomata::FindClosestPointsToArea(int areaMarker)
{
  std::pair<Position, Position> res;

  int minDistance = 3;

  for (auto& p1 : _areasByMarker[areaMarker])
  {
    int minD = _mapSize.X * _mapSize.Y;

    for (auto& kvp : _areasByMarker)
    {
      // Skip area we are currently checking
      if (kvp.first == areaMarker)
      {
        continue;
      }

      for (auto& p2 : _areasByMarker[kvp.first])
      {
        int bd = Util::BlockDistance(p1, p2);
        if (bd < minD)
        {
          res = { p1, p2 };
          minD = bd;
        }

        if (minD <= minDistance)
        {
          //printf ("min distance points: %i %i - %i %i\n", res.first.X, res.first.Y, res.second.X, res.second.Y);
          return res;
        }
      }
    }
  }

  //printf ("min distance points: %i %i - %i %i\n", res.first.X, res.first.Y, res.second.X, res.second.Y);

  return res;
}

