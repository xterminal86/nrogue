#include "level-builder.h"

#include "util.h"
#include "rng.h"

void LevelBuilder::BuildLevel(std::vector<RoomForLevel> possibleRooms, int startX, int startY, int mapSizeX, int mapSizeY)
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

  // TODO: !!! optimize, optimize, optimize !!!
  // It's too fucking slow right now.

  while (!_rooms.empty())
  {    
    RoomHelper currentRoom = _rooms.front();

    _rooms.pop();    

    TryToAddRoomTo(currentRoom, RoomEdgeEnum::NORTH);
    TryToAddRoomTo(currentRoom, RoomEdgeEnum::EAST);
    TryToAddRoomTo(currentRoom, RoomEdgeEnum::SOUTH);
    TryToAddRoomTo(currentRoom, RoomEdgeEnum::WEST);
  }

  // PrintVisitedCells();
}

void LevelBuilder::TryToAddRoomTo(RoomHelper& currentRoom, RoomEdgeEnum side)
{
  //auto dbg = Util::StringFormat("Trying to add to [%i;%i] on %s...", currentRoom.UpperLeftCorner.X, currentRoom.UpperLeftCorner.Y, _edgeNameByType[side].data());
  //Logger::Instance().Print(dbg);

  int offset = currentRoom.RoomSize + 1;

  // Offsets are swapped to correspond to map coordinates
  std::map<RoomEdgeEnum, Position> cursorAddsByType =
  {
    { RoomEdgeEnum::NORTH, Position(0, -offset) },
    { RoomEdgeEnum::EAST, Position(offset, 0) },
    { RoomEdgeEnum::SOUTH, Position(0, offset) },
    { RoomEdgeEnum::WEST, Position(-offset, 0) },
  };

  Position areaStart;
  areaStart.X = currentRoom.UpperLeftCorner.X + cursorAddsByType[side].X;
  areaStart.Y = currentRoom.UpperLeftCorner.Y + cursorAddsByType[side].Y;

  if (IsAreaVisited(areaStart, currentRoom.RoomSize))
  {
    return;
  }

  auto rooms = GetRoomsForLayout(currentRoom.Layout, side);
  if (rooms.size() != 0)
  {
    int index = RNG::Instance().RandomRange(0, rooms[side].size());

    //Logger::Instance().Print("Found:");
    //Util::PrintLayout(rooms[side][index].Layout);

    rooms[side][index].UpperLeftCorner.X = currentRoom.UpperLeftCorner.X + cursorAddsByType[side].X;
    rooms[side][index].UpperLeftCorner.Y = currentRoom.UpperLeftCorner.Y + cursorAddsByType[side].Y;

    //dbg = Util::StringFormat("position [%i;%i]", rooms[side][index].UpperLeftCorner.X, rooms[side][index].UpperLeftCorner.Y);
    //Logger::Instance().Print(dbg);

    if (!CheckLimits(rooms[side][index]))
    {
      //dbg = Util::StringFormat("[%i;%i] failed on CheckLimits()!", rooms[side][index].UpperLeftCorner.X, rooms[side][index].UpperLeftCorner.Y);
      //Logger::Instance().Print(dbg);
      return;
    }

    _rooms.push(rooms[side][index]);
    MapChunks.push_back(rooms[side][index]);

    VisitCells(currentRoom);

    _roomsCount++;
  }
  else
  {
    //Logger::Instance().Print("No rooms found!");
  }
}

std::map<RoomEdgeEnum, std::vector<RoomHelper>> LevelBuilder::GetRoomsForLayout(RoomLayout layout, RoomEdgeEnum side)
{
  std::map<RoomEdgeEnum, std::vector<RoomHelper>> res;

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

      if (currentRoom.CanAttach(rh, side))
      {
        res[side].push_back(rh);
      }
      else
      {
        //Logger::Instance().Print("Can't attach:");
        //Util::PrintLayout(rh.Layout);
      }
    }
  }

  return res;
}

bool LevelBuilder::CheckLimits(RoomHelper& room)
{
  int size = room.Layout.size();

  int lx = room.UpperLeftCorner.X;
  int ly = room.UpperLeftCorner.Y;
  int hx = lx + size;
  int hy = ly + size;

  bool fitsInMap = (lx >= 0 && hx < _mapSize.X
                 && ly >= 0 && hy < _mapSize.Y);

  //auto dbg = Util::StringFormat("\t\tCheckLimits(): lx: %i ly: %i hx: %i hy: %i...", lx, ly, hx, hy);
  //Logger::Instance().Print(dbg);

  if (!fitsInMap)
  {
    //Logger::Instance().Print("\t\tDoesn't fit in map!");
    return false;
  }

  //Logger::Instance().Print("\t\tPassed!");

  return true;
}

bool LevelBuilder::IsAreaVisited(Position start, int size)
{
  int lx = start.X;
  int ly = start.Y;
  int hx = lx + size;
  int hy = ly + size;

  lx = Util::Clamp(lx, 0, _mapSize.X - 1);
  ly = Util::Clamp(ly, 0, _mapSize.Y - 1);
  hx = Util::Clamp(hx, 0, _mapSize.X - 1);
  hy = Util::Clamp(hy, 0, _mapSize.Y - 1);

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

void LevelBuilder::VisitCells(RoomHelper& room)
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

RoomLayout LevelBuilder::SelectRoom()
{
  int dice = Util::RollDice();

  int roomSize = _roomsForLevel[0].Room.size();

  std::vector<RoomLayout> rooms;

  for (auto& r : _roomsForLevel)
  {
    if (dice <= r.Chance)
    {
      rooms.push_back(r.Room);
    }
  }

  // Construct fall back room (empty NxN)
  if (rooms.size() == 0)
  {
    auto res = ConstructEmptyRoom(roomSize);
    rooms.push_back(res);
  }

  int index = RNG::Instance().RandomRange(0, rooms.size());

  return rooms[index];
}

std::vector<RoomLayout> LevelBuilder::SelectRooms()
{
  int dice = Util::RollDice();

  //auto dbg = Util::StringFormat("\t\tRolled: %i", dice);
  //Logger::Instance().Print(dbg);

  int roomSize = _roomsForLevel[0].Room.size();

  std::vector<RoomLayout> rooms;

  for (auto& r : _roomsForLevel)
  {
    //dbg = Util::StringFormat("\t\tChance: %i", r.Chance);
    //Logger::Instance().Print(dbg);

    if (dice <= r.Chance)
    {
      // Logger::Instance().Print("\t\tPassed!");

      rooms.push_back(r.Room);
    }
  }

  // Construct fall back room (empty NxN)
  if (rooms.size() == 0)
  {
    auto res = ConstructEmptyRoom(roomSize);
    rooms.push_back(res);
  }

  return rooms;
}

RoomLayout LevelBuilder::ConstructEmptyRoom(int size)
{
  RoomLayout res;

  for (int x = 0; x < size; x++)
  {
    std::string row;
    for (int y = 0; y < size; y++)
    {
      row += ".";
    }

    res.push_back(row);
  }

  return res;
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

void LevelBuilder::PrintVisitedCells()
{
  std::string visitedCellsStr;

  for (int x = 0; x < _mapSize.X; x++)
  {
    for (int y = 0; y < _mapSize.Y; y++)
    {
      visitedCellsStr += std::to_string(_visitedCells[x][y].Visited);
    }

    visitedCellsStr += "\n";
  }

  Logger::Instance().Print(visitedCellsStr);
}
