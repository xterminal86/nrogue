#include "level-builder.h"

#include "util.h"
#include "rng.h"

void LevelBuilder::BuildLevel(int sizeX, int sizeY)
{
  for (int x = 0; x < sizeX; x++)
  {
    std::vector<std::string> row;

    for (int y = 0; y < sizeY; y++)
    {
      row.push_back(".");
    }

    _map.push_back(row);
  }

  int maxRooms = 16;

  int roomsCount = 0;

  int roomIndex = RNG::Instance().RandomRange(0, GlobalConstants::DungeonRooms.size());
  int randomRotation = RNG::Instance().RandomRange(0, 4);

  auto layout = GlobalConstants::DungeonRooms[roomIndex];
  layout = Util::RotateRoomLayout(layout, (RoomLayoutRotation)randomRotation);

  _rooms.emplace(layout);

  RoomHelper rh;
  rh.ParseLayout(layout);

  rh.NorthEdgeLocked = true;
  rh.WestEdgeLocked = true;

  _mapChunks[_cursor] = rh;

  roomsCount++;

  while (_rooms.size() != 0 && roomsCount < maxRooms)
  {
    RoomLayout currentRoom = _rooms.back();

    _rooms.pop();

    rh.ParseLayout(currentRoom);

    TryToAddRoom(rh);
  }
}

void LevelBuilder::TryToAddRoom(RoomHelper& currentRoom)
{
  std::vector<RoomEdgeEnum> edgeTypes =
  {
    RoomEdgeEnum::NORTH,
    RoomEdgeEnum::EAST,
    RoomEdgeEnum::SOUTH,
    RoomEdgeEnum::WEST
  };

  std::map<RoomEdgeEnum, RoomEdgeEnum> oppositeEdgeByType =
  {
    { RoomEdgeEnum::NORTH, RoomEdgeEnum::SOUTH },
    { RoomEdgeEnum::EAST, RoomEdgeEnum::WEST },
    { RoomEdgeEnum::SOUTH, RoomEdgeEnum::NORTH },
    { RoomEdgeEnum::WEST, RoomEdgeEnum::EAST }
  };

  std::vector<RoomLayoutRotation> allDegrees =
  {
    RoomLayoutRotation::NONE,
    RoomLayoutRotation::CCW_90,
    RoomLayoutRotation::CCW_180,
    RoomLayoutRotation::CCW_270
  };

  auto roomEdges = currentRoom.Edges();
  for (auto type : edgeTypes)
  {
    if (!roomEdges[type])
    {
      continue;
    }


  }
}
