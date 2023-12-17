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
///                  For example, for 'tileSizeFactor' of 1,
///                  tile size will become 2 * 1 + 1 = 3.
///                  For 2 -> 2 * 2 + 1 = 5
///                  For 3 -> 2 * 3 + 1 = 7
///                  and so on.
///
/// wallsSizeFactor - number from 1 to 'tileSizeFactor' (clamped)
///                   which determines amount of walls extension
///                   towards the center of a tile.
///                   Works from tile sizes greater than or equal to 5,
///                   so 'tileSizeFactor' must be >= 2.
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
/// postProcess - flag that requests for additional postprocessing to be done:
///               namely, removal of lone square sized blocks of walls
///               with size of 'wallsSizeFactor' by 'wallsSizeFactor'
///               which become quite numerous
///               after algorithm fininshes map generation.
///
///
/// For example: 120 240 4 3 1 produces quite decent results.
///
void BlobTiles::Generate(int mapSizeX, int mapSizeY,
                         int tileSizeFactor,
                         int wallsSizeFactor,
                         bool postProcess)
{
  _tileSizeFactor  = tileSizeFactor;
  _wallsSizeFactor = wallsSizeFactor;

  _tileSizeFactor  = Util::Clamp(_tileSizeFactor,  1, tileSizeFactor);
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
        int ind = Util::RandomRange(0, roomsToChoose.size(), _rng);
        PlaceTile(x, y, roomsToChoose[ind]);
      }
    }
  }

  if (_postProcess)
  {
    TryToRemoveLoneBlocks();
  }

  //
  // Generator works from [1;1] to map size, but since map size is a multiple
  // of _tileSize, we need additional row and column for subsequent map borders
  // after map generation finishes.
  //
  MapCell empty;
  empty.Image = '.';

  AddRow(empty);
  AddColumn(empty);

  CreateMapBorders();

  if (_postProcess)
  {
    TryToRemoveDeadEndCorridors();
  }

  ConnectIsolatedAreas();

  PlaceDoors();

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

  const std::unordered_map<Direction, P> indexOffsetsByDir =
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

void BlobTiles::TryToRemoveDeadEndCorridors()
{
  //
  // Remove dead ends leading to map edges.
  // Their size and length depends on _tileSizeFactor and _wallsSizeFactor.
  //
  int deadEndLength = _wallsSizeFactor;

  //
  // This determines only the size of subarea with empty cells.
  //
  int deadEndSize = 2 * (_tileSizeFactor - _wallsSizeFactor) + 1;

  //
  // Size of the whole biased layout with walls.
  //
  deadEndSize += 2;

  //
  // Forming layout like:
  //
  // ### -
  // #.#  |
  // #.#  | deadEndLength
  // #.#  |
  // #.# -
  //
  // | |
  //  -
  //   deadEndSize
  //
  // which we'll rotate in 4 directions and then apply its filled version
  // to all appropriate positions on the map.
  //
  StringV biasedLayout;
  biasedLayout.reserve(deadEndLength + 1);

  for (int i = 0; i <= deadEndLength; i++)
  {
    std::string line(deadEndSize, '#');

    if (i != 0)
    {
      for (int j = 1; j < deadEndSize - 1; j++)
      {
        line[j] = '.';
      }
    }

    biasedLayout.push_back(line);
  }

  std::vector<StringV> biases;
  biases.reserve(4);

  for (int angle = 0; angle <= (int)RoomLayoutRotation::CCW_270; angle++)
  {
    auto lr = Util::RotateRoomLayout(biasedLayout,
                                     (RoomLayoutRotation)angle);
    biases.push_back(lr);
  }

  //
  // Since every replacement changes map layout which in turn might introduce
  // new unexpected layouts to replace, we should adjust scanning to match
  // layout's orientation that we are searching for.
  // I.e. go from left to right (or right to left) top to bottom for 0,
  // strictly left to right and whatever vertically from top to bottom for 90,
  // but then bottom to top and whatever for 180,
  // and finally bottom to top and either way horizontally for 270.
  // Basically we should respect vertical orientation for 0 and 180 degrees
  // and horizontal for 90 and 270.
  //
  int angle = 0;
  for (auto& bias : biases)
  {
    int sx = 0;
    int sy = 0;
    int ex = _mapSize.X - bias.size();
    int ey = _mapSize.Y - bias[0].length();
    int xStep = 1;
    int yStep = 1;

    switch(angle)
    {
      case (int)RoomLayoutRotation::CCW_180:
      {
        sx = _mapSize.X - bias.size();
        ex = 0;
        xStep = -1;
      }
      break;

      case (int)RoomLayoutRotation::CCW_270:
      {
        sy = _mapSize.Y - bias[0].length();
        ey = 0;
        yStep = -1;
      }
      break;
    }

    for (int x = sx;
         (angle == (int)RoomLayoutRotation::CCW_180) ? x >= 0 : x <= ex;
         x += xStep)
    {
      for (int y = sy;
           (angle == (int)RoomLayoutRotation::CCW_270) ? y >= 0 : y <= ey;
           y += yStep)
      {
        auto& chunk = ExtractMapChunk(x, y,
                                      bias.size() - 1,
                                      bias[0].length() - 1);

        if (AreChunksEqual(chunk, bias))
        {
          FillMapChunk(x, y,
                       bias.size() - 1,
                       bias[0].length() - 1,
                       '#');
        }
      }
    }

    angle++;
  }
}

// =============================================================================

void BlobTiles::TryToRemoveLoneBlocks()
{
  //
  // This generator tends to produce lone blocks of the following pattern:
  //
  // _wallsSizeFactor = 1
  //
  // .... -
  // .##.  |
  // .##.  | blockArea
  // .... -
  //
  // |  |
  //  --
  //   blockArea
  //
  // size of which depends on _wallsSizeFactor.
  //
  // To remove it, we first construct this pattern in question and then just go
  // over the whole map with sliding window and replace it with empty space
  // if found.
  //
  int blockArea = (_wallsSizeFactor + 1) * 2;

  StringV blockToSearch;
  blockToSearch.reserve(blockArea);

  //
  // First just fill the whole pattern with empty space.
  //
  for (int i = 0; i < blockArea; i++)
  {
    std::string line(blockArea, '.');
    blockToSearch.push_back(line);
  }

  //
  // Then fill the inner subarea with walls.
  //
  for (int x = 1; x < blockArea - 1; x++)
  {
    for (int y = 1; y < blockArea - 1; y++)
    {
      blockToSearch[x][y] = '#';
    }
  }

  //
  // Find and replace.
  //
  for (int x = 0; x < _mapSize.X; x++)
  {
    for (int y = 0; y < _mapSize.Y; y++)
    {
      auto chunk = ExtractMapChunk(x, y, blockArea - 1, blockArea - 1);

      //
      // Empty chunk is returned when we go out of bounds.
      //
      if (chunk.empty())
      {
        continue;
      }

      if (AreChunksEqual(blockToSearch, chunk))
      {
        FillMapChunk(x, y, blockArea - 1, blockArea - 1, '.');
      }
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
