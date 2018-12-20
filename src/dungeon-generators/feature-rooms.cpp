#include "feature-rooms.h"

/// Builds dungeon by attaching new rooms to existing ones
/// maxIterations should be empirically chosen, because
/// it depends on roomSizes.
/// Set to (mapSize.X * mapSize.Y) to use all available map area, but
/// be aware of computational time.
void FeatureRooms::Generate(Position mapSize, Position roomSizes, int maxIterations)
{
  _mapSize = mapSize;

  _map = CreateFilledMap(mapSize.X, mapSize.Y);

  int roomMinSize = roomSizes.X;
  int roomMaxSize = roomSizes.Y;

  int roomSizeX = RNG::Instance().RandomRange(roomMinSize, roomMaxSize);
  int roomSizeY = RNG::Instance().RandomRange(roomMinSize, roomMaxSize);

  int startX = _mapSize.X / 2 - roomSizeX / 2;
  int startY = _mapSize.Y / 2 - roomSizeY / 2;

  Position start(startX, startY);

  CreateRoom(start, { roomSizeX, roomSizeY });

  for (int i = 0; i < maxIterations; i++)
  {
    Position newPos;

    std::vector<Position> validCells;

    for (int x = 0; x < mapSize.X; x++)
    {
      for (int y = 0; y < mapSize.Y; y++)
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

    if (validCells.size() != 0)
    {
      int posIndex = RNG::Instance().RandomRange(0, validCells.size());

      Position doorPos = validCells[posIndex];

      // Get direction of further carving for deadend.
      // I.e.
      // ####
      // ..X# ->
      // ####
      //
      RoomEdgeEnum carveDir = GetCarveDirectionForDeadend(doorPos);

      std::map<RoomEdgeEnum, Position> carveOffsetsByDir =
      {
        { RoomEdgeEnum::NORTH, { -1, 0 } },
        { RoomEdgeEnum::EAST, { 0, 1 } },
        { RoomEdgeEnum::SOUTH, { 1, 0 } },
        { RoomEdgeEnum::WEST, { 0, -1 } }
      };

      std::vector<RoomBuildDirection> dirs =
      {
        RoomBuildDirection::NE,
        RoomBuildDirection::NW,
        RoomBuildDirection::SE,
        RoomBuildDirection::SW
      };

      Position carveOffset = carveOffsetsByDir[carveDir];

      newPos.X = doorPos.X + carveOffset.X;
      newPos.Y = doorPos.Y + carveOffset.Y;

      int dirIndex = RNG::Instance().RandomRange(0, dirs.size());

      auto buildDir = dirs[dirIndex];

      auto offsets = GetOffsetsForDirection(buildDir);

      int roomSizeX = RNG::Instance().RandomRange(roomMinSize, roomMaxSize);
      int roomSizeY = RNG::Instance().RandomRange(roomMinSize, roomMaxSize);

      if (IsAreaWalls(newPos, { roomSizeX, roomSizeY }, buildDir))
      {
        _map[doorPos.X][doorPos.Y].Image = '+';

        CreateRoom(newPos, { roomSizeX, roomSizeY }, buildDir);
      }
    }
  }

  FillMapRaw();
}

void FeatureRooms::CreateRoom(Position upperLeftCorner, Position size, RoomBuildDirection buildDir)
{
  int sx = upperLeftCorner.X;
  int sy = upperLeftCorner.Y;
  int ex = upperLeftCorner.X;
  int ey = upperLeftCorner.Y;

  Position dir = GetOffsetsForDirection(buildDir);

  ex += size.X * dir.X;
  ey += size.Y * dir.Y;

  if (!IsInsideMap(upperLeftCorner) || !IsInsideMap({ ex, ey }))
  {
    return;
  }

  for (int x = sx; x != ex; x += dir.X)
  {
    for (int y = sy; y != ey; y += dir.Y)
    {
      _map[x][y].Image = '.';
    }
  }
}

RoomEdgeEnum FeatureRooms::GetCarveDirectionForDeadend(Position pos)
{
  // A little hacky method

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

bool FeatureRooms::IsAreaWalls(Position corner, Position size, RoomBuildDirection buildDir)
{
  int sx = corner.X;
  int sy = corner.Y;
  int ex = corner.X;
  int ey = corner.Y;

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
      if (_map[x][y].Image == '.')
      {
        return false;
      }
    }
  }

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
