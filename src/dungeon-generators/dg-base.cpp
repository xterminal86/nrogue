#include "dg-base.h"

#include "util.h"

#ifdef DEBUG_BUILD
#include "logger.h"
#endif

void DGBase::PrintMapRaw()
{
  auto raw = GetMapRawString();
  DebugLog("%s\n", raw.data());
}

// =============================================================================

void DGBase::LogPrintMapRaw()
{
  auto raw = GetMapRawString();
  LogPrint(raw);
}

// =============================================================================

void DGBase::FillMapRaw()
{
  MapRaw.clear();

  for (int x = 0; x < _mapSize.X; x++)
  {
    std::vector<char> row;
    for (int y = 0; y < _mapSize.Y; y++)
    {
      row.push_back(_map[x][y].Image);
    }

    MapRaw.push_back(row);
  }
}

// =============================================================================

std::string DGBase::GetMapRawString()
{
  std::string result;

  result = "***** MapRaw *****\n\n";

  result += " ";

  std::string coords = "0123456789";

  int coordIndex = 0;
  for (int y = 0; y < _mapSize.Y; y++)
  {
    result += coords[coordIndex];
    coordIndex++;

    coordIndex = coordIndex % coords.length();
  }

  result += "\n";

  coordIndex = 0;

  for (int x = 0; x < _mapSize.X; x++)
  {
    result += coords[coordIndex];

    for (int y = 0; y < _mapSize.Y; y++)
    {
      result += MapRaw[x][y];
    }

    result += coords[coordIndex];

    result += "\n";

    coordIndex++;

    coordIndex = coordIndex % coords.length();
  }

  coordIndex = 0;

  result += " ";

  for (int y = 0; y < _mapSize.Y; y++)
  {
    result += coords[coordIndex];
    coordIndex++;

    coordIndex = coordIndex % coords.length();
  }

  result += "\n";

  return result;
}

// =============================================================================

std::vector<std::vector<MapCell>> DGBase::CreateEmptyMap(int w, int h)
{
  std::vector<std::vector<MapCell>> map;

  for (int x = 0; x < w; x++)
  {
    std::vector<MapCell> row;
    for (int y = 0; y < h; y++)
    {
      MapCell c;
      c.Coordinates.X = x;
      c.Coordinates.Y = y;
      c.Image = '.';
      c.Visited = false;
      row.push_back(c);
    }

    map.push_back(row);
  }

  return map;
}

// =============================================================================

std::vector<std::vector<MapCell>> DGBase::CreateFilledMap(int w, int h, char image)
{
  std::vector<std::vector<MapCell>> map;

  for (int x = 0; x < w; x++)
  {
    std::vector<MapCell> row;
    for (int y = 0; y < h; y++)
    {
      MapCell c;
      c.Coordinates.X = x;
      c.Coordinates.Y = y;
      c.Image = image;
      c.Visited = false;
      row.push_back(c);
    }

    map.push_back(row);
  }

  return map;
}

// =============================================================================

std::vector<std::vector<MapCell>> DGBase::CreateRandomlyFilledMap(int w, int h, int chance)
{
  std::vector<std::vector<MapCell>> map;

  for (int x = 0; x < w; x++)
  {
    std::vector<MapCell> row;
    for (int y = 0; y < h; y++)
    {
      bool isWall = Util::Rolld100(chance);

      MapCell c;
      c.Coordinates.X = x;
      c.Coordinates.Y = y;
      c.Image = isWall ? '#' : '.';
      c.Visited = false;
      row.push_back(c);
    }

    map.push_back(row);
  }

  return map;
}

// =============================================================================

bool DGBase::CheckLimits(const Position& start, int roomSize)
{
  int lx = start.X;
  int ly = start.Y;
  int hx = lx + roomSize;
  int hy = ly + roomSize;

  bool fitsInMap = (lx >= 0 && hx < _mapSize.X
                 && ly >= 0 && hy < _mapSize.Y);

  if (!fitsInMap)
  {
    return false;
  }

  return true;
}

// =============================================================================

bool DGBase::IsInsideMap(const Position& pos)
{
  return (pos.X >= 1
       && pos.X < _mapSize.X - 1
       && pos.Y >= 1
       && pos.Y < _mapSize.Y - 1);
}

// =============================================================================

//
// Cell is "valid" if it touches only one floor tile.
//
bool DGBase::IsDeadEnd(const Position& p)
{
  int lx = p.X - 1;
  int ly = p.Y - 1;
  int hx = p.X + 1;
  int hy = p.Y + 1;

  int count = 0;

  auto walkable = [this](int x, int y)
  {
    char& img = _map[x][y].Image;

    // Tile is walkable if it's
    //
    // 'ground'
    // 'stone'
    // 'shallow water'
    // 'grass'
    // 'dirt'
    bool cond = (img == '.'
              || img == ' '
              || img == 'w'
              || img == 'g'
              || img == 'd');

    return cond;
  };

  if (walkable(lx, p.Y)) count++;
  if (walkable(hx, p.Y)) count++;
  if (walkable(p.X, ly)) count++;
  if (walkable(p.X, hy)) count++;

  return (count == 1);
}

// =============================================================================

void DGBase::FillDeadEnds()
{
  for (int x = 0; x < _mapSize.X; x++)
  {
    for (int y = 0; y < _mapSize.Y; y++)
    {
      if (IsInsideMap({ x, y }) && IsDeadEnd({ x, y }))
      {
        _map[x][y].Image = '#';
      }
    }
  }
}

// =============================================================================

void DGBase::CutProblemCorners()
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

// =============================================================================

//
// ...    ...
// .#. -> ...
// ...    ...
//
void DGBase::RemoveSingleWalls()
{
  for (int x = 1; x < _mapSize.X - 1; x++)
  {
    for (int y = 1; y < _mapSize.Y - 1; y++)
    {
      if (_map[x][y].Image == '#' && CountAround(x, y, '.') == 8)
      {
        _map[x][y].Image = '.';
      }
    }
  }
}

// =============================================================================

//
// ###    ###
// #.# -> ###
// ###    ###
//
void DGBase::FillSingleCells()
{
  for (int x = 1; x < _mapSize.X - 1; x++)
  {
    for (int y = 1; y < _mapSize.Y - 1; y++)
    {
      if (_map[x][y].Image == '.' && CountAround(x, y, '#') == 8)
      {
        _map[x][y].Image = '#';
      }
    }
  }
}

// =============================================================================

void DGBase::RemoveEndWalls()
{
  bool dontDo = (_endWallsRemovalParams.Passes <= 0);

  int emptyCellsMin = std::min(_endWallsRemovalParams.EmptyCellsAroundMin,
                               _endWallsRemovalParams.EmptyCellsAroundMax);

  int emptyCellsMax = std::max(_endWallsRemovalParams.EmptyCellsAroundMin,
                               _endWallsRemovalParams.EmptyCellsAroundMax);

  bool wrongParams = (emptyCellsMin <= 1 || emptyCellsMax > 8);

  if (dontDo || wrongParams)
  {
    return;
  }

  std::vector<Position> marks;
  for (int i = 0; i < _endWallsRemovalParams.Passes; i++)
  {
    marks.clear();

    for (int x = 1; x < _mapSize.X - 1; x++)
    {
      for (int y = 1; y < _mapSize.Y - 1; y++)
      {
        if (_map[x][y].Image == '#')
        {
          int emptyCellsCount = CountAround(x, y, '.');
          if (emptyCellsCount >= emptyCellsMin
           && emptyCellsCount <= emptyCellsMax)
          {
            marks.push_back({ x, y });
          }
        }
      }
    }

    for (auto& i : marks)
    {
      _map[i.X][i.Y].Image = '.';
    }
  }
}

// =============================================================================

//
// If we found situation like this:
//
// #.#..#
// #.#B.#
// #.A#.#
// #.....
// #.....
//
// we replace random wall around A (which itself is '.')
// with empty space to visually disallow walking into a corner.
//
void DGBase::CheckIfProblemCorner(const Position& p)
{
  int lx = p.X - 1;
  int ly = p.Y - 1;
  int hx = p.X + 1;
  int hy = p.Y + 1;

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

  for (size_t i = 0; i < diagonals.size(); i++)
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

// =============================================================================

int DGBase::CountAround(int x, int y, char ch)
{
  int lx = x - 1;
  int ly = y - 1;
  int hx = x + 1;
  int hy = y + 1;

  int res = 0;

  for (int ix = lx; ix <= hx; ix++)
  {
    for (int iy = ly; iy <= hy; iy++)
    {
      if (x == ix && y == iy)
      {
        continue;
      }

      if (!IsInsideMap({ ix, iy }))
      {
        //res++;
        continue;
      }

      if (_map[ix][iy].Image == ch)
      {
        res++;
      }
    }
  }

  return res;
}

// =============================================================================

double DGBase::GetFillingRatio()
{
  int empty = 0;
  int walls = 0;

  for (int x = 0; x < _mapSize.X; x++)
  {
    for (int y = 0; y < _mapSize.Y; y++)
    {
      if (_map[x][y].Image == '.')
      {
        empty++;
      }
      else if (_map[x][y].Image == '#')
      {
        walls++;
      }
    }
  }

  return ((double)empty / (double)walls);
}

// =============================================================================

Position* DGBase::FindNonMarkedCell()
{
  _nonMarkedCell = { -1, -1 };

  for (int x = 1; x < _mapSize.X - 1; x++)
  {
    for (int y = 1; y < _mapSize.Y - 1; y++)
    {
      if (_map[x][y].Marker == -1
       && _map[x][y].Image == '.')
      {
        _nonMarkedCell = { x, y };
        return &_nonMarkedCell;
      }
    }
  }

  return nullptr;
}

// =============================================================================

void DGBase::AddCellToProcess(const Position& from,
                              Direction dir,
                              std::stack<Position>& addTo)
{
  const std::map<Direction, Position> newPosByLookDir =
  {
    { Direction::WEST,  { from.X,     from.Y - 1 } },
    { Direction::EAST,  { from.X,     from.Y + 1 } },
    { Direction::NORTH, { from.X - 1, from.Y     } },
    { Direction::SOUTH, { from.X + 1, from.Y     } }
  };

  Position newPos = newPosByLookDir.at(dir);
  if (!IsInsideMap(newPos))
  {
    return;
  }

  if (_map[newPos.X][newPos.Y].Image  == '.'
   && _map[newPos.X][newPos.Y].Marker == -1)
  {
    addTo.push(newPos);
  }
}

// =============================================================================

int DGBase::MarkRegions()
{
  _marker = 0;

  _areaPointsByMarker.clear();

  std::stack<Position> toProcess;

  while (true)
  {
    Position* p = FindNonMarkedCell();
    if (p == nullptr)
    {
      break;
    }

    toProcess.push(*p);

    while (!toProcess.empty())
    {
      Position p = toProcess.top();
      toProcess.pop();

      _map[p.X][p.Y].Marker = _marker;

      bool alreadyExists = false;
      for (auto& point : _areaPointsByMarker[_marker])
      {
        if (point == p)
        {
          alreadyExists = true;
          break;
        }
      }

      if (!alreadyExists)
      {
        _areaPointsByMarker[_marker].push_back(p);
      }

      AddCellToProcess(p, Direction::WEST,  toProcess);
      AddCellToProcess(p, Direction::EAST,  toProcess);
      AddCellToProcess(p, Direction::NORTH, toProcess);
      AddCellToProcess(p, Direction::SOUTH, toProcess);
    }

    _marker++;
  }

  return _marker;
}

// =============================================================================

void DGBase::ConnectPoints(const Position& p1, const Position& p2)
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

// =============================================================================

void DGBase::ConnectIsolatedAreas()
{
  const int minDistance = 3;

  int regionsFound = MarkRegions();

  //DebugLog("\n\nRegions found: %i\n\n", regionsFound);

  //
  // If no isolated regions found.
  //
  if (regionsFound <= 1)
  {
    return;
  }

  while (regionsFound > 1)
  {
    std::pair<Position, Position> connectionPointCandidate;

    std::vector<std::pair<Position, Position>> connectionPoints;

    int minD = std::numeric_limits<int>::max();

    //
    // Areas are marked left to right, top to bottom.
    //
    // Find all possible connection points of two adjacent regions.
    //
    for (auto& p1 : _areaPointsByMarker[0])
    {
      for (auto& p2 : _areaPointsByMarker[1])
      {
        int bd = Util::BlockDistance(p1, p2);
        if (bd < minD)
        {
          connectionPointCandidate = { p1, p2 };
          minD = bd;
        }

        if (minD <= minDistance)
        {
          minD = std::numeric_limits<int>::max();
          connectionPoints.push_back(connectionPointCandidate);
        }
      }
    }

    //
    // If walls are too thick and nothing could be found
    // that suits the constraint, get by with at least something.
    //
    if (connectionPoints.empty())
    {
      connectionPoints.push_back(connectionPointCandidate);
    }

    //
    // Get random connection point from all possible variants.
    //
    int index = RNG::Instance().RandomRange(0, connectionPoints.size());
    auto pointsToConnect = connectionPoints[index];
    ConnectPoints(pointsToConnect.first, pointsToConnect.second);

    //DebugLog("\t\t\tconnecting [%i;%i] -> [%i;%i]\n", pointsToConnect.first.X, pointsToConnect.first.Y, pointsToConnect.second.X, pointsToConnect.second.Y);

    //
    // Rinse and repeat.
    //
    UnmarkRegions();

    regionsFound = MarkRegions();

    //DebugLog("\n\n\tNew regions found: %i\n\n", regionsFound);
  }
}

// =============================================================================

void DGBase::UnmarkRegions()
{
  for (int x = 0; x < _mapSize.X; x++)
  {
    for (int y = 0; y < _mapSize.Y; y++)
    {
      if (_map[x][y].Image == '.')
      {
        _map[x][y].Marker = -1;
      }
    }
  }
}
