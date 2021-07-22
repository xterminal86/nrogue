#include "from-layouts.h"

#include "rng.h"
#include "util.h"

/// \brief Build dungeon by attaching random rooms to each other from
/// possibleRooms list.
///
/// May result in imperfect and/or impassable dungeon
/// if layouts in the list are not connectable.
void FromLayouts::Generate(const std::vector<RoomForLevel>& possibleRooms, int startX, int startY, int mapSizeX, int mapSizeY)
{
  _roomsForLevel = possibleRooms;

  for (int x = 0; x < mapSizeX; x++)
  {
    std::vector<MapCell> row;
    for (int y = 0; y < mapSizeY; y++)
    {
      MapCell c;
      c.Coordinates.X = x;
      c.Coordinates.Y = y;
      c.Visited = false;
      row.push_back(c);
    }

    _visitedCells.push_back(row);
  }

  _mapSize.X = mapSizeX;
  _mapSize.Y = mapSizeY;

  _startingPoint.X = startX;
  _startingPoint.Y = startY;

  int randomRotation = RNG::Instance().RandomRange(0, 4);

  auto layout = SelectRoom();
  layout = Util::RotateRoomLayout(layout, (RoomLayoutRotation)randomRotation);

  RoomHelper start;
  start.ParseLayout(layout);
  start.UpperLeftCorner.Set(startX, startY);

  _rooms.push(start);

  MapChunks.push_back(start);

  while (!_rooms.empty())
  {
    RoomHelper currentRoom = _rooms.top();

    _rooms.pop();

    TryToAddRoomTo(currentRoom, RoomEdgeEnum::NORTH);
    TryToAddRoomTo(currentRoom, RoomEdgeEnum::EAST);
    TryToAddRoomTo(currentRoom, RoomEdgeEnum::SOUTH);
    TryToAddRoomTo(currentRoom, RoomEdgeEnum::WEST);
  }

  ConvertChunksToMapRaw();
}

RoomLayout FromLayouts::SelectRoom()
{
  int dice = Util::Rolld100();

  std::vector<RoomLayout> rooms;

  for (auto& r : _roomsForLevel)
  {
    if (dice <= r.Chance)
    {
      rooms.push_back(r.Layout);
    }
  }

  // If we were unlucky with dice,
  // just select random room
  if (rooms.size() == 0)
  {
    int index = RNG::Instance().RandomRange(0, _roomsForLevel.size());
    auto fallback = _roomsForLevel[index];
    rooms.push_back(fallback.Layout);
  }

  int index = RNG::Instance().RandomRange(0, rooms.size());

  return rooms[index];
}

void FromLayouts::TryToAddRoomTo(const RoomHelper& currentRoom, RoomEdgeEnum side)
{
  int offset = currentRoom.RoomSize + 1;

  // Offsets are swapped to correspond to map coordinates
  std::map<RoomEdgeEnum, Position> cursorAddsByType =
  {
    { RoomEdgeEnum::NORTH, Position(0, -offset) },
    { RoomEdgeEnum::EAST, Position(offset, 0) },
    { RoomEdgeEnum::SOUTH, Position(0, offset) },
    { RoomEdgeEnum::WEST, Position(-offset, 0) },
  };

  Position newAreaStart;
  newAreaStart.X = currentRoom.UpperLeftCorner.X + cursorAddsByType[side].X;
  newAreaStart.Y = currentRoom.UpperLeftCorner.Y + cursorAddsByType[side].Y;

  if (!CheckLimits(newAreaStart, currentRoom.RoomSize))
  {
    return;
  }

  if (IsAreaVisited(newAreaStart, currentRoom.RoomSize))
  {
    return;
  }

  // It is possible to get 0 rooms
  // (e.g. trying to add open area to full wall side)
  auto rooms = GetRoomsForLayout(currentRoom.Layout, side);
  if (rooms.size() != 0)
  {
    int index = RNG::Instance().RandomRange(0, rooms.size());

    rooms[index].UpperLeftCorner.X = newAreaStart.X;
    rooms[index].UpperLeftCorner.Y = newAreaStart.Y;

    _rooms.push(rooms[index]);
    MapChunks.push_back(rooms[index]);
  }
  else
  {
    // FIXME: may result in walled room

    // If no rooms could be added,
    // element 0 is assumed to be fallback layout
    auto fallback = _roomsForLevel[0];

    //auto fallback = CreateSquareLayout(fallback.Layout.size(), '.');

    RoomHelper rh;
    rh.ParseLayout(fallback.Layout);

    rh.UpperLeftCorner.X = newAreaStart.X;
    rh.UpperLeftCorner.Y = newAreaStart.Y;

    _rooms.push(rh);
    MapChunks.push_back(rh);

    //Logger::Instance().Print("No rooms to attach found - creating empty spot");
  }

  VisitCells(currentRoom);

  _roomsCount++;
}

void FromLayouts::VisitCells(const RoomHelper& room)
{
  int size = room.Layout.size();

  int lx = room.UpperLeftCorner.X;
  int ly = room.UpperLeftCorner.Y;
  int hx = lx + size;
  int hy = ly + size;

  for (int x = lx; x <= hx; x++)
  {
    for (int y = ly; y <= hy; y++)
    {
      _visitedCells[x][y].Visited = true;
    }
  }

  //auto str = Util::StringFormat("\t\tMarked area lx: %i ly: %i hx: %i hy: %i", lx, ly, hx, hy);
  //Logger::Instance().Print(str);
}

void FromLayouts::ConvertChunksToMapRaw()
{
  for (int x = 0; x < _mapSize.X; x++)
  {
    std::vector<char> row;
    for (int y = 0; y < _mapSize.Y; y++)
    {
      row.push_back('.');
    }

    MapRaw.push_back(row);
  }

  for (auto& chunk : MapChunks)
  {
    int x = chunk.UpperLeftCorner.X;
    int y = chunk.UpperLeftCorner.Y;

    for (auto& row : chunk.Layout)
    {
      for (size_t c = 0; c < row.length(); c++)
      {
        MapRaw[x][y + c] = row[c];
      }

      x++;
    }
  }
}

bool FromLayouts::IsAreaVisited(const Position& start, int roomSize)
{
  int lx = start.X;
  int ly = start.Y;
  int hx = lx + roomSize;
  int hy = ly + roomSize;

  /*
  lx = Util::Clamp(lx, 0, _mapSize.X - 1);
  ly = Util::Clamp(ly, 0, _mapSize.Y - 1);
  hx = Util::Clamp(hx, 0, _mapSize.X - 1);
  hy = Util::Clamp(hy, 0, _mapSize.Y - 1);
  */

  for (int x = lx; x <= hx; x++)
  {
    for (int y = ly; y <= hy; y++)
    {
      if (_visitedCells[x][y].Visited)
      {
        //auto dbg = Util::StringFormat("\t\tCell [%i;%i] is already occupied!", x, y);
        //Logger::Instance().Print(dbg);
        return true;
      }
    }
  }

  return false;
}

std::vector<RoomHelper> FromLayouts::GetRoomsForLayout(const RoomLayout& layout, RoomEdgeEnum side)
{
  std::vector<RoomHelper> roomsVector;

  RoomHelper currentRoom;
  currentRoom.ParseLayout(layout);

  //Logger::Instance().Print("Processing:");
  //Util::PrintLayout(currentRoom.Layout);

  auto rooms = SelectRooms();

  for (auto& r : rooms)
  {
    for (auto& angle : _allDegrees)
    {
      auto newLayout = Util::RotateRoomLayout(r, angle);

      RoomHelper rh;
      rh.ParseLayout(newLayout);

      //Logger::Instance().Print("Trying to attach:");
      //Util::PrintLayout(rh.Layout);

      if (currentRoom.CanAttach(rh, side))
      {
        roomsVector.push_back(rh);
      }
      else
      {
        //Logger::Instance().Print("Can't attach:");
        //Util::PrintLayout(rh.Layout);
      }
    }
  }

  return roomsVector;
}

std::vector<RoomLayout> FromLayouts::SelectRooms()
{
  int dice = Util::Rolld100();

  //auto dbg = Util::StringFormat("\t\tRolled: %i", dice);
  //Logger::Instance().Print(dbg);

  std::vector<RoomLayout> rooms;

  for (auto& r : _roomsForLevel)
  {
    //dbg = Util::StringFormat("\t\tChance: %i", r.Chance);
    //Logger::Instance().Print(dbg);

    if (dice <= r.Chance)
    {
      // Logger::Instance().Print("\t\tPassed!");

      rooms.push_back(r.Layout);
    }
  }

  return rooms;
}
