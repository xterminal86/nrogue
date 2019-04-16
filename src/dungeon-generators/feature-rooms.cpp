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

  auto newSize = GetRandomRoomSize();

  int startX = _mapSize.X / 2 - newSize.X / 2;
  int startY = _mapSize.Y / 2 - newSize.Y / 2;

  Position start(startX, startY);

  CreateEmptyRoom(start, newSize);

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

      // Get position offsets for carving
      auto carveOffset = GetCarveOffsetsForDir(carveDir);

      newRoomStartPos.X = doorPos.X + carveOffset.X;
      newRoomStartPos.Y = doorPos.Y + carveOffset.Y;

      auto res = Util::WeightedRandom(_roomWeightByType);

      FeatureRoomType typeRolled = res.first;
      std::pair<int, int> weightAndMax = _weightsMap[res.first];
      int maxAllowed = weightAndMax.second;

      if (maxAllowed > 0 && _generatedSoFar[typeRolled] >= maxAllowed)
      {
        continue;
      }

      bool failed = TryToCreateRoom(doorPos, newRoomStartPos, typeRolled);
      if (!failed)
      {
        _map[doorPos.X][doorPos.Y].Image = '+';
        _generatedSoFar[typeRolled]++;
      }

      /*
      // Select random dir to carve

      std::vector<RoomBuildDirection> dirs =
      {
        RoomBuildDirection::NE,
        RoomBuildDirection::NW,
        RoomBuildDirection::SE,
        RoomBuildDirection::SW
      };

      int dirIndex = RNG::Instance().RandomRange(0, dirs.size());

      auto buildDir = dirs[dirIndex];

      int roomSizeX = RNG::Instance().RandomRange(roomMinSize, roomMaxSize);
      int roomSizeY = RNG::Instance().RandomRange(roomMinSize, roomMaxSize);

      Position roomSize = { roomSizeX, roomSizeY };

      if (IsAreaWalls(newRoomStartPos, roomSize, buildDir))
      {
        TryToCreateRoom(doorPos, newRoomStartPos, roomSize, buildDir);
      }
      */
    }
  }

  FillMapRaw();
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
                                   FeatureRoomType roomType)
{
  // TODO: moar room types

  bool failed = false;

  switch (roomType)
  {
    case FeatureRoomType::EMPTY:
    {
      int rnd = RNG::Instance().RandomRange(0, 2);
      bool isCentered = (rnd == 0);
      CreateEmptyRoom(doorPos, isCentered);
    }
    break;

    case FeatureRoomType::SHRINE:
    {
      // FIXME: kinda hack
      if (roomSize.X >= GlobalConstants::ShrineLayouts[0].size()
       && roomSize.Y >= GlobalConstants::ShrineLayouts[0].size())
      {
        int index = RNG::Instance().RandomRange(0, GlobalConstants::ShrineNameByType.size());
        auto it = GlobalConstants::ShrineNameByType.begin();
        std::advance(it, index);

        CreateShrine(newRoomStartPos, buildDir, it->first);
      }
      else
      {
        failed = true;
      }
    }
    break;

    case FeatureRoomType::DIAMOND:
    {
      if ((roomSize.X == roomSize.Y)
       && (roomSize.X >= 3)
       && (roomSize.X % 2 != 0))
      {
        CreateDiamondRoom(newRoomStartPos, roomSize, buildDir);
      }
      else
      {
        failed = true;
      }
    }
    break;

    default:
      failed = true;
      break;
  }

  return failed;
}

void FeatureRooms::CreateDiamondRoom(const Position& start,
                                     const Position& roomSize,
                                     RoomBuildDirection buildDir)
{

}

Position FeatureRooms::GetCarveOffsetsForDir(RoomEdgeEnum carveDir)
{
  std::map<RoomEdgeEnum, Position> carveOffsetsByDir =
  {
    { RoomEdgeEnum::NORTH, { -1, 0 } },
    { RoomEdgeEnum::EAST, { 0, 1 } },
    { RoomEdgeEnum::SOUTH, { 1, 0 } },
    { RoomEdgeEnum::WEST, { 0, -1 } }
  };

  return carveOffsetsByDir[carveDir];
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

bool FeatureRooms::CreateEmptyRoom(Position start, bool centered)
{  
  auto carveDir = GetCarveDirectionForDeadend(start);

  if (centered)
  {
    auto roomSizes = GetRandomRoomSize();

  }
  else
  {
  }

  Position dir = GetOffsetsForDirection(buildDir);

  ex += size.X * dir.X;
  ey += size.Y * dir.Y;

  if (!IsInsideMap(start) || !IsInsideMap({ ex, ey }))
  {
    return false;
  }

  FillWithEmpty({ sx, sy }, { ex, ey }, dir);

  return true;
}

void FeatureRooms::FillWithEmpty(const Position& start,
                                 const Position& end,
                                 const Position& dir)
{
  for (int x = start.X; x != end.X; x += dir.X)
  {
    for (int y = start.Y; y != end.Y; y += dir.Y)
    {
      _map[x][y].Image = '.';
    }
  }
}

void FeatureRooms::CreateShrine(const Position& start,
                                RoomBuildDirection buildDir,
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

  Position dir = GetOffsetsForDirection(buildDir);

  ex += roomSize * dir.X;
  ey += roomSize * dir.Y;

  for (int x = start.X; x != ex; x += dir.X)
  {
    for (int y = start.Y; y != ey; y += dir.Y)
    {
      _map[x][y].Image = layout[lx][ly];
      ly++;
    }

    lx++;
    ly = 0;
  }
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

bool FeatureRooms::IsAreaWalls(const Position& start,
                               const Position& size,
                               RoomBuildDirection buildDir)
{
  int sx = start.X;
  int sy = start.Y;
  int ex = start.X;
  int ey = start.Y;

  Position dir = GetOffsetsForDirection(buildDir);

  // Grab one more row and column in corresponding directions

  // Starting point goes in opposite direction

  sx += -dir.X;
  sy += -dir.Y;

  ex += (size.X * dir.X + dir.X);
  ey += (size.Y * dir.Y + dir.Y);

  if (!IsInsideMap({ sx, sy }) || !IsInsideMap({ ex, ey }))
  {
    return false;
  }

  for (int x = sx; x != ex; x += dir.X)
  {
    for (int y = sy; y != ey; y += dir.Y)
    {
      if (_map[x][y].Image != '#')
      {
        return false;
      }
    }
  }

  return true;
}

bool FeatureRooms::IsAreaWallsCentered(const Position& start,
                                       const Position& size)
{
  return true;
}

Position FeatureRooms::GetOffsetsForDirection(RoomBuildDirection dir)
{
  Position res;

  switch (dir)
  {
    case RoomBuildDirection::SE:
      res.X = 1;
      res.Y = 1;
      break;

    case RoomBuildDirection::SW:
      res.X = 1;
      res.Y = -1;
      break;

    case RoomBuildDirection::NW:
      res.X = -1;
      res.Y = -1;
      break;

    case RoomBuildDirection::NE:
      res.X = -1;
      res.Y = 1;
      break;
  }

  return res;
}
