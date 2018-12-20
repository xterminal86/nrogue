#include "recursive-backtracker.h"

/// 1. Get random direction.
/// 2. Try to replace wall with empty space.
/// 3. Backtrack to previous cell if failed.
/// 4. Repeat until all cells are visited.
void RecursiveBacktracker::Generate(Position mapSize, Position startingPoint)
{
  std::stack<Position> openCells;

  _mapSize = mapSize;

  _map = CreateFilledMap(mapSize.X, mapSize.Y);

  int sx, sy = 0;

  if (startingPoint.X == -1 || startingPoint.Y == -1)
  {
    sx = RNG::Instance().RandomRange(1, mapSize.X - 1);
    sy = RNG::Instance().RandomRange(1, mapSize.Y - 1);
  }
  else
  {
    sx = startingPoint.X;
    sy = startingPoint.Y;
  }

  _startingPoint.Set(sx, sy);

  _map[sx][sy].Visited = true;
  _map[sx][sy].Image = '.';

  Position p(sx, sy);
  openCells.push(p);

  while (!openCells.empty())
  {
    auto np = openCells.top();
    auto res = GetRandomCell(np);
    if (res.size() != 0)
    {
      openCells.push(res[0]);
    }
    else
    {
      openCells.pop();
    }
  }

  CutProblemCorners();
  FillMapRaw();
}

void RecursiveBacktracker::CutProblemCorners()
{
  for (int x = 0; x < _mapSize.X; x++)
  {
    for (int y = 0; y < _mapSize.Y; y++)
    {
      Position p(x, y);
      CheckIfProblemCorner(p);
    }
  }
}

/// If we found situation like this:
///
/// #.#..#
/// #.#B.#
/// #.A#.#
/// #.....
/// #.....
///
/// we replace random wall around A (which is '.') with empty space
/// to disallow diagonal walking from A to B
void RecursiveBacktracker::CheckIfProblemCorner(Position p)
{
  int lx = p.X - 1;
  int ly = p.Y - 1;
  int hx = p.X + 1;
  int hy = p.Y + 1;

  int count = 0;

  if (lx < 0 || ly < 0 || hx > _mapSize.X - 1 || hy > _mapSize.Y - 1)
  {
    return;
  }

  std::vector<Position> diagonals =
  {
    { lx, ly },
    { hx, ly },
    { lx, hy },
    { hx, hy }
  };

  std::vector<std::vector<Position>> choices =
  {
    { { -1,  0 },  { 0, -1 } },
    { {  1,  0 },  { 0, -1 } },
    { { -1,  0 },  { 0,  1 } },
    { {  1,  0 },  { 0,  1 } }
  };

  for (int i = 0; i < diagonals.size(); i++)
  {
    Position d = diagonals[i];

    int wallX1 = p.X + choices[i][0].X;
    int wallY1 = p.Y + choices[i][0].Y;
    int wallX2 = p.X + choices[i][1].X;
    int wallY2 = p.Y + choices[i][1].Y;

    bool hasWalls = (_map[wallX1][wallY1].Image == '#'
                  && _map[wallX2][wallY2].Image == '#');

    if (_map[p.X][p.Y].Image == '.'
     && _map[d.X][d.Y].Image == '.'
     && hasWalls)
    {
      int rndIndex = RNG::Instance().RandomRange(0, 2);
      Position choice = choices[i][rndIndex];
      _map[p.X + choice.X][p.Y + choice.Y].Image = '.';
      break;
    }
  }
}

std::vector<Position> RecursiveBacktracker::GetRandomCell(Position p)
{
  std::vector<Position> res;

  std::map<RoomEdgeEnum, Position> offsetsBySide =
  {
    { RoomEdgeEnum::NORTH, { -1, 0 } },
    { RoomEdgeEnum::SOUTH, { 1, 0 }  },
    { RoomEdgeEnum::EAST,  { 0, 1 }  },
    { RoomEdgeEnum::WEST,  { 0, -1 } }
  };

  std::vector<Position> candidates;

  for (auto& kvp : offsetsBySide)
  {
    Position newPos;
    newPos.X = p.X + kvp.second.X;
    newPos.Y = p.Y + kvp.second.Y;

    if (IsInsideMap(newPos)
     && IsDeadEnd(newPos)
     && !_map[newPos.X][newPos.Y].Visited)
    {
      candidates.push_back(newPos);
    }
  }

  if (candidates.size() != 0)
  {
    int index = RNG::Instance().RandomRange(0, candidates.size());
    Position cp = candidates[index];
    _map[cp.X][cp.Y].Visited = true;
    _map[cp.X][cp.Y].Image = '.';
    res.push_back(cp);
  }

  return res;
}

