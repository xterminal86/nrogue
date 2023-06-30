#include "blob-tiles.h"

#include "rng.h"
#include "util.h"

#ifdef MSVC_COMPILER
#include <sstream>
#endif

///
/// \brief Generate dungeon from 47 tile subset of 2-edge 2-corner Wang tileset.
///
/// http://cr31.co.uk/stagecast/wang/blob.html
///
/// AKA "Blob Tiles".
///
/// tileSizeFactor - number from 1 to N which determines square tile size.
///                  For example, for tileSizeFactor of 1,
///                  tile size will become 2 * 1 + 1 = 3.
///                  For 2 -> 2 * 2 + 1 = 5
///                  For 3 -> 2 * 3 + 1 = 7
///                  and so on.
///
/// wallsSizeFactor - number from 1 to tileSizeFactor (clamped) which determines
///                   amount of walls extension towards the center of a tile.
///                   Works from tile sizes greater than or equal to 5,
///                   so tileSizeFactor must be >= 2.
///                   For example, consider tile with tileSizeFactor = 2:
///
///                   #...#
///                   #...#
///                   #...#
///                   #...#
///                   #####
///
///                   This is wallsSizeFactor = 1.
///                   If we make it 2, tile will look like this:
///
///                   ##.##
///                   ##.##
///                   ##.##
///                   #####
///                   #####
///
/// postProcess - flag (on by default) that requests for additional
///               removal of lone square sized blocks of walls
///               with size wallsSizeFactor by wallsSizeFactor
///               which become quite numerous
///               after algorithm fininshes map generation.
///
void BlobTiles::Generate(int mapSizeX, int mapSizeY,
                         int tileSizeFactor,
                         int wallsSizeFactor,
                         bool postProcess)
{
  _tileSizeFactor = tileSizeFactor;
  _wallsSizeFactor = wallsSizeFactor;

  _tileSizeFactor  = Util::Clamp(_tileSizeFactor, 1, tileSizeFactor);
  _wallsSizeFactor = Util::Clamp(_wallsSizeFactor, 1, _tileSizeFactor);

  //
  // 3x3, 5x5, 7x7, ...
  //
  _tileSize = 2 * _tileSizeFactor + 1;

  int mxAdjusted = mapSizeX - ((mapSizeX + _tileSize) % _tileSize) + 1;
  int myAdjusted = mapSizeY - ((mapSizeY + _tileSize) % _tileSize) + 1;

  _mapSize = { mxAdjusted, myAdjusted };

  _postProcess = postProcess;

  PrepareBaseTiles();

  _tilesetToUse.resize(_tilesetBaseAdjusted.size());

  //
  // Will be hardcoding appropriate rotations for specific tiles.
  //
  for (size_t i = 0; i < _tilesetBaseAdjusted.size(); i++)
  {
    //
    // Total symmetry - every rotation changes nothing.
    //
    if (i == 0 || i == 9 || i == 14)
    {
      _tilesetToUse[i].push_back(_tilesetBaseAdjusted[i]);
    }
    //
    // Mirror image - only one rotation makes a difference.
    //
    else if (i == 4 || i == 12)
    {
      auto rl = Util::RotateRoomLayout(_tilesetBaseAdjusted[i],
                                       RoomLayoutRotation::CCW_90);

      _tilesetToUse[i].push_back(_tilesetBaseAdjusted[i]);
      _tilesetToUse[i].push_back(rl);
    }
    //
    // All four rotations needed.
    //
    else
    {
      for (int rotationType = 0;
           rotationType <= (int)RoomLayoutRotation::CCW_270;
           rotationType++)
      {
        auto rl = Util::RotateRoomLayout(_tilesetBaseAdjusted[i],
                                         (RoomLayoutRotation)rotationType);
        _tilesetToUse[i].push_back(rl);
      }
    }
  }

  _map = CreateFilledMap(_mapSize.X, _mapSize.Y, '.');

  std::vector<StringV> roomsToChoose;

  for (int x = 1; x < _mapSize.X; x += _tileSize)
  {
    for (int y = 1; y < _mapSize.Y; y += _tileSize)
    {
      roomsToChoose.clear();

      for (auto& items : _tilesetToUse)
      {
        for (auto& tile : items)
        {
          bool leftOk = CheckEdge(x, y, RoomEdgeEnum::WEST, tile);
          bool upOk   = CheckEdge(x, y, RoomEdgeEnum::NORTH, tile);
          if (leftOk && upOk)
          {
            roomsToChoose.push_back(tile);
          }
        }
      }

      if (!roomsToChoose.empty())
      {
        int ind = RNG::Instance().RandomRange(0, roomsToChoose.size());
        PlaceTile(x, y, roomsToChoose[ind]);
      }
    }
  }

  if (_postProcess)
  {
    PostProcess();
  }

  //
  // Generator works from 1,1 to the map size, but since map size is a multiple
  // of _tileSize, we need additional row and column for subsequent map borders
  // after map generation finishes.
  //
  MapCell empty;
  empty.Image = '.';

  AddRow(empty);
  AddColumn(empty);

  CreateMapBorders();
  ConnectIsolatedAreas();

  FillMapRaw();
}

// =============================================================================

void BlobTiles::PrepareBaseTiles()
{
  _tilesetBaseAdjusted.reserve(_tilesetBase.size());

  for (auto& originalTile : _tilesetBase)
  {
    StringV newTile = originalTile;

    if (_tileSizeFactor > 1)
    {
      EnlargeTile(newTile);

      if (_wallsSizeFactor > 1)
      {
        ExtendWalls(newTile);
      }
    }

    _tilesetBaseAdjusted.push_back(newTile);
  }
}

// =============================================================================

void BlobTiles::EnlargeTile(StringV& tile)
{
  int center = (_tilesetBase[0][0].length() - 1) / 2;

  int insertTimes = 2 * (_tileSizeFactor - 1);

  //
  // Extract horizontal middle line of original tile
  // and insert it insertTimes times in the middle of original tile layout.
  //
  std::string middleLineH;
  middleLineH.reserve(tile[0].length());

  for (size_t i = 0; i < tile[0].length(); i++)
  {
    middleLineH.append(1, tile[center][i]);
  }

  for (int i = 0; i < insertTimes; i++)
  {
    tile.insert(tile.begin() + center, middleLineH);
  }

  //
  // Do the same with vertical middle line accordingly, thus resulting
  // in new tile NxN, where N = 2 * _tileSizeFactor - 1;
  //
  std::string middleLineV;
  middleLineV.reserve(tile.size());

  for (size_t i = 0; i < tile.size(); i++)
  {
    middleLineV.append(1, tile[i][center]);
  }

  size_t ind = 0;

  for (auto& line : tile)
  {
    for (int i = 0; i < insertTimes; i++)
    {
      line.insert(line.begin() + center, middleLineV[ind]);
    }

    ind++;
  }
}

// =============================================================================

void BlobTiles::ExtendWalls(StringV& tile)
{
  using P = std::pair<int, int>;

  int n = tile[0].length();

  const std::map<Direction, P> indexOffsetsByDir =
  {
    { Direction::EAST,  {  0,  1 } },
    { Direction::WEST,  {  0, -1 } },
    { Direction::NORTH, {  1,  0 } },
    { Direction::SOUTH, { -1,  0 } }
  };

  //
  // Since this particular tileset is edge-based,
  // we can loop through all of them and extend all '#' cells
  // by _wallsSizeFactor.
  //
  auto ExtendEdge = [this, &tile, n, &indexOffsetsByDir](Direction dir)
  {
    int ix = 0;
    int iy = 0;

    for (int i = 0; i < n; i++)
    {
      char* img = nullptr;

      switch(dir)
      {
        case Direction::NORTH:
        {
          img = &tile[0][i];
          ix = 0;
          iy = i;
        }
        break;

        case Direction::SOUTH:
        {
          img = &tile[n - 1][i];
          ix = n - 1;
          iy = i;
        }
        break;

        case Direction::EAST:
        {
          img = &tile[i][0];
          ix = i;
          iy = 0;
        }
        break;

        case Direction::WEST:
        {
          img = &tile[i][n - 1];
          ix = i;
          iy = n - 1;
        }
        break;
      }

      //
      // Expand dong.
      //
      if (*img == '#')
      {
        for (int j = 0; j < _wallsSizeFactor - 1; j++)
        {
          const P& offsets = indexOffsetsByDir.at(dir);

          ix += offsets.first;
          iy += offsets.second;

          char& nextImg = tile[ix][iy];
          if (nextImg == '#')
          {
            break;
          }

          nextImg = '#';
        }
      }
    }
  };

  ExtendEdge(Direction::NORTH);
  ExtendEdge(Direction::SOUTH);
  ExtendEdge(Direction::EAST);
  ExtendEdge(Direction::WEST);
}

// =============================================================================

void BlobTiles::PlaceTile(int x, int y, const StringV& tile)
{
  for (int i = 0; i < _tileSize; i++)
  {
    for (int j = 0; j < _tileSize; j++)
    {
      _map[x + i][y + j].Image = tile[i][j];
    }
  }
}

// =============================================================================

bool BlobTiles::CheckEdge(int x, int y,
                          RoomEdgeEnum edge,
                          const StringV& tileToCheck)
{
  int lx = x - _tileSize;
  int ly = y - _tileSize;

  //
  // Out of bounds left or up is considered valid tile to attach to.
  //
  if ((edge == RoomEdgeEnum::WEST  && ly < 1)
   || (edge == RoomEdgeEnum::NORTH && lx < 1))
  {
    return true;
  }

  switch (edge)
  {
    case RoomEdgeEnum::WEST:
    {
      StringV lw = GetMapChunkAround(x, ly);

      for (int i = 0; i < _tileSize; i++)
      {
        //
        // Check rightmost edge of left tile with leftmost edge of current tile.
        //
        if (lw[i][_tileSize - 1] != tileToCheck[i][0])
        {
          return false;
        }
      }
    }
    break;

    case RoomEdgeEnum::NORTH:
    {
      StringV ln = GetMapChunkAround(lx, y);

      for (int i = 0; i < _tileSize; i++)
      {
        //
        // Check bottom edge of up tile with top edge of current tile.
        //
        if (ln[_tileSize - 1][i] != tileToCheck[0][i])
        {
          return false;
        }
      }
    }
    break;
  }

  return true;
}

// =============================================================================

StringV BlobTiles::GetMapChunkAround(int x, int y)
{
  StringV chunk;

  for (int i = x; i < x + _tileSize; i++)
  {
    std::string line;
    for (int j = y; j < y + _tileSize; j++)
    {
      line += _map[i][j].Image;
    }

    chunk.push_back(line);
  }

  return chunk;
}

// =============================================================================

void BlobTiles::PostProcess()
{
  //
  // Determined experimentally.
  //
  int biasSize = _wallsSizeFactor * 2;

  for (int x = 0; x < _mapSize.X; x++)
  {
    for (int y = 0; y < _mapSize.Y; y++)
    {
      TryToRemoveBias(x, y, biasSize);
    }
  }
}

// =============================================================================

void BlobTiles::TryToRemoveBias(int x, int y, int biasSize)
{
  int lx = x - 1;
  int ly = y - 1;
  int hx = lx + biasSize + 1;
  int hy = ly + biasSize + 1;

  bool outOfBounds = (lx < 0 || ly < 0
                    || hx > _mapSize.X - 1 || hy > _mapSize.Y - 1);

  if (outOfBounds)
  {
    return;
  }

  //
  // Try to remove blocks of walls _wallsSizeFactor by _wallsSizeFactor size.
  //
  // For that we first scan the area in question, determine that it's all walls
  // and then make sure that it's surrounded by empty cells.
  // Only then we can remove it.
  //
  auto points = Util::GetPerimeter(lx, ly, biasSize + 1, biasSize + 1);

  //
  // If points around area in question are not empty cells, forget it.
  //
  for (auto& p : points)
  {
    if (_map[p.X][p.Y].Image != '.')
    {
      return;
    }
  }

  //
  // Now all that's left is to check if area in question is all walls...
  //
  for (int i = x; i < x + biasSize; i++)
  {
    for (int j = y; j < y + biasSize; j++)
    {
      if (_map[i][j].Image != '#')
      {
        return;
      }
    }
  }

  //
  // ...and replace it with empty blocks.
  //
  for (int i = x; i < x + biasSize; i++)
  {
    for (int j = y; j < y + biasSize; j++)
    {
      _map[i][j].Image = '.';
    }
  }
}

// =============================================================================

void BlobTiles::ForCustomDebugStuff()
{
  PrintTiles();
}

// =============================================================================

void BlobTiles::PrintTiles()
{
  const std::string decor(80, '-');

  std::stringstream ss;

  //
  // Must be exactly 47 in the end.
  //
  int index = 1;

  for (auto& items : _tilesetToUse)
  {
    for (auto& tile : items)
    {
      ss << decor << "\n";
      ss << index << "\n";
      ss << decor << "\n";

      for (auto& line : tile)
      {
        ss << line << "\n";
      }

      index++;
    }
  }

  printf("%s\n", ss.str().data());
}

// =============================================================================

#ifdef DEBUG_BUILD
void BlobTiles::PrintMap(int curX, int curY)
{
  std::stringstream ss;

  ss << curX << " ; " << curY << "\n\n";

  int out = 0;

  for (int i = 0; i < _mapSize.X; i++)
  {
    out = i % 10;
    ss << std::to_string(out);
  }

  ss << "\n";

  for (int x = 0; x < _mapSize.X; x++)
  {
    out = x % 10;

    std::string line;
    for (int y = 0; y < _mapSize.Y; y++)
    {
      line += _map[x][y].Image;
    }

    line += std::to_string(out);

    ss << line << "\n";
  }

  printf("%s\n\n", ss.str().data());
}

void BlobTiles::PrintTile(const StringV& tile)
{
  std::stringstream ss;

  for (auto& line : tile)
  {
    for (auto& c : line)
    {
      ss << c;
    }

    ss << "\n";
  }

  printf("\n%s\n", ss.str().data());
}
#endif
