#include "from-tiles.h"
#include "util.h"

void FromTiles::Generate(const Position& mapSize,
                         int tileSetIndex,
                         bool postProcess,
                         bool removeBias)
{
  _mapSize = mapSize;

  // 3x3
  _tileArea = 9;
  _tileEdgeLength = 3;

  CreateTileset();

  _map = CreateFilledMap(mapSize.X, mapSize.Y, ' ');

  int tsIndex = 0;
  if (tileSetIndex == -1)
  {
    tsIndex = RNG::Instance().RandomRange(0, _tileset.size());

  }
  else
  {
    tsIndex = Util::Clamp(tileSetIndex, 0, _tileset.size() - 1);
  }

  // Start from center to have all 4 directions to develop
  int cx = (mapSize.X - 1) / 2;
  int cy = (mapSize.Y - 1) / 2;

  _currentTileset = _tileset[tsIndex];

  size_t index = RNG::Instance().RandomRange(0, _currentTileset.size());
  Tile t = _currentTileset[index];
  PlaceLayout({ cx, cy }, t);
  _visited.push_back({ cx, cy });
  AddPointsToProcess({ cx, cy });

  while (!_toProcess.empty())
  {
    auto p = _toProcess.top();
    _toProcess.pop();
    TryToPlaceLayout(p);
  }

  FillBorders();

  if (postProcess)
  {
    FillSingleCells();
  }

  ConnectIsolatedAreas();
  CutProblemCorners();

  if (postProcess)
  {
    RemoveSingleWalls();
  }

  // Remove horizontal corridor at the top
  // by filling borders of the map inwards.
  if (removeBias)
  {
    // Unmark regions
    for (int x = 0; x < mapSize.X; x++)
    {
      for (int y = 0; y < mapSize.Y; y++)
      {
        if (_map[x][y].Image == '.')
        {
          _map[x][y].Marker = -1;
        }
      }
    }

    for (int i = 0; i < 2; i++)
    {
      for (int x = 1 + i; x < mapSize.X - (i + 2); x++)
      {
        _map[x][1 + i].Image = '#';
        _map[x][mapSize.Y - (i + 2)].Image = '#';
      }

      for (int y = 1 + i; y < mapSize.Y - (i + 2); y++)
      {
        _map[i + 1][y].Image = '#';
        _map[mapSize.X - (i + 2)][y].Image = '#';
      }
    }

    FillSingleCells();
    ConnectIsolatedAreas();
    CutProblemCorners();
    RemoveSingleWalls();
  }

  FillMapRaw();
}

Tile FromTiles::ConvertStringToTile(const std::string& line)
{
  Tile res;

  std::string row;

  int column = 0;
  for (auto& c : line)
  {
    row += c;

    column++;

    if (column == _tileEdgeLength)
    {
      column = 0;
      res.push_back(row);
      row.clear();
    }
  }

  return res;
}

void FromTiles::CreateTileset()
{
  for (int i = 1; i < _tileArea; i++)
  {
    std::string line(_tileArea, '.');

    for (int j = 0; j < i; j++)
    {
      line[j] = '#';
    }

    Tiles tiles;

    do
    {
      Tile t = ConvertStringToTile(line);
      tiles.push_back(t);
    }
    while(std::next_permutation(line.begin(), line.end()));

    _tileset.push_back(tiles);
  }
}

void FromTiles::FillBorders()
{
  for (auto& line : _map)
  {
    for (MapCell& c : line)
    {
      if (c.Image == ' ')
      {
        c.Image = '#';
      }
    }
  }
}

void FromTiles::TryToPlaceLayout(const Position& p)
{
  if (!IsInsideMap(p) || WasVisited(p))
  {
    return;
  }

  std::map<RoomEdgeEnum, Position> points;

  Position u = { p.X - _tileEdgeLength, p.Y                   };
  Position d = { p.X + _tileEdgeLength, p.Y                   };
  Position l = { p.X,                   p.Y - _tileEdgeLength };
  Position r = { p.X,                   p.Y + _tileEdgeLength };

  points[RoomEdgeEnum::NORTH] = u;
  points[RoomEdgeEnum::SOUTH] = d;
  points[RoomEdgeEnum::EAST]  = r;
  points[RoomEdgeEnum::WEST]  = l;

  Tiles validLayouts;

  for (Tile& layout : _currentTileset)
  {
    bool isOk = true;

    for (auto& kvp : points)
    {
      if (!IsInsideMap(kvp.second))
      {
        continue;
      }

      auto l = ReadLayout(kvp.second);
      if (!IsLayoutEmpty(l))
      {
        if (!CanBeConnected(layout, l, kvp.first))
        {
          isOk = false;
          break;
        }
      }
    }

    if (isOk)
    {
      validLayouts.push_back(layout);
    }
  }

  if (!validLayouts.empty())
  {
    int index = RNG::Instance().RandomRange(0, validLayouts.size());
    PlaceLayout(p, validLayouts[index]);
    AddPointsToProcess(p);
    _visited.push_back(p);
  }
}

bool FromTiles::CanBeConnected(const Tile& l1,
                               const Tile& l2,
                               const RoomEdgeEnum& along)
{
  std::map<RoomEdgeEnum, RoomEdgeEnum> oppositeDirByDir =
  {
    { RoomEdgeEnum::NORTH, RoomEdgeEnum::SOUTH },
    { RoomEdgeEnum::EAST,  RoomEdgeEnum::WEST  },
    { RoomEdgeEnum::SOUTH, RoomEdgeEnum::NORTH },
    { RoomEdgeEnum::WEST,  RoomEdgeEnum::EAST  },
  };

  if (l1.empty() || l1.empty())
  {
    return false;
  }

  std::string s1 = GetLayoutEdge(l1, along);
  std::string s2 = GetLayoutEdge(l2, oppositeDirByDir[along]);

  for (size_t i = 0; i < s1.length(); i++)
  {
    if (s1[i] == '.' && s2[i] == '.')
    {
      return true;
    }
  }

  return false;
}

std::string FromTiles::GetLayoutEdge(const Tile& l, const RoomEdgeEnum& along)
{
  std::string edge;

  switch (along)
  {
    case RoomEdgeEnum::NORTH:
    {
      for (int y = 0; y < _tileEdgeLength; y++)
      {
        edge += l[0][y];
      }
    }
    break;

    case RoomEdgeEnum::EAST:
    {
      for (int x = 0; x < _tileEdgeLength; x++)
      {
        edge += l[x][_tileEdgeLength - 1];
      }
    }
    break;

    case RoomEdgeEnum::SOUTH:
    {
      for (int y = 0; y < _tileEdgeLength; y++)
      {
        edge += l[_tileEdgeLength - 1][y];
      }
    }
    break;

    case RoomEdgeEnum::WEST:
    {
      for (int x = 0; x < _tileEdgeLength; x++)
      {
        edge += l[x][0];
      }
    }
    break;
  }

  return edge;
}

Tile FromTiles::ReadLayout(const Position& p)
{
  Tile res;

  for (int x = 0; x < _tileEdgeLength; x++)
  {
    std::string row;
    for (int y = 0; y < _tileEdgeLength; y++)
    {
      row += '.';
    }

    res.push_back(row);
  }

  int lx = p.X - 1;
  int ly = p.Y - 1;
  int hx = p.X + 1;
  int hy = p.Y + 1;

  int rx = 0;
  int ry = 0;

  for (int x = lx; x <= hx; x++)
  {
    ry = 0;

    for (int y = ly; y <= hy; y++)
    {
      if (IsInsideMap({ x, y }))
      {
        res[rx][ry] = _map[x][y].Image;
      }
      else
      {
        res.clear();
        return res;
      }

      ry++;
    }

    rx++;
  }

  return res;
}

void FromTiles::PlaceLayout(const Position& pos, const Tile& tile)
{
  int lx = pos.X - 1;
  int ly = pos.Y - 1;
  int hx = pos.X + 1;
  int hy = pos.Y + 1;

  int rx = 0;
  int ry = 0;

  for (int x = lx; x <= hx; x++)
  {
    ry = 0;

    for (int y = ly; y <= hy; y++)
    {
      if (IsInsideMap({ x, y }))
      {
        _map[x][y].Image = tile.at(rx).at(ry);
      }

      ry++;
    }

    rx++;
  }
}

void FromTiles::AddPointsToProcess(const Position& p)
{
  std::vector<Position> positions =
  {
    { p.X - _tileEdgeLength, p.Y                   },
    { p.X + _tileEdgeLength, p.Y                   },
    { p.X,                   p.Y - _tileEdgeLength },
    { p.X,                   p.Y + _tileEdgeLength }
  };

  for (auto& item : positions)
  {
    _toProcess.push(item);
  }
}

bool FromTiles::IsLayoutEmpty(const Tile& layout)
{
  for (auto& line : layout)
  {
    for (auto& c : line)
    {
      if (c != ' ')
      {
        return false;
      }
    }
  }

  return true;
}

bool FromTiles::IsLayoutPresent(const Tile& layout)
{
  /*
  for (auto& l : _tileset)
  {
    if (AreLayoutsEqual(l, layout))
    {
      return true;
    }
  }
  */

  return false;
}

bool FromTiles::AreLayoutsEqual(const Tile& l1, const Tile& l2)
{
  for (size_t i = 0; i < l1.size(); i++)
  {
    if (l1[i] != l2[i])
    {
      return false;
    }
  }

  return true;
}

bool FromTiles::WasVisited(const Position& p)
{
  for (auto& p2 : _visited)
  {
    if (p.X == p2.X && p.Y == p2.Y)
    {
      return true;
    }
  }

  return false;
}
