#include "dg-base.h"

#include "util.h"

#ifdef DEBUG_BUILD
#include "logger.h"
#endif

DGBase::DGBase()
{
  _rng.seed(RNG::Instance().Seed);
}

// =============================================================================

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
    result.append(1, coords[coordIndex]);
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
      result.append(1, MapRaw[x][y]);
    }

    result.append(1, coords[coordIndex]);

    result += "\n";

    coordIndex++;

    coordIndex = coordIndex % coords.length();
  }

  coordIndex = 0;

  result += " ";

  for (int y = 0; y < _mapSize.Y; y++)
  {
    result.append(1, coords[coordIndex]);
    coordIndex++;

    coordIndex = coordIndex % coords.length();
  }

  result += "\n";

  return result;
}

// =============================================================================

const std::vector<std::vector<MapCell>>& DGBase::GeneratedMap()
{
  return _map;
}

// =============================================================================

void DGBase::ForCustomDebugStuff()
{
  //
  // Override this for custom printf debugging and stuff inside generator class.
  //
}

// =============================================================================

std::vector<std::vector<MapCell>> DGBase::CreateFilledMap(int w, int h, char image)
{
  std::vector<std::vector<MapCell>> map;

  map.reserve(w);

  for (int x = 0; x < w; x++)
  {
    std::vector<MapCell> row;

    row.reserve(h);

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

  map.reserve(w);

  for (int x = 0; x < w; x++)
  {
    std::vector<MapCell> row;

    row.reserve(h);

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

bool DGBase::IsInBounds(int x, int y)
{
  bool outOfBounds = (x < 0 || x > _mapSize.X - 1
                   || y < 0 || y > _mapSize.Y - 1);

  return !outOfBounds;
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
      int rndIndex = Util::RandomRange(0, 2, _rng);
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

const StringV& DGBase::ExtractMapChunk(int x, int y, int w, int h)
{
  _mapChunk.clear();

  int hx = x + w;
  int hy = y + h;

  if (!IsInBounds(x, y) || !IsInBounds(hx, hy))
  {
    return _mapChunk;
  }

  for (int i = x; i <= hx; i++)
  {
    std::string line;
    for (int j = y; j <= hy; j++)
    {
      line.append(1, _map[i][j].Image);
    }

    _mapChunk.push_back(line);
  }

  return _mapChunk;
}

// =============================================================================

bool DGBase::FillMapChunk(int x, int y, int w, int h, char with, bool markVisited)
{
  int hx = x + w;
  int hy = y + h;

  if (!IsInBounds(x, y) || !IsInBounds(hx, hy))
  {
    return false;
  }

  for (int i = x; i <= hx; i++)
  {
    for (int j = y; j <= hy; j++)
    {
      _map[i][j].Image = with;

      if (markVisited)
      {
        _map[i][j].Visited = true;
      }
    }
  }

  return true;
}

// =============================================================================

bool DGBase::VisitArea(int x, int y, int w, int h)
{
  int hx = x + w;
  int hy = y + h;

  if (!IsInBounds(x, y) || !IsInBounds(hx, hy))
  {
    return false;
  }

  for (int i = x; i <= hx; i++)
  {
    for (int j = y; j <= hy; j++)
    {
      _map[i][j].Visited = true;
    }
  }

  return true;
}

// =============================================================================

bool DGBase::AreChunksEqual(const StringV& chunk1, const StringV& chunk2)
{
  //
  // Assuming dimensions are equal.
  //
  if ( (chunk1.size() != chunk2.size())
    || (chunk1[0].length() != chunk2[0].length()) )
  {
    return false;
  }

  for (size_t i = 0; i < chunk1.size(); i++)
  {
    for (size_t j = 0; j < chunk1[0].length(); j++)
    {
      if (chunk1[i][j] != chunk2[i][j])
      {
        return false;
      }
    }
  }

  return true;
}

// =============================================================================

double DGBase::GetEmptyPercent()
{
  int total = _mapSize.X * _mapSize.Y;

  int empty = 0;

  for (int x = 0; x < _mapSize.X; x++)
  {
    for (int y = 0; y < _mapSize.Y; y++)
    {
      if (_map[x][y].Image == '.')
      {
        empty++;
      }
    }
  }

  return ((double)empty / (double)total);
}

// =============================================================================

MapCell* DGBase::GetCell(int x, int y)
{
  MapCell* res = nullptr;

  if (IsInBounds(x, y))
  {
    res = &_map[x][y];
  }

  return res;
}

// =============================================================================

bool DGBase::IsCorner(int x, int y, CornerType cornerType)
{
  bool res = false;

  switch (cornerType)
  {
    case CornerType::UL:
    {
      //
      // #?
      // ?.
      //
      res = (_map[x][y].Image == '#'
          && _map[x + 1][y].Image != '.'
          && _map[x][y + 1].Image != '.'
          && _map[x + 1][y + 1].Image == '.');
    }
    break;

    case CornerType::UR:
    {
      //
      // ?#
      // .?
      //
      res = (_map[x][y].Image == '#'
          && _map[x + 1][y].Image != '.'
          && _map[x][y - 1].Image != '.'
          && _map[x + 1][y - 1].Image == '.');
    }
    break;

    case CornerType::DL:
    {
      //
      // ?.
      // #?
      //
      res = (_map[x][y].Image == '#'
          && _map[x - 1][y].Image != '.'
          && _map[x][y + 1].Image != '.'
          && _map[x - 1][y + 1].Image == '.');
    }
    break;

    case CornerType::DR:
    {
      //
      // .?
      // ?#
      //
      res = (_map[x][y].Image == '#'
          && _map[x - 1][y].Image != '.'
          && _map[x][y - 1].Image != '.'
          && _map[x - 1][y - 1].Image == '.');
    }
    break;
  }

  return res;
}

// =============================================================================

Position* DGBase::FindCorner(int x, int y, CornerType cornerToFind)
{
  auto IsWallOk = [this](int x, int y)
  {
    return (_map[x][y].Image == '#' || _map[x][y].Image == '+');
  };

  Position* res = nullptr;

  switch (cornerToFind)
  {
    case CornerType::UR:
    {
      for (int i = y; i < _mapSize.Y; i++)
      {
        if (!IsWallOk(x, i))
        {
          break;
        }

        if (IsCorner(x, i, CornerType::UR))
        {
          _cornerPos = { x, i };
          res = &_cornerPos;
          break;
        }
      }
    }
    break;

    case CornerType::DR:
    {
      for (int i = x; i < _mapSize.X; i++)
      {
        if (!IsWallOk(i, y))
        {
          break;
        }

        if (IsCorner(i, y, CornerType::DR))
        {
          _cornerPos = { i, y };
          res = &_cornerPos;
          break;
        }
      }
    }
    break;

    case CornerType::DL:
    {
      for (int i = y; i >= 0; i--)
      {
        if (!IsWallOk(x, i))
        {
          break;
        }

        if (IsCorner(x, i, CornerType::DL))
        {
          _cornerPos = { x, i };
          res = &_cornerPos;
          break;
        }
      }
    }
    break;

    case CornerType::UL:
    {
      for (int i = x; i >= 0; i--)
      {
        if (!IsWallOk(i, y))
        {
          break;
        }

        if (IsCorner(i, y, CornerType::UL))
        {
          _cornerPos = { i, y };
          res = &_cornerPos;
          break;
        }
      }
    }
    break;
  }

  return res;
}

// =============================================================================

bool DGBase::IsAreaEmpty(int x1, int y1, int x2, int y2)
{
  for (int x = x1; x <= x2; x++)
  {
    for (int y = y1; y <= y2; y++)
    {
      if (_map[x][y].Image != '.' || _map[x][y].ZoneMarker != TransformedRoom::UNMARKED)
      {
        return false;
      }
    }
  }

  return true;
}

// =============================================================================

const std::vector<Rect>& DGBase::GetEmptyRooms()
{
  _emptyRooms.clear();

  //
  // Try to find empty rooms by encircling clockwise from starting corner.
  //
  // 1. Try to find upper left corner of a potential room.
  // 2. If found, draw line to the right until you find upper right corner.
  // 3. Draw down to find lower right corner.
  // 4. Draw left to find lower left corner.
  // 5. Check if found area is actually empty and Y coordinate
  //    of lower left corner coincides with upper left corner.
  //    so that we won't mistakenly detect situation like this:
  //
  //      Problem
  //     /
  //    ##
  //    #.
  //    # UL    UR
  //    #.#######
  //    #.#.....#
  //    #.#.....#
  //    #.......#
  //    #########
  //    DL      DR
  //
  for (int x = 0; x < _mapSize.X - 1; x++)
  {
    for (int y = 0; y < _mapSize.Y - 1; y++)
    {
      if (IsCorner(x, y, CornerType::UL))
      {
        Position* p = FindCorner(x, y + 1, CornerType::UR);
        if (p != nullptr)
        {
          Position ur = *p;

          p = FindCorner(ur.X + 1, ur.Y, CornerType::DR);
          if (p != nullptr)
          {
            Position dr = *p;

            p = FindCorner(dr.X, dr.Y - 1, CornerType::DL);
            if (p != nullptr && p->Y == y)
            {
              Position dl = *p;

              p = FindCorner(dl.X - 1, dl.Y, CornerType::UL);
              if (p != nullptr)
              {
                if (IsAreaEmpty(x + 1, y + 1, dr.X - 1, dr.Y - 1))
                {
                  //
                  // Do not count corridors as empty rooms, i.e.
                  //
                  //          ###
                  // #####    #.#
                  // #...# or #.#
                  // #####    #.#
                  //          ###
                  //
                  bool notACorridor = ( (dr.X - 1) - (x + 1) > 1
                                     && (dr.Y - 1) - (y + 1) > 1 );
                  if (notACorridor)
                  {
                    Rect r;

                    r.X1 = x + 1;
                    r.Y1 = y + 1;
                    r.X2 = dr.X - 1;
                    r.Y2 = dr.Y - 1;

                    _emptyRooms.push_back(r);
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  return _emptyRooms;
}

// =============================================================================

void DGBase::TransformRooms(const TransformedRoomsWeights& weights)
{
  if (_emptyRooms.empty())
  {
    DebugLog("no empty rooms - nothing to transform!");
    return;
  }

  std::unordered_map<TransformedRoom, int> generatedSoFar;
  std::unordered_map<TransformedRoom, int> roomWeightByType;

  for (auto& kvp : weights)
  {
    const TransformedRoom& type = kvp.first;
    const int& weight           = kvp.second.first;

    generatedSoFar[type]   = 0;
    roomWeightByType[type] = weight;
  }

  std::shuffle(_emptyRooms.begin(), _emptyRooms.end(), _rng);

  for (size_t i = 0; i < _emptyRooms.size(); i++)
  {
    //
    // Some rooms may have size requirements, so before trying to place them
    // at the spot of this index, we'll check every available rooms
    // and then pick one at random, thus marking that zone.
    // The room that we picked this way may be somewhere in the middle
    // of _emptyRooms, so in TransformArea() below we may actually
    // mark some room beforehand in this way, so on next iteration
    // of this for loop it will be already marked.
    //
    if (_map[_emptyRooms[i].X1][_emptyRooms[i].Y1].ZoneMarker != TransformedRoom::UNMARKED)
    {
      continue;
    }

    auto res = Util::WeightedRandom(roomWeightByType);

    int maxAllowed = weights.at(res.first).second;
    if (maxAllowed > 0 && generatedSoFar[res.first] >= maxAllowed)
    {
      continue;
    }

    if (TransformArea(res.first, i))
    {
      generatedSoFar[res.first]++;
    }
  }

  //
  // Some cleanup just in case.
  //
  _emptyRooms.clear();
  _failures.clear();

  FillMapRaw();
}

// =============================================================================

bool DGBase::TransformArea(TransformedRoom type, size_t emptyRoomIndex)
{
  if (_failures.count(type) == 1)
  {
    //DebugLog("This room is a failure - cannot be placed in this map");
    return false;
  }

  //
  // TODO: place illusionary walls / doors for interesting areas
  //       by scanning area surrounding walls.
  //
  bool success = true;

  const Rect& area = _emptyRooms[emptyRoomIndex];

  switch (type)
  {
    // -------------------------------------------------------------------------

    case TransformedRoom::EMPTY:
      MarkAreaEmpty(area);
      break;

    // -------------------------------------------------------------------------

    case TransformedRoom::SHRINE:
    {
      //DebugLog("Trying to place SHRINE at %s", area.ToString().data());

      //
      // "As described in Callable, when invoking a pointer to non-static member
      // function or pointer to non-static data member, the first argument
      // has to be a reference or pointer (including, possibly, smart pointer
      // such as std::shared_ptr and std::unique_ptr) to an object whose member
      // will be accessed."
      //
      auto fn = std::bind(&DGBase::PlaceShrine, this, std::placeholders::_1);

      /*
      success = TryToPlaceRoom({
                                 { 5, 5 },
                                 { 5, 7 },
                                 { 7, 5 },
                                 { 7, 7 }
                               },
                               area, emptyRoomIndex, type, fn);
      */

      success = TryToPlaceRoom(5, 7, area, emptyRoomIndex, type, fn);

      if (!success)
      {
        _failures[type] = true;
      }
    }
    break;

    // -------------------------------------------------------------------------

    case TransformedRoom::TREASURY:
    {
      //DebugLog("Trying to place TREASURY at %s", area.ToString().data());

      auto fn = std::bind(&DGBase::PlaceTreasury, this, std::placeholders::_1);
      success = TryToPlaceRoom(5, 9, area, emptyRoomIndex, type, fn);

      if (!success)
      {
        _failures[type] = true;
      }
    }
    break;

    // -------------------------------------------------------------------------

    case TransformedRoom::STORAGE:
    {
      auto fn = std::bind(&DGBase::PlaceStorage, this, std::placeholders::_1);
      success = TryToPlaceRoom(3, 7, area, emptyRoomIndex, type, fn);

      if (!success)
      {
        _failures[type] = true;
      }
    }
    break;

    // -------------------------------------------------------------------------

    case TransformedRoom::FLOODED:
    {
      for (int x = area.X1; x <= area.X2; x++)
      {
        for (int y = area.Y1; y <= area.Y2; y++)
        {
          _map[x][y].ZoneMarker = type;
          _map[x][y].Image      = 'W';
        }
      }
    }
    break;

    // -------------------------------------------------------------------------

    case TransformedRoom::CHESTROOM:
    {
      auto fn = std::bind(&DGBase::PlaceChestroom, this, std::placeholders::_1);
      success = TryToPlaceRoom(3, 5, area, emptyRoomIndex, type, fn);

      if (!success)
      {
        _failures[type] = true;
      }
    }
    break;

    // -------------------------------------------------------------------------

    default:
      DebugLog("Unknown room type - %d", (int)type);
      success = false;
      break;
  }

  return success;
}

// =============================================================================

bool DGBase::DoesAreaFit(const Rect& area, int minSize, int maxSize)
{
  return (area.Width()  >= minSize && area.Width()  <= maxSize
       && area.Height() >= minSize && area.Height() <= maxSize);
}

// =============================================================================

bool DGBase::DoesAreaFitExactly(const Rect& area, const PairII& size)
{
  return (area.Width() == size.first && area.Height() == size.second);
}

// =============================================================================

std::vector<size_t> DGBase::TryToFindSuitableRooms(int minSize, int maxSize,
                                                   size_t skipRoomIndex)
{
  std::vector<size_t> res;

  //DebugLog("  trying to find suitable rooms for minSize = %d, maxSize = %d", minSize, maxSize);

  for (size_t i = 0; i < _emptyRooms.size(); i++)
  {
    if (i == skipRoomIndex)
    {
      continue;
    }

    const Rect& area = _emptyRooms[i];

    //
    // Check if room candidate satisfies size requirements and was not already
    // transformed by somebody else.
    //
    bool isNotMarked = (_map[area.X1][area.Y1].ZoneMarker == TransformedRoom::UNMARKED);
    if (DoesAreaFit(area, minSize, maxSize) && isNotMarked)
    {
      //DebugLog("  found room %s", _emptyRooms[i].ToString().data());
      res.push_back(i);
    }
  }

  return res;
}

// =============================================================================

std::vector<size_t> DGBase::TryToFindSuitableRooms(const std::vector<PairII>& exactSizes,
                                                   size_t skipRoomIndex)
{
  std::vector<size_t> res;

  for (auto& item : exactSizes)
  {
    int minSize = item.first;
    int maxSize = item.second;

    //DebugLog("  trying to find suitable rooms for [%d ; %d]", minSize, maxSize);

    for (size_t i = 0; i < _emptyRooms.size(); i++)
    {
      if (i == skipRoomIndex)
      {
        continue;
      }

      const Rect& area = _emptyRooms[i];

      bool isNotMarked = (_map[area.X1][area.Y1].ZoneMarker == TransformedRoom::UNMARKED);
      bool exactSize = (area.Width() == minSize && area.Height() == maxSize);
      if (exactSize && isNotMarked)
      {
        //DebugLog("  found room %s", _emptyRooms[i].ToString().data());
        res.push_back(i);
      }
    }
  }

  return res;
}

// =============================================================================

void DGBase::PlaceTreasury(const Rect& area)
{
  bool toggler = false;
  int counter = 0;

  for (int x = area.X1; x <= area.X2; x++)
  {
    toggler = !toggler;
    counter = toggler ? 0 : 1;

    for (int y = area.Y1; y <= area.Y2; y++)
    {
      _map[x][y].Image = (counter % 2 == 0) ? '1' : '2';
      _map[x][y].ZoneMarker = TransformedRoom::TREASURY;

      if (Util::Rolld100(30))
      {
        _map[x][y].ObjectHere = ItemType::COINS;
      }
      else
      {
        _map[x][y].ObjectHere = GameObjectType::NONE;
      }

      counter++;
    }
  }
}

// =============================================================================

void DGBase::PlaceShrine(const Rect& area)
{
  MarkZone(area, TransformedRoom::SHRINE);

  int index = Util::RandomRange(0, GlobalConstants::ShrineLayouts.size(), _rng);
  auto it = GlobalConstants::ShrineLayoutsByType.begin();
  std::advance(it, index);
  int layoutIndex = Util::RandomRange(0, it->second.size(), _rng);
  auto layout = it->second[layoutIndex];

  const std::vector<RoomLayoutRotation> angles =
  {
    RoomLayoutRotation::NONE,
    RoomLayoutRotation::CCW_90,
    RoomLayoutRotation::CCW_180,
    RoomLayoutRotation::CCW_270
  };

  int angleIndex = Util::RandomRange(0, angles.size(), _rng);

  layout = Util::RotateRoomLayout(layout, angles[angleIndex]);

  int wx = area.Width()  - 5;
  int wy = area.Height() - 5;

  int offsetX = Util::RandomRange(0, wx + 1, _rng);
  int offsetY = Util::RandomRange(0, wy + 1, _rng);

  //DebugLog("  got offsets %d %d", offsetX, offsetY);

  TryToPlaceLayout(area, layout, offsetX, offsetY);

  CheckBlockedPassages(area, layout, offsetX, offsetY);

  //DebugLog("  marking [%d %d] tile as shrine", area.X1 + offsetX, area.Y1 + offsetY);

  int shrineMiddle = (layout.size() - 1) / 2;

  int shrineMarkerX = area.X1 + offsetX + shrineMiddle;
  int shrineMarkerY = area.Y1 + offsetY + shrineMiddle;

  _map[shrineMarkerX][shrineMarkerY].ObjectHere = it->first;
}

// =============================================================================

void DGBase::PlaceStorage(const Rect& area)
{
  for (int x = area.X1; x <= area.X2; x++)
  {
    for (int y = area.Y1; y <= area.Y2; y++)
    {
      _map[x][y].ZoneMarker = TransformedRoom::STORAGE;

      if (Util::Rolld100(20))
      {
        _map[x][y].ObjectHere = GameObjectType::BREAKABLE;
      }
    }
  }
}

// =============================================================================

void DGBase::PlaceChestroom(const Rect& area)
{
  for (int x = area.X1; x <= area.X2; x++)
  {
    for (int y = area.Y1; y <= area.Y2; y++)
    {
      _map[x][y].ZoneMarker = TransformedRoom::CHESTROOM;
    }
  }

  int x = area.X1 + area.Width() / 2;
  int y = area.Y1 + area.Height() / 2;

  _map[x][y].ObjectHere = GameObjectType::CONTAINER;
}

// =============================================================================

void DGBase::CheckBlockedPassages(const Rect& area, const StringV& layout,
                                  int offsetX, int offsetY)
{
  //
  // Check if placed layout blocks anything, e.g.:
  //
  // layout
  //  ---
  // |   |
  // ##.#######
  // #...#+        <- BAD, must replace layout's '#' with '.'
  // ../..#####
  // #...#
  // ##.##
  //
  // Scan vertical and horizontal walls of layout area and check if there are
  // no doors or empty cells near them. If there are, then replace layout's
  // wall with empty cell.
  //

  int sx = area.X1 + offsetX;
  int sy = area.Y1 + offsetY;
  int ex = sx + layout.size() - 1;
  int ey = sy + layout[0].length() - 1;

  //DebugLog("  checking blocked for %d %d - %d %d", sx, sy, ex, ey);

  //
  // To prevent self-checking we must separately check
  // horizontal and vertical blocked passages when we're scanning
  // horizontal and vertical walls of the layout if placed in a room
  // which size is bigger than layout.
  //
  //  layout
  //   ---
  //  |   |
  // ........
  // .#####..
  // .#...##.
  // .#...#X#   <- point X can be considered vertical and horizontal passageway
  // .#...##.      simultaneously if checked for both conditions at once.
  // .#####..
  // ........
  //
  // |      |
  //  ------
  //  room size
  //

  auto IsPassage = [this](int x, int y, bool checkH)
  {
    bool horizontal = (_map[x][y].Image == '.' || _map[x][y].Image == '+')
                   && (_map[x - 1][y].Image == '#' && _map[x + 1][y].Image == '#');

    bool vertical = (_map[x][y].Image == '.' || _map[x][y].Image == '+')
                 && (_map[x][y + 1].Image == '#' && _map[x][y - 1].Image == '#');

    return (checkH ? horizontal : vertical);
  };

  //
  // Horizontal.
  //
  for (int x = sx; x <= ex; x++)
  {
    if (_map[x][sy].Image == '#')
    {
      if (IsPassage(x, sy - 1, true))
      {
        _map[x][sy].Image = '.';
      }
    }

    if (_map[x][ey].Image == '#')
    {
      if (IsPassage(x, ey + 1, true))
      {
        _map[x][ey].Image = '.';
      }
    }
  }

  //
  // Vertical.
  //
  for (int y = sy; y <= ey; y++)
  {
    if (_map[sx][y].Image == '#')
    {
      if (IsPassage(sx - 1, y, false))
      {
        _map[sx][y].Image = '.';
      }
    }

    if (_map[ex][y].Image == '#')
    {
      if (IsPassage(ex + 1, y, false))
      {
        _map[ex][y].Image = '.';
      }
    }
  }
}

// =============================================================================

bool DGBase::TryToPlaceRoom(int minSize, int maxSize,
                            const Rect& area,
                            size_t emptyRoomIndex,
                            TransformedRoom roomType,
                            const std::function<void(const Rect&)>& fn)
{
  bool success = true;

  if (!Util::IsFunctionValid(fn))
  {
    DebugLog("Placement function is not defined!");
    return false;
  }

  if (DoesAreaFit(area, minSize, maxSize))
  {
    fn(area);
    //DebugLog("OK");
  }
  else
  {
    auto candidates = TryToFindSuitableRooms(minSize, maxSize, emptyRoomIndex);
    if (!candidates.empty())
    {
      int index = Util::RandomRange(0, candidates.size(), _rng);
      size_t ri = candidates[index];
      //DebugLog("OK - placing at %s", _emptyRooms[ri].ToString().data());
      fn(_emptyRooms[ri]);
    }
    else
    {
      //DebugLog("requirements failed - skipping");
      success = false;
    }
  }

  return success;
}

// =============================================================================

bool DGBase::TryToPlaceRoom(const std::vector<PairII>& exactSizes,
                            const Rect& area,
                            size_t emptyRoomIndex,
                            TransformedRoom roomType,
                            const std::function<void(const Rect&)>& fn)
{
  bool success = true;

  if (!Util::IsFunctionValid(fn))
  {
    DebugLog("Placement function is not defined!");
    return false;
  }

  bool found = false;

  for (auto& item : exactSizes)
  {
    //DebugLog("Trying [%d ; %d ] for area %s ...",
    //         item.first, item.second, area.ToString().data());

    if (DoesAreaFitExactly(area, item))
    {
      found = true;
      fn(area);
      //DebugLog("OK");
      break;
    }
  }

  if (!found)
  {
    //DebugLog("Current room doesn't fit - trying to find suitable");

    auto candidates = TryToFindSuitableRooms(exactSizes, emptyRoomIndex);
    if (!candidates.empty())
    {
      int index = Util::RandomRange(0, candidates.size(), _rng);
      size_t ri = candidates[index];
      //DebugLog("OK - placing at %s", _emptyRooms[ri].ToString().data());
      fn(_emptyRooms[ri]);
    }
    else
    {
      //DebugLog("requirements failed - skipping");
      success = false;
    }
  }

  return success;
}

// =============================================================================

void DGBase::TryToPlaceLayout(const Rect& area, const StringV& layout,
                              int offsetX, int offsetY)
{
  if (layout.empty())
  {
    DebugLog("Layout is empty!");
    return;
  }

  //DebugLog("layout size = %llu, len = %llu", layout.size(), layout[0].length());

  bool doesNotFit = (area.Height() < (int)layout.size()
                  || area.Width()  < (int)layout[0].length());
  if (doesNotFit)
  {
    DebugLog("Layout doesn't fit!");
    return;
  }

  int startX = area.X1 + offsetX;
  int startY = area.Y1 + offsetY;

  int lx = startX;
  int ly = startY;

  for (auto& line : layout)
  {
    for (auto& c : line)
    {
      _map[lx][ly].Image = c;
      ly++;
    }

    ly = startY;
    lx++;
  }
}

// =============================================================================

void DGBase::MarkZone(const Rect& area, TransformedRoom zoneType)
{
  for (int x = area.X1; x <= area.X2; x++)
  {
    for (int y = area.Y1; y <= area.Y2; y++)
    {
      _map[x][y].ZoneMarker = zoneType;
    }
  }
}

// =============================================================================

void DGBase::MarkAreaEmpty(const Rect& area)
{
  MarkZone(area, TransformedRoom::EMPTY);
}

// =============================================================================

void DGBase::MarkAreaDebug(const Rect& area, char c)
{
  for (int x = area.X1; x <= area.X2; x++)
  {
    for (int y = area.Y1; y <= area.Y2; y++)
    {
      _map[x][y].Image = c;
    }
  }
}

// =============================================================================

double DGBase::GetEmptyOverWallsRatio()
{
  int empty = 0;
  int walls = 1;

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
      if (_map[x][y].AreaMarker == -1
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
  const std::unordered_map<Direction, Position> newPosByLookDir =
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
   && _map[newPos.X][newPos.Y].AreaMarker == -1)
  {
    addTo.push(newPos);
  }
}

// =============================================================================

int DGBase::MarkRegions()
{
  int marker = 0;

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

      _map[p.X][p.Y].AreaMarker = marker;

      bool alreadyExists = false;
      for (auto& point : _areaPointsByMarker[marker])
      {
        if (point == p)
        {
          alreadyExists = true;
          break;
        }
      }

      if (!alreadyExists)
      {
        _areaPointsByMarker[marker].push_back(p);
      }

      AddCellToProcess(p, Direction::WEST,  toProcess);
      AddCellToProcess(p, Direction::EAST,  toProcess);
      AddCellToProcess(p, Direction::NORTH, toProcess);
      AddCellToProcess(p, Direction::SOUTH, toProcess);
    }

    marker++;
  }

  return marker;
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
    int index = Util::RandomRange(0, connectionPoints.size(), _rng);
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

void DGBase::AddRow(const MapCell& cell)
{
  std::vector<MapCell> row;

  row.reserve(_mapSize.Y);

  for (int y = 0; y < _mapSize.Y; y++)
  {
    MapCell toAdd;
    toAdd.Coordinates = { _mapSize.X, y };
    toAdd.Image = cell.Image;

    row.push_back(toAdd);
  }

  _map.push_back(row);

  _mapSize.X++;
}

// =============================================================================

void DGBase::AddColumn(const MapCell& cell)
{
  for (int x = 0; x < _mapSize.X; x++)
  {
    MapCell toAdd;
    toAdd.Coordinates = { x, _mapSize.Y };
    toAdd.Image = cell.Image;

    _map[x].push_back(toAdd);
  }

  _mapSize.Y++;
}

// =============================================================================

void DGBase::CreateMapBorders()
{
  auto pts = Util::GetPerimeter(0, 0, _mapSize.X - 1, _mapSize.Y - 1);

  for (auto& p : pts)
  {
    _map[p.X][p.Y].Image = '#';
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
        _map[x][y].AreaMarker = -1;
      }
    }
  }
}

// =============================================================================

void DGBase::PlaceDoors(bool useAdditionalLayout)
{
  _useAdditionalLayoutForDoors = useAdditionalLayout;

  auto spotFound = FindPlaceForDoor();
  while (spotFound.size() != 0)
  {
    if (spotFound.size() != 0)
    {
      Position p = spotFound[0];
      _map[p.X][p.Y].Image = '+';
    }

    spotFound = FindPlaceForDoor();
  }
}

// =============================================================================

std::vector<Position> DGBase::FindPlaceForDoor()
{
  std::vector<Position> res;

  for (int x = 1; x < _mapSize.X - 1; x++)
  {
    for (int y = 1; y < _mapSize.Y - 1; y++)
    {
      if (IsSpotValidForDoor({ x, y }))
      {
        res.push_back({ x, y });
        return res;
      }
    }
  }

  return res;
}

// =============================================================================

bool DGBase::IsSpotValidForDoor(const Position& p)
{
  using Pattern = std::vector<std::string>;

  std::vector<Pattern> validPatterns =
  {
    {
      "...",
      "#.#",
      "#.#"
    },
    {
      "...",
      "#.#",
      "..."
    },
    {
      "#..",
      "#.#",
      "..."
    },
    {
      "..#",
      "#.#",
      "..."
    },
    {
      "#..",
      "#.#",
      "#.."
    }
  };

  if (_useAdditionalLayoutForDoors)
  {
    //
    // Tend to produce doors in L shaped corridors.
    //

    Pattern add1 =
    {
      "#.#",
      "#.#",
      "#.."
    };

    Pattern add2 =
    {
      "#.#",
      "#.#",
      "..#"
    };

    validPatterns.push_back(add1);
    validPatterns.push_back(add2);
  }

  auto angles =
  {
    RoomLayoutRotation::NONE,
    RoomLayoutRotation::CCW_90,
    RoomLayoutRotation::CCW_180,
    RoomLayoutRotation::CCW_270
  };

  int lx = p.X - 1;
  int ly = p.Y - 1;

  auto patternToCheck = ExtractMapChunk(lx, ly, 2, 2);

  for (auto& pattern : validPatterns)
  {
    for (auto& angle : angles)
    {
      auto layout = Util::RotateRoomLayout(pattern, angle);
      if (!AreChunksEqual(layout, patternToCheck))
      {
        continue;
      }
      else
      {
        return true;
      }
    }
  }

  return false;
}
