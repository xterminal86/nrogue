#include "dg-base.h"

#include "logger.h"
#include "util.h"

void DGBase::PrintMapRaw()
{
  auto raw = GetMapRawString();
  DebugLog("%s\n", raw.data());
}

void DGBase::LogPrintMapRaw()
{
  auto raw = GetMapRawString();
  Logger::Instance().Print(raw);
}

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

std::vector<std::vector<MapCell>> DGBase::CreateFilledMap(int w, int h)
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
      c.Image = '#';
      c.Visited = false;
      row.push_back(c);
    }

    map.push_back(row);
  }

  return map;
}

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

bool DGBase::IsInsideMap(const Position& pos)
{
  return (pos.X >= 1
       && pos.X < _mapSize.X - 1
       && pos.Y >= 1
       && pos.Y < _mapSize.Y - 1);
}

// Cell is "valid" if it touches only one floor tile
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

/// If we found situation like this:
///
/// #.#..#
/// #.#B.#
/// #.A#.#
/// #.....
/// #.....
///
/// we replace random wall around A (which itself is '.')
/// with empty space to disallow diagonal walking from A to B
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
