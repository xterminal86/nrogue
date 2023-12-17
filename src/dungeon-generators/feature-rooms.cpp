#include "feature-rooms.h"

#include "rng.h"
#include "util.h"

///
/// \brief Builds dungeon by attaching new rooms to existing ones.
///
/// 'doorPlacementChance' is a percentage of a chance
/// to place door between rooms.
/// 'maxIterations' should be empirically chosen, because
/// it depends on roomSizes.
/// Generally set 'maxIterations' to (mapSize.X * mapSize.Y)
/// to maximize the effect, but be aware of computational time.
///
void FeatureRooms::Generate(const Position& mapSize,
                            const Position& roomSizes,
                            const FeatureRoomsWeights& weightsMap,
                            uint8_t doorPlacementChance,
                            int maxIterations)
{
  _weightsMap = weightsMap;
  _mapSize    = mapSize;
  _roomSizes  = roomSizes;

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
    if (validCells.size() == 0)
    {
      break;
    }

    int posIndex = Util::RandomRange(0, validCells.size(), _rng);

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

    //
    // Go to next cell behind the door position in corresponding direction.
    //
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
      uint8_t doorChance = Util::Clamp(doorPlacementChance, 0, 100);

      uint8_t chanceRolled = Util::RandomRange(1, 101, _rng);

      _map[doorPos.X][doorPos.Y].Image = (chanceRolled <= doorChance) ? '+' : '.';
      _generatedSoFar[typeRolled]++;
    }
  }

  FillMapRaw();
}

// =============================================================================

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

  int index = Util::RandomRange(0, dirs.size(), _rng);
  RoomEdgeEnum d = dirs[index];

  int sx = _mapSize.X / 2;
  int sy = _mapSize.Y / 2;

  CreateEmptyRoom({ sx, sy }, newSize, d);
}

// =============================================================================

Position FeatureRooms::GetRandomRoomSize()
{
  int roomMinSize = _roomSizes.X;
  int roomMaxSize = _roomSizes.Y;

  int roomSizeX = Util::RandomRange(roomMinSize, roomMaxSize, _rng);
  int roomSizeY = Util::RandomRange(roomMinSize, roomMaxSize, _rng);

  return { roomSizeX, roomSizeY };
}

// =============================================================================

bool FeatureRooms::TryToCreateRoom(const Position& doorPos,
                                   const Position& newRoomStartPos,
                                   RoomEdgeEnum direction,
                                   FeatureRoomType roomType)
{
  // TODO: moar room types?

  bool success = false;

  switch (roomType)
  {
    case FeatureRoomType::EMPTY:
    {
      auto rndSize = GetRandomRoomSize();
      success = CreateEmptyRoom(newRoomStartPos, rndSize, direction);
    }
    break;

    case FeatureRoomType::FLOODED:
    {
      auto rndSize = GetRandomRoomSize();
      int choice = Util::RandomRange(0, 2, _rng);
      char variant = (choice == 0) ? 'W' : 'l';
      success = CreateEmptyRoom(newRoomStartPos, rndSize, direction, variant);
    }
    break;

    case FeatureRoomType::DIAMOND:
    {
      int size = Util::RandomRange(2, 8, _rng);
      success = CreateDiamondRoom(newRoomStartPos, size, direction);
    }
    break;

    case FeatureRoomType::ROUND:
    {
      // Room becomes round only if radius >= 4,
      // otherwise it degenerates into empty room.
      int r = Util::RandomRange(4, 8, _rng);
      success = CreateRoundRoom(newRoomStartPos, r, direction);
    }
    break;

    case FeatureRoomType::SHRINE:
    {
      int index = Util::RandomRange(0,
                                    GlobalConstants::ShrineNameByType.size(),
                                    _rng);
      auto it = GlobalConstants::ShrineNameByType.begin();
      std::advance(it, index);

      success = CreateShrine(newRoomStartPos, direction, it->first);
    }
    break;

    case FeatureRoomType::POND:
    case FeatureRoomType::GARDEN:
    case FeatureRoomType::FOUNTAIN:
    {
      auto layoutVariant = _specialRoomLayoutByType.at(roomType);
      int layoutSubvariant = Util::RandomRange(0, 2, _rng);
      int index = Util::RandomRange(0, layoutVariant.size(), _rng);
      auto layout = layoutVariant[index];
      success = PlaceLayout(newRoomStartPos, layout, direction, (layoutSubvariant == 0));
    }
    break;

    case FeatureRoomType::PILLARS:
    {
      int index = Util::RandomRange(0,
                                    GlobalConstants::PillarsLayouts.size(),
                                    _rng);
      auto layout = GlobalConstants::PillarsLayouts[index];
      success = PlaceLayout(newRoomStartPos, layout, direction);
    }
    break;

    default:
      success = false;
      break;
  }

  return success;
}

// =============================================================================

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
      double d = Util::LinearDistance({ x, y }, { sx, sy });
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

// =============================================================================

bool FeatureRooms::CreateDiamondRoom(const Position& start,
                                     int size,
                                     RoomEdgeEnum dir)
{
  int mx = start.X;
  int my = start.Y;

  std::unordered_map<RoomEdgeEnum, Position> firstStep =
  {
    { RoomEdgeEnum::NORTH, { -1,  0 } },
    { RoomEdgeEnum::EAST,  {  0,  1 } },
    { RoomEdgeEnum::SOUTH, {  1,  0 } },
    { RoomEdgeEnum::WEST,  {  0, -1 } }
  };

  std::unordered_map<RoomEdgeEnum, Position> secondStep =
  {
    { RoomEdgeEnum::NORTH, {  0, -1 } },
    { RoomEdgeEnum::EAST,  { -1,  0 } },
    { RoomEdgeEnum::SOUTH, {  0,  1 } },
    { RoomEdgeEnum::WEST,  {  1,  0 } }
  };

  std::unordered_map<RoomEdgeEnum, Position> offsetsByDir =
  {
    { RoomEdgeEnum::NORTH, { -1,  1 } },
    { RoomEdgeEnum::EAST,  {  1,  1 } },
    { RoomEdgeEnum::SOUTH, {  1, -1 } },
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

// =============================================================================

std::vector<Position> FeatureRooms::GetValidCellsToCarveFrom()
{
  std::vector<Position> validCells;

  for (int x = 0; x < _mapSize.X; x++)
  {
    for (int y = 0; y < _mapSize.Y; y++)
    {
      //
      // Because IsDeadEnd() checks neighbours, allow
      // cells only inside >= 1 < mapSize.X - 1
      //
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

// =============================================================================

bool FeatureRooms::CreateEmptyRoom(const Position& start,
                                   const Position& size,
                                   RoomEdgeEnum dir,
                                   char ground)
{
  int shiftX = Util::RandomRange(0, size.X, _rng);
  int shiftY = Util::RandomRange(0, size.Y, _rng);

  int heightOffsetToStart = size.X - 1;
  int widthOffsetToStart = size.Y - 1;

  int sx = start.X;
  int sy = start.Y;
  int ex = sx;
  int ey = sy;

  switch (dir)
  {
    case RoomEdgeEnum::NORTH:
    {
      sx -= heightOffsetToStart;
      sy -= shiftY;
    }
    break;

    case RoomEdgeEnum::EAST:
    {
      sx -= shiftX;
    }
    break;

    case RoomEdgeEnum::SOUTH:
    {
      sy -= shiftY;
    }
    break;

    case RoomEdgeEnum::WEST:
    {
      sx -= shiftX;
      sy -= widthOffsetToStart;
    }
    break;
  }

  ex = sx + size.X;
  ey = sy + size.Y;

  if (!IsAreaValid({ sx, sy }, { ex, ey }))
  {
    return false;
  }

  for (int x = sx; x < ex; x++)
  {
    for (int y = sy; y < ey; y++)
    {
      _map[x][y].Image = ground;
    }
  }

  return true;
}

// =============================================================================

bool FeatureRooms::CreateShrine(const Position& start,
                                RoomEdgeEnum dir,
                                ShrineType type)
{
  std::vector<StringV> layouts = GlobalConstants::ShrineLayoutsByType.at(type);
  int index = Util::RandomRange(0, layouts.size(), _rng);
  auto layout = layouts[index];

  //
  // layout.size() is 5
  // Center room entrance along the dir.
  //
  auto res = CenterRoomAlongDir(start, layout.size(), dir);

  int sx = res.first.X;
  int ex = res.first.Y;
  int sy = res.second.X;
  int ey = res.second.Y;

  if (!IsAreaValid({ sx, sy }, { ex, ey }))
  {
    return false;
  }

  int lx = 0;
  int ly = 0;

  for (int x = sx; x < ex; x++)
  {
    for (int y = sy; y < ey; y++)
    {
      //
      // Place shrine information at the center of area.
      //
      if (lx == 2 && ly == 2)
      {
        Position key = { x, y };
        ShrinesByPosition.emplace(key, type);
      }

      _map[x][y].Image = layout[lx][ly];
      ly++;
    }

    lx++;
    ly = 0;
  }

  return true;
}

// =============================================================================

void FeatureRooms::DemonizeLayout(StringV& layout)
{
  for (size_t x = 0; x < layout.size(); x++)
  {
    for (size_t y = 0; y < layout.size(); y++)
    {
      char img = layout[x][y];

      //
      // Change water and fountains to lava,
      // grass to dirt, trees to withered trees.
      //
      if (img == 'w' || img == 'W' || img == 'F')
      {
        img = 'l';
      }
      else if (img == 'g')
      {
        img = 'd';
      }
      else if (img == 'T')
      {
        img = 't';
      }

      layout[x][y] = img;
    }
  }
}

// =============================================================================

bool FeatureRooms::PlaceLayout(const Position& start,
                               StringV& layout,
                               RoomEdgeEnum dir,
                               bool demonize)
{
  int shiftX = Util::RandomRange(0, layout.size(), _rng);
  int shiftY = Util::RandomRange(0, layout[0].size(), _rng);

  int heightOffsetToStart = layout.size() - 1;
  int widthOffsetToStart = layout[0].size() - 1;

  int sx = start.X;
  int sy = start.Y;
  int ex = sx;
  int ey = sy;

  switch (dir)
  {
    case RoomEdgeEnum::NORTH:
    {
      sx -= heightOffsetToStart;
      sy -= shiftY;
    }
    break;

    case RoomEdgeEnum::EAST:
    {
      sx -= shiftX;
    }
    break;

    case RoomEdgeEnum::SOUTH:
    {
      sy -= shiftY;
    }
    break;

    case RoomEdgeEnum::WEST:
    {
      sx -= shiftX;
      sy -= widthOffsetToStart;
    }
    break;
  }

  ex = sx + layout.size();
  ey = sy + layout[0].size();

  if (!IsAreaValid({ sx, sy }, { ex, ey }))
  {
    return false;
  }

  if (demonize)
  {
    DemonizeLayout(layout);
  }

  int lx = 0;
  int ly = 0;

  for (int x = sx; x < ex; x++)
  {
    for (int y = sy; y < ey; y++)
    {
      _map[x][y].Image = layout[lx][ly];
      ly++;
    }

    lx++;
    ly = 0;
  }

  return true;
}

// =============================================================================

RoomEdgeEnum FeatureRooms::GetCarveDirectionForDeadend(Position pos)
{
  //
  // A little hacky method.
  //
  // Looks in opposite direction of a wall
  // and if there's a ground tile there, returns
  // the direction this wall should be carving into.
  //
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

// =============================================================================

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

// =============================================================================

//
// In order to forbid "eating" of walls by other rooms during generation,
// ensure that cell to be changed is surrounded by walls
// (i.e. it's not adjacent to some room space already built).
//
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

// =============================================================================

//
// Returns pair of coordinates span [ (x1 -> x2), (y1 -> y2) ]
// for a room centered along specified direction.
//
// 'roomSize' must be odd.
//
std::pair<Position, Position> FeatureRooms::CenterRoomAlongDir(const Position& start, int roomSize, RoomEdgeEnum dir)
{
  std::pair<Position, Position> res;

  int shift = (roomSize - 1) / 2;

  int sx = start.X;
  int sy = start.Y;
  int ex = start.X;
  int ey = start.Y;

  switch (dir)
  {
    case RoomEdgeEnum::NORTH:
      sx -= roomSize;
      sy -= shift;
      ey += shift;
      break;

    case RoomEdgeEnum::EAST:
      sx -= shift;
      ex += shift;
      ey += roomSize;
      break;

    case RoomEdgeEnum::SOUTH:
      ex += roomSize;
      sy -= shift;
      ey += shift;
      break;

    case RoomEdgeEnum::WEST:
      sx -= shift;
      ex += shift;
      sy -= roomSize;
      break;
  }

  if (dir == RoomEdgeEnum::NORTH)
  {
    // To attach layout to the door position
    sx++;
    ex++;

    // Include last row of the layout
    ey++;
  }
  else if (dir == RoomEdgeEnum::SOUTH)
  {
    // Same logic as above

    sx--;
    ex--;

    ey++;
  }
  else if (dir == RoomEdgeEnum::WEST)
  {
    sy++;
    ey++;

    ex++;
  }
  else if (dir == RoomEdgeEnum::EAST)
  {
    sy--;
    ey--;

    ex++;
  }

  res =
  {
    { sx, ex },
    { sy, ey }
  };

  return res;
}

// =============================================================================

bool FeatureRooms::IsAreaValid(const Position& start, const Position& end)
{
  for (int x = start.X; x < end.X; x++)
  {
    for (int y = start.Y; y < end.Y; y++)
    {
      if (!IsCellValid({ x, y }))
      {
        return false;
      }
    }
  }

  return true;
}
