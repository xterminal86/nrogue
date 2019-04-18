#include "feature-rooms.h"

#include "rng.h"
#include "util.h"

/// Builds dungeon by attaching new rooms to existing ones
/// maxIterations should be empirically chosen, because
/// it depends on roomSizes.
/// Set to (mapSize.X * mapSize.Y) to use all available map area, but
/// be aware of computational time.
void FeatureRooms::Generate(Position mapSize,
                            Position roomSizes,
                            const FeatureRoomsWeights& weightsMap,
                            int maxIterations)
{
  _weightsMap = weightsMap;
  _mapSize = mapSize;
  _roomSizes = roomSizes;

  for (auto& kvp : _weightsMap)
  {
    _generatedSoFar[kvp.first] = 0;
    _roomWeightByType[kvp.first] = kvp.second.first;
  }

  _map = CreateFilledMap(mapSize.X, mapSize.Y);

  CreateStartingRoom();

  for (int i = 0; i < maxIterations; i++)
  {
    Position newRoomStartPos;

    auto validCells = GetValidCellsToCarveFrom();
    if (validCells.size() != 0)
    {
      int posIndex = RNG::Instance().RandomRange(0, validCells.size());

      Position doorPos = validCells[posIndex];

      // Get direction of further carving for deadend.
      //
      // i.e.
      //
      // ####
      // ..X# ->
      // ####
      //
      RoomEdgeEnum carveDir = GetCarveDirectionForDeadend(doorPos);

      Position carveOffsets = GetOffsetsForDirection(carveDir);

      // Go to next cell behind the door position in corresponding direction
      newRoomStartPos.X = doorPos.X + carveOffsets.X;
      newRoomStartPos.Y = doorPos.Y + carveOffsets.Y;

      auto res = Util::WeightedRandom(_roomWeightByType);

      FeatureRoomType typeRolled = res.first;
      std::pair<int, int> weightAndMax = _weightsMap[res.first];
      int maxAllowed = weightAndMax.second;

      if (maxAllowed > 0 && _generatedSoFar[typeRolled] >= maxAllowed)
      {
        continue;
      }

      bool success = TryToCreateRoom(doorPos, newRoomStartPos, carveDir, typeRolled);
      if (success)
      {
        _map[doorPos.X][doorPos.Y].Image = '+';
        _generatedSoFar[typeRolled]++;
      }
    }
    else
    {
      break;
    }    
  }

  FillMapRaw();
}

void FeatureRooms::CreateStartingRoom()
{
  Position newSize = GetRandomRoomSize();

  std::vector<RoomEdgeEnum> dirs =
  {
    RoomEdgeEnum::EAST,
    RoomEdgeEnum::NORTH,
    RoomEdgeEnum::SOUTH,
    RoomEdgeEnum::WEST
  };

  int index = RNG::Instance().RandomRange(0, dirs.size());
  RoomEdgeEnum d = dirs[index];

  int sx = _mapSize.X / 2;
  int sy = _mapSize.Y / 2;

  CreateEmptyRoom({ sx, sy }, newSize, d);
}

Position FeatureRooms::GetRandomRoomSize()
{
  int roomMinSize = _roomSizes.X;
  int roomMaxSize = _roomSizes.Y;

  int roomSizeX = RNG::Instance().RandomRange(roomMinSize, roomMaxSize);
  int roomSizeY = RNG::Instance().RandomRange(roomMinSize, roomMaxSize);

  return { roomSizeX, roomSizeY };
}

bool FeatureRooms::TryToCreateRoom(const Position& doorPos,
                                   const Position& newRoomStartPos,
                                   RoomEdgeEnum direction,
                                   FeatureRoomType roomType)
{
  // TODO: moar room types

  bool success = false;

  switch (roomType)
  {
    case FeatureRoomType::EMPTY:
    {
      auto rndSize = GetRandomRoomSize();
      success = CreateEmptyRoom(newRoomStartPos, rndSize, direction);
    }
    break;

    case FeatureRoomType::DIAMOND:
    {
      int size = RNG::Instance().RandomRange(2, 8);
      success = CreateDiamondRoom(newRoomStartPos, size, direction);
    }
    break;

    case FeatureRoomType::ROUND:
    {
      // Round room works well starting from radius = 4,
      // otherwise it degenerates into empty room.
      int r = RNG::Instance().RandomRange(4, 8);
      success = CreateRoundRoom(newRoomStartPos, r, direction);
    }
    break;

    case FeatureRoomType::SHRINE:
    {
      int index = RNG::Instance().RandomRange(0, GlobalConstants::ShrineNameByType.size());
      auto it = GlobalConstants::ShrineNameByType.begin();
      std::advance(it, index);

      // FIXME: finish
      //success = CreateShrine(newRoomStartPos, direction, it->first);
    }
    break;    

    default:
      success = false;
      break;
  }

  return success;
}

bool FeatureRooms::CreateRoundRoom(const Position& start,
                                   int radius,
                                   RoomEdgeEnum dir)
{
  int sx = start.X;
  int sy = start.Y;

  switch (dir)
  {
    case RoomEdgeEnum::NORTH:
      sx -= (radius - 1);
      break;

    case RoomEdgeEnum::EAST:
      sy += (radius - 1);
      break;

    case RoomEdgeEnum::SOUTH:
      sx += (radius - 1);
      break;

    case RoomEdgeEnum::WEST:
      sy -= (radius - 1);
      break;
  }

  int lx = sx - radius;
  int ly = sy - radius;
  int hx = sx + radius;
  int hy = sy + radius;

  std::vector<Position> cellsToChange;

  for (int x = lx; x <= hx; x++)
  {
    for (int y = ly; y <= hy; y++)
    {
      float d = Util::LinearDistance({ x, y }, { sx, sy });
      if (d < radius)
      {
        if (!IsCellValid({ x, y }))
        {
          return false;
        }
        else
        {
          cellsToChange.push_back({ x, y });
        }
      }
    }
  }

  for (auto& i : cellsToChange)
  {
    _map[i.X][i.Y].Image = '.';
  }

  return true;
}

bool FeatureRooms::CreateDiamondRoom(const Position& start,
                                     int size,
                                     RoomEdgeEnum dir)
{
  int mx = start.X;
  int my = start.Y;

  std::map<RoomEdgeEnum, Position> firstStep =
  {
    { RoomEdgeEnum::NORTH, { -1, 0 } },
    { RoomEdgeEnum::EAST,  { 0, 1 } },
    { RoomEdgeEnum::SOUTH, { 1, 0 } },
    { RoomEdgeEnum::WEST,  { 0, -1 } }
  };

  std::map<RoomEdgeEnum, Position> secondStep =
  {
    { RoomEdgeEnum::NORTH, { 0, -1 } },
    { RoomEdgeEnum::EAST,  { -1, 0 } },
    { RoomEdgeEnum::SOUTH, { 0, 1 } },
    { RoomEdgeEnum::WEST,  { 1, 0 } }
  };

  std::map<RoomEdgeEnum, Position> offsetsByDir =
  {
    { RoomEdgeEnum::NORTH, { -1, 1 } },
    { RoomEdgeEnum::EAST,  { 1, 1 } },
    { RoomEdgeEnum::SOUTH, { 1, -1 } },
    { RoomEdgeEnum::WEST,  { -1, -1 } }
  };

  bool isFirstStep = true;

  int iterations = size * 2 - 1;

  std::vector<Position> cellsToChange;

  for (int i = 0; i < iterations; i++)
  {
    int tx = mx;
    int ty = my;

    for (int j = 0; j < size; j++)
    {
      if (!isFirstStep && (j == size - 1))
      {
        break;
      }

      if (!IsCellValid({ tx, ty }))
      {
        return false;
      }
      else
      {
        cellsToChange.push_back({ tx, ty });
      }

      Position drawOffsets = offsetsByDir[dir];

      tx += drawOffsets.X;
      ty += drawOffsets.Y;
    }

    Position nextOffset = isFirstStep ? firstStep[dir] : secondStep[dir];

    mx += nextOffset.X;
    my += nextOffset.Y;

    isFirstStep = !isFirstStep;
  }

  for (auto& i : cellsToChange)
  {
    _map[i.X][i.Y].Image = '.';
  }

  return true;
}

std::vector<Position> FeatureRooms::GetValidCellsToCarveFrom()
{
  std::vector<Position> validCells;

  for (int x = 0; x < _mapSize.X; x++)
  {
    for (int y = 0; y < _mapSize.Y; y++)
    {
      // Because IsDeadEnd() checks neighbours, allow
      // cells only inside >= 1 < mapSize.X - 1
      if (IsInsideMap({ x, y })
       && IsDeadEnd({ x, y })
       && _map[x][y].Image == '#')
      {
        validCells.push_back({ x, y });
      }
    }
  }

  return validCells;
}

bool FeatureRooms::CreateEmptyRoom(Position start, Position size, RoomEdgeEnum dir)
{
  int sx = start.X;
  int sy = start.Y;

  int shift = 0;

  if (dir == RoomEdgeEnum::NORTH || dir == RoomEdgeEnum::SOUTH)
  {
    shift = RNG::Instance().RandomRange(0, size.X);
    sy -= shift;
  }
  else if (dir == RoomEdgeEnum::EAST || dir == RoomEdgeEnum::WEST)
  {
    shift = RNG::Instance().RandomRange(0, size.Y);
    sx -= shift;
  }

  int ex = sx;
  int ey = sy;

  switch (dir)
  {
    case RoomEdgeEnum::NORTH:
      ex -= size.Y;
      ey += size.X;
      break;

    case RoomEdgeEnum::SOUTH:
      ex += size.Y;
      ey += size.X;
      break;

    case RoomEdgeEnum::EAST:
      ex += size.Y;
      ey += size.X;
      break;

    case RoomEdgeEnum::WEST:
      ex += size.Y;
      ey -= size.X;
      break;
  }

  int minX = std::min(sx, ex);
  int minY = std::min(sy, ey);
  int maxX = std::max(sx, ex);
  int maxY = std::max(sy, ey);

  // Since corner points may be swapped,
  // for loop may go from corner to the starting point,
  // thus not including it.
  // It happens in these two cases, so we just hack it.

  if (dir == RoomEdgeEnum::NORTH)
  {
    minX++;
    maxX++;
  }
  else if (dir == RoomEdgeEnum::WEST)
  {
    minY++;
    maxY++;
  }

  std::vector<Position> cellsToChange;

  // Do not include maxX and maxY since
  // 'size' was added to 'start', making the whole thing
  // 1 unit longer.

  for (int x = minX; x < maxX; x++)
  {
    for (int y = minY; y < maxY; y++)
    {
      if (!IsCellValid({ x, y }))
      {
        return false;
      }
      else
      {
        cellsToChange.push_back({ x, y });
      }
    }
  }

  for (auto& i : cellsToChange)
  {
    _map[i.X][i.Y].Image = '.';
  }

  return true;
}

bool FeatureRooms::CreateShrine(const Position& start,
                                RoomEdgeEnum dir,
                                ShrineType type)
{
  std::vector<StringsArray2D> layouts = GlobalConstants::ShrineLayoutsByType.at(type);
  int index = RNG::Instance().RandomRange(0, layouts.size());
  auto layout = layouts[index];

  int roomSize = layout.size();

  int lx = 0;
  int ly = 0;

  int sx = start.X;
  int sy = start.Y;
  int ex = start.X;
  int ey = start.Y;

  Position d = GetOffsetsForDirection(dir);

  ex += roomSize * d.X;
  ey += roomSize * d.Y;

  for (int x = start.X; x != ex; x += d.X)
  {
    for (int y = start.Y; y != ey; y += d.Y)
    {
      _map[x][y].Image = layout[lx][ly];
      ly++;
    }

    lx++;
    ly = 0;
  }

  return true;
}

RoomEdgeEnum FeatureRooms::GetCarveDirectionForDeadend(Position pos)
{
  // A little hacky method.
  //
  // Looks in opposite direction of a wall
  // and if there's a ground tile there, returns
  // the direction this wall should be carving into.

  int x = pos.X;
  int y = pos.Y;

  int lx = pos.X - 1;
  int ly = pos.Y - 1;
  int hx = pos.X + 1;
  int hy = pos.Y + 1;

  if (_map[x][ly].Image == '.')
  {
    return RoomEdgeEnum::EAST;
  }
  else if (_map[x][hy].Image == '.')
  {
    return RoomEdgeEnum::WEST;
  }
  else if (_map[lx][y].Image == '.')
  {
    return RoomEdgeEnum::SOUTH;
  }
  else if (_map[hx][y].Image == '.')
  {
    return RoomEdgeEnum::NORTH;
  }

  return RoomEdgeEnum::NORTH;
}

Position FeatureRooms::GetOffsetsForDirection(RoomEdgeEnum dir)
{
  Position res;

  switch (dir)
  {
    case RoomEdgeEnum::EAST:
      res = { 0, 1 };
      break;

    case RoomEdgeEnum::NORTH:
      res = { -1, 0 };
      break;

    case RoomEdgeEnum::SOUTH:
      res = { 1, 0 };
      break;

    case RoomEdgeEnum::WEST:
      res = { 0, -1 };
      break;
  }

  return res;
}

// In order to forbid "eating" of walls by other rooms during generation,
// ensure that cell to be changed is surrounded by walls
// (i.e. it's not adjacent to some room space already built).
bool FeatureRooms::IsCellValid(const Position& pos)
{
  int lx = pos.X - 1;
  int ly = pos.Y - 1;
  int hx = pos.X + 1;
  int hy = pos.Y + 1;

  if (!IsInsideMap({ lx, ly }) || !IsInsideMap({ hx, hy }))
  {
    return false;
  }

  for (int x = lx; x <= hx; x++)
  {
    for (int y = ly; y <= hy; y++)
    {
      if (x == pos.X && y == pos.Y)
      {
        continue;
      }

      if (_map[x][y].Image != '#')
      {
        return false;
      }
    }
  }

  return true;
}
