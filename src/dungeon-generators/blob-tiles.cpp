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
/// So called "Blob tiles".
///
void BlobTiles::Generate(int mapSizeX, int mapSizeY)
{
  int mxAdjusted = mapSizeX - ((mapSizeX + _tileSize) % _tileSize) + 1;
  int myAdjusted = mapSizeY - ((mapSizeY + _tileSize) % _tileSize) + 1;

  _mapSize = { mxAdjusted, myAdjusted };

  _tilesetToUse.resize(_tilesetBase.size());

  //
  // Will be hardcoding appropriate rotations for specific tiles.
  //
  for (size_t i = 0; i < _tilesetBase.size(); i++)
  {
    //
    // Total symmetry - every rotation changes nothing.
    //
    if (i == 0 || i == 9 || i == 14)
    {
      _tilesetToUse[i].push_back(_tilesetBase[i]);
    }
    //
    // Mirror image - only one rotation makes a difference.
    //
    else if (i == 4 || i == 12)
    {
      auto rl = Util::RotateRoomLayout(_tilesetBase[i],
                                       RoomLayoutRotation::CCW_90);

      _tilesetToUse[i].push_back(_tilesetBase[i]);
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
        auto rl = Util::RotateRoomLayout(_tilesetBase[i],
                                         (RoomLayoutRotation)rotationType);
        _tilesetToUse[i].push_back(rl);
      }
    }
  }

  _map = CreateFilledMap(_mapSize.X, _mapSize.Y, '.');

  std::vector<StringsArray2D> roomsToChoose;

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

  MapCell empty;
  empty.Image = '.';

  //
  // Generator works from 1,1 to the map size, but since map size is a multiple
  // of _tileSize, we need additional row and column for subsequent map borders
  // after map generation finishes.
  //
  AddRow(empty);
  AddColumn(empty);

  FillMapRaw();
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

void BlobTiles::PrintTile(const StringsArray2D& tile)
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

// =============================================================================

void BlobTiles::PlaceTile(int x, int y, const StringsArray2D& tile)
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
                          const StringsArray2D& tileToCheck)
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
      StringsArray2D lw = GetMapChunkAround(x, ly);

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
      StringsArray2D ln = GetMapChunkAround(lx, y);

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

StringsArray2D BlobTiles::GetMapChunkAround(int x, int y)
{
  StringsArray2D chunk;

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
  // Must be 47 in the end.
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
