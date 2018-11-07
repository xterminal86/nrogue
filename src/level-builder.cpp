#include "level-builder.h"

#include "util.h"
#include "rng.h"

LevelBuilder::LevelBuilder()
{
  _roomsCount = 0;
}

void LevelBuilder::BuildLevel(int mapSizeX, int mapSizeY)
{  
  _mapSize.X = mapSizeX;
  _mapSize.Y = mapSizeY;

  int roomIndex = RNG::Instance().RandomRange(0, GlobalConstants::DungeonRooms.size());
  int randomRotation = RNG::Instance().RandomRange(0, 4);

  auto layout = GlobalConstants::DungeonRooms[roomIndex];
  layout = Util::RotateRoomLayout(layout, (RoomLayoutRotation)randomRotation);

  RoomHelper start;
  start.ParseLayout(layout);

  _rooms.push(start);

  MapChunks.push_back(start);

  _roomsCount++;

  while (!_rooms.empty() && _roomsCount < MaxRooms)
  {    
    RoomHelper currentRoom = _rooms.back();

    _rooms.pop();

    TryToAddRoomTo(currentRoom);
  }
}

void LevelBuilder::TryToAddRoomTo(RoomHelper& currentRoom)
{
  std::map<RoomEdgeEnum, Position> cursorAddsByType =
  {
    { RoomEdgeEnum::NORTH, Position(-currentRoom.RoomSize, 0) },
    { RoomEdgeEnum::EAST, Position(0, currentRoom.RoomSize) },
    { RoomEdgeEnum::SOUTH, Position(currentRoom.RoomSize, 0) },
    { RoomEdgeEnum::WEST, Position(0, -currentRoom.RoomSize) },
  };

  for (auto& side : _edgeTypes)
  {
    auto edges = currentRoom.EdgesStatus();
    if (edges[side])
    {
      continue;
    }

    auto rooms = GetRoomsForLayout(currentRoom.Layout, side, GlobalConstants::DungeonRooms);
    if (rooms.size() != 0)
    {
      int index = RNG::Instance().RandomRange(0, rooms[side].size());

      rooms[side][index].UpperLeftCorner.X = currentRoom.UpperLeftCorner.X + cursorAddsByType[side].X;
      rooms[side][index].UpperLeftCorner.Y = currentRoom.UpperLeftCorner.Y + cursorAddsByType[side].Y;

      if (!CheckLimits(rooms[side][index]))
      {
        continue;
      }

      rooms[side][index].SetEdgeLockStatus(_oppositeEdgeByType[side], true);

      _rooms.push(rooms[side][index]);
      MapChunks.push_back(rooms[side][index]);

      _roomsCount++;
    }
  }
}

void LevelBuilder::PrintResult()
{
  printf("***** RESULT *****\n\n");

  PrintChunks();
}

void LevelBuilder::PrintChunks()
{
  for (auto& i : MapChunks)
  {
    printf("\n[%i;%i]\n", i.UpperLeftCorner.X, i.UpperLeftCorner.Y);
    printf("N: %i E: %i S: %i W: %i\n", i.NorthEdgeLocked,
                                        i.EastEdgeLocked,
                                        i.SouthEdgeLocked,
                                        i.WestEdgeLocked);

    for (auto& row : i.Layout)
    {
      for (auto& c : row)
      {
        printf("%c", c);
      }

      printf("\n");
    }

    printf("\n");

    for (int x = 0; x < i.OccupyMap.size(); x++)
    {
      for (int y = 0; y < i.OccupyMap[x].size(); y++)
      {
        printf("%i", i.OccupyMap[x][y]);
      }

      printf("\n");
    }

    printf("\n");

    for (auto& i : i.RoomEdgesByType)
    {
      printf ("%s: ", _edgeNameByType[i.first].data());

      for (auto c : i.second)
      {
        printf ("%i ", c);
      }

      printf("\n");
    }
  }

  printf("Total of %i rooms\n", _roomsCount);
}

std::map<RoomEdgeEnum, std::vector<RoomHelper>> LevelBuilder::GetRoomsForLayout(RoomLayout layout, RoomEdgeEnum side, std::vector<RoomLayout> roomsList)
{
  std::map<RoomEdgeEnum, std::vector<RoomHelper>> res;

  RoomHelper currentRoom;
  currentRoom.ParseLayout(layout);

  for (auto& r : roomsList)
  {
    RoomHelper rh;
    rh.ParseLayout(r);

    if (currentRoom.CanAttach(rh, side))
    {
      res[side].push_back(rh);
    }
  }

  return res;
}

bool LevelBuilder::CheckLimits(RoomHelper& room)
{
  int rx = std::abs(room.UpperLeftCorner.X);
  int ry = std::abs(room.UpperLeftCorner.Y);

  bool cond = (rx >= 0 && rx < _mapSize.X / 2 && ry >= 0 && ry < _mapSize.Y / 2);

  return cond;
}
