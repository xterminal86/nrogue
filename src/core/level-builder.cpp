#include "level-builder.h"

#include "util.h"
#include "rng.h"

void LevelBuilder::CellularAutomata(Position mapSize, int initialWallChance, int birthThreshold, int deathThreshold, int maxIterations)
{
  _map = CreateRandomlyFilledMap(mapSize.X, mapSize.Y, initialWallChance);

  // If we change data in-place we would mix old and
  // new results, so we should perform check on initial map
  // and store results in temporary one and copy them after
  // whole initial map was scanned.
  auto tmp = CreateEmptyMap(mapSize.X, mapSize.Y);

  for (int i = 0; i < maxIterations; i++)
  {
    // copy original map contents to temp
    for (int x = 0; x < mapSize.X; x++)
    {
      for (int y = 0; y < mapSize.Y; y++)
      {
        tmp[x][y].Image = _map[x][y].Image;
      }
    }

    for (int x = 0; x < mapSize.X; x++)
    {
      for (int y = 0; y < mapSize.Y; y++)
      {
        if (_map[x][y].Visited)
        {
          continue;
        }

        int aliveCells = CountAround(x, y, '.');
        if (_map[x][y].Image == '.')
        {
          tmp[x][y].Image = (aliveCells < deathThreshold) ? '#' : '.';
        }
        else if (_map[x][y].Image == '#')
        {
          tmp[x][y].Image = (aliveCells > birthThreshold) ? '.' : '#';
        }
      }
    }

    // copy temp array to original map
    for (int x = 0; x < mapSize.X; x++)
    {
      for (int y = 0; y < mapSize.Y; y++)
      {
        _map[x][y].Image = tmp[x][y].Image;
      }
    }
  }

  FillMapRaw();
}

int LevelBuilder::CountAround(int x, int y, char ch)
{
  int lx = x - 1;
  int ly = y - 1;
  int hx = x + 1;
  int hy = y + 1;

  int res = 0;

  for (int ix = lx; ix <= hx; ix++)
  {
    for (int iy = ly; iy <= hy; iy++)
    {
      if (x == ix && y == iy)
      {
        continue;
      }

      if (!IsInsideMap({ ix, iy }))
      {
        //res++;
        continue;
      }

      if (_map[ix][iy].Image == ch)
      {
        res++;
      }
    }
  }

  return res;
}

/// Builds tunnels perpendicular to previous direction,
/// backtracks to previous position if failed.
///
/// Additional tweaks include removal of deadends and
/// ....
/// ..#.
/// .#..
/// ....
/// situations
void LevelBuilder::BacktrackingTunneler(Position mapSize, Position tunnelMinMax, bool additionalTweaks, Position start)
{
  _mapSize = mapSize;

  _map = CreateFilledMap(mapSize.X, mapSize.Y);

  int sx, sy = 0;

  if (start.X == -1 || start.Y == -1)
  {
    sx = RNG::Instance().RandomRange(1, mapSize.X - 1);
    sy = RNG::Instance().RandomRange(1, mapSize.Y - 1);
  }
  else
  {
    sx = start.X;
    sy = start.Y;
  }

  _startingPoint.Set(sx, sy);

  _map[sx][sy].Image = '.';
  _map[sx][sy].Visited = true;

  // Contains corridor start position and direction
  std::stack<std::pair<Position, Position>> nodePoints;

  Position rndDir = GetRandomDir(_startingPoint)[0];

  nodePoints.push({ _startingPoint, rndDir });

  while (!nodePoints.empty())
  {
    auto currentNode = nodePoints.top();

    Position pos = currentNode.first;
    Position dir = currentNode.second;

    int x = pos.X;
    int y = pos.Y;

    _map[x][y].Image = '.';
    _map[x][y].Visited = true;

    int tunMin = tunnelMinMax.X;
    int tunMax = tunnelMinMax.Y;

    int length = RNG::Instance().RandomRange(tunMin, tunMax);

    //auto str = Util::StringFormat("Current node: %i %i dir %i %i selected length %i", x, y, dir.X, dir.Y, length);
    //Logger::Instance().Print(str);

    bool wasBuilt = true;

    while (length > 0)
    {
      //auto str = Util::StringFormat("\tTrying to build to %i %i", x + dir.X, y + dir.Y);
      //Logger::Instance().Print(str);

      if (IsInsideMap({ x + dir.X, y + dir.Y})
       && IsDeadEnd({ x + dir.X, y + dir.Y })
       && !_map[x + dir.X][y + dir.Y].Visited)
      {
        x += dir.X;
        y += dir.Y;

        _map[x][y].Image = '.';
        _map[x][y].Visited = true;
      }
      else
      {
        //auto str = Util::StringFormat("\t\tFailed! Adding node point %i %i dir %i %i", x, y, dir.X, dir.Y);
        //Logger::Instance().Print(str);

        wasBuilt = false;

        //nodePoints.push({ { x, y }, dir });

        break;
      }

      length--;
    }

    if (wasBuilt)
    {
      //auto str = Util::StringFormat("\tFinished building, adding node point %i %i dir %i %i", x, y, dir.X, dir.Y);
      //Logger::Instance().Print(str);

      nodePoints.push({ { x, y }, dir });
    }

    auto res = TryToGetPerpendicularDir({ x, y }, dir);
    if (res.size() == 0)
    {
      nodePoints.pop();

      /*
      if (nodePoints.size() != 0)
      {
        auto p = nodePoints.top();
        auto str = Util::StringFormat("\tBacktracking to %i %i dir %i %i", p.first.X, p.first.Y, p.second.X, p.second.Y);
        Logger::Instance().Print(str);
      }
      */
    }
    else
    {
      nodePoints.push({ { x, y }, res[0] });
      //auto str = Util::StringFormat("\tAdding %i %i dir %i %i", x, y, res[0].X, res[0].Y);
      //Logger::Instance().Print(str);
    }
  }

  if (additionalTweaks)
  {
    FillDeadEnds();
    CutProblemCorners();
  }

  FillMapRaw();
}

/// Builds tunnels in all directions until maxTunnels are built.
void LevelBuilder::Tunneler(Position mapSize, int maxTunnels, Position tunnelLengthMinMax, Position start)
{
  int tunnelsMax = maxTunnels;

  _mapSize = mapSize;

  _map = CreateFilledMap(mapSize.X, mapSize.Y);

  int sx, sy = 0;

  if (start.X == -1 || start.Y == -1)
  {
    sx = RNG::Instance().RandomRange(1, mapSize.X - 1);
    sy = RNG::Instance().RandomRange(1, mapSize.Y - 1);
  }
  else
  {
    sx = start.X;
    sy = start.Y;
  }

  _startingPoint.Set(sx, sy);

  std::vector<Position> directions =
  {
    { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 }
  };

  Position mapPos(sx, sy);

  int index = RNG::Instance().RandomRange(0, directions.size());
  Position lastDir = directions[index];

  while (tunnelsMax > 0)
  {
    Position newDir = GetRandomPerpendicularDir(lastDir);

    int currentLength = 0;
    int rndLength = RNG::Instance().RandomRange(tunnelLengthMinMax.X, tunnelLengthMinMax.Y);

    std::vector<Position> corridor;

    while (currentLength <= rndLength && IsInsideMap(mapPos))
    {
      corridor.push_back(mapPos);

      _map[mapPos.X][mapPos.Y].Image = '.';

      mapPos.X += newDir.X;
      mapPos.Y += newDir.Y;

      currentLength++;
    }

    lastDir.Set(newDir);

    index = RNG::Instance().RandomRange(0, corridor.size());

    mapPos = corridor[index];

    tunnelsMax--;
  }

  FillMapRaw();
}

/// 1. Get random direction.
/// 2. Try to replace wall with empty space.
/// 3. Backtrack to previous cell if failed.
/// 4. Repeat until all cells are visited.
void LevelBuilder::RecursiveBacktracker(Position mapSize, Position startingPoint)
{
  std::stack<Position> openCells;

  _mapSize = mapSize;

  _map = CreateFilledMap(mapSize.X, mapSize.Y);

  int sx, sy = 0;

  if (startingPoint.X == -1 || startingPoint.Y == -1)
  {
    sx = RNG::Instance().RandomRange(1, mapSize.X - 1);
    sy = RNG::Instance().RandomRange(1, mapSize.Y - 1);
  }
  else
  {
    sx = startingPoint.X;
    sy = startingPoint.Y;
  }

  _startingPoint.Set(sx, sy);

  _map[sx][sy].Visited = true;
  _map[sx][sy].Image = '.';

  Position p(sx, sy);
  openCells.push(p);

  while (!openCells.empty())
  {
    auto np = openCells.top();
    auto res = GetRandomCell(np);
    if (res.size() != 0)
    {
      openCells.push(res[0]);
    }
    else
    {
      openCells.pop();
    }
  }

  CutProblemCorners();
  FillMapRaw();  
}

void LevelBuilder::BuildLevelFromLayouts(std::vector<RoomForLevel>& possibleRooms, int startX, int startY, int mapSizeX, int mapSizeY)
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

  ConvertChunksToLayout();

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

  Position newAreaStart;
  newAreaStart.X = currentRoom.UpperLeftCorner.X + cursorAddsByType[side].X;
  newAreaStart.Y = currentRoom.UpperLeftCorner.Y + cursorAddsByType[side].Y;

  if (!CheckLimits(newAreaStart, currentRoom.RoomSize))
  {
    //dbg = Util::StringFormat("[%i;%i] failed on CheckLimits()!", newAreaStart.X, newAreaStart.Y);
    //Logger::Instance().Print(dbg);
    return;
  }

  if (IsAreaVisited(newAreaStart, currentRoom.RoomSize))
  {
    //dbg = Util::StringFormat("[%i;%i] already visited!", newAreaStart.X, newAreaStart.Y);
    //Logger::Instance().Print(dbg);
    return;
  }

  // It is possible to get 0 rooms
  // (e.g. trying to add open area to full wall side)
  auto rooms = GetRoomsForLayout(currentRoom.Layout, side);
  if (rooms.size() != 0)
  {    
    //dbg = Util::StringFormat("Rooms to select: %i", rooms.size());
    //Logger::Instance().Print(dbg);

    int index = RNG::Instance().RandomRange(0, rooms.size());

    //Logger::Instance().Print("Selected:");
    //Util::PrintLayout(rooms[index].Layout);

    rooms[index].UpperLeftCorner.X = newAreaStart.X;
    rooms[index].UpperLeftCorner.Y = newAreaStart.Y;

    //dbg = Util::StringFormat("position [%i;%i]", rooms[side][index].UpperLeftCorner.X, rooms[side][index].UpperLeftCorner.Y);
    //Logger::Instance().Print(dbg);

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

std::vector<RoomHelper> LevelBuilder::GetRoomsForLayout(RoomLayout& layout, RoomEdgeEnum side)
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

bool LevelBuilder::CheckLimits(Position& start, int roomSize)
{
  int lx = start.X;
  int ly = start.Y;
  int hx = lx + roomSize;
  int hy = ly + roomSize;

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

bool LevelBuilder::IsInsideMap(Position pos)
{
  return (pos.X >= 1
       && pos.X < _mapSize.X - 1
       && pos.Y >= 1
       && pos.Y < _mapSize.Y - 1);
}

bool LevelBuilder::IsAreaVisited(Position& start, int roomSize)
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

std::vector<RoomLayout> LevelBuilder::SelectRooms()
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

RoomLayout LevelBuilder::CreateSquareLayout(int size, chtype ch)
{
  RoomLayout res;

  for (int x = 0; x < size; x++)
  {
    std::string row;
    for (int y = 0; y < size; y++)
    {
      row += ch;
    }

    res.push_back(row);
  }

  return res;
}

void LevelBuilder::LogPrintMapRaw()
{
  Logger::Instance().Print("***** MapRaw *****\n\n");

  std::string result = "\n";

  for (int x = 0; x < _mapSize.X; x++)
  {
    for (int y = 0; y < _mapSize.Y; y++)
    {
      result += MapRaw[x][y];
    }

    result += "\n";
  }

  Logger::Instance().Print(result);
}

void LevelBuilder::PrintMapRaw()
{
  printf("***** MapRaw *****\n\n");

  std::string result = "\n";

  for (int x = 0; x < _mapSize.X; x++)
  {
    for (int y = 0; y < _mapSize.Y; y++)
    {
      result += MapRaw[x][y];
    }

    result += "\n";
  }

  printf("%s\n", result.data());
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

void LevelBuilder::ConvertChunksToLayout()
{
  for (int x = 0; x < _mapSize.X; x++)
  {
    std::vector<char> row;
    for (int y = 0; y < _mapSize.Y; y++)
    {
      row.push_back('.');
    }

    MapLayout.push_back(row);
  }

  for (auto& chunk : MapChunks)
  {
    int x = chunk.UpperLeftCorner.X;
    int y = chunk.UpperLeftCorner.Y;

    for (auto& row : chunk.Layout)
    {
      for (int c = 0; c < row.length(); c++)
      {
        MapLayout[x][y + c] = row[c];
      }

      x++;
    }
  }
}

std::vector<std::vector<MapCell>> LevelBuilder::CreateEmptyMap(int w, int h)
{
  std::vector<std::vector<MapCell>> map;

  for (int x = 0; x < w; x++)
  {
    std::vector<MapCell> row;
    for (int y = 0; y < h; y++)
    {
      MapCell c;
      c.Coordinates.X = x;
      c.Coordinates.Y = y;
      c.Image = '.';
      c.Visited = false;
      row.push_back(c);
    }

    map.push_back(row);
  }

  return map;
}

std::vector<std::vector<MapCell>> LevelBuilder::CreateFilledMap(int w, int h)
{
  std::vector<std::vector<MapCell>> map;

  for (int x = 0; x < w; x++)
  {
    std::vector<MapCell> row;
    for (int y = 0; y < h; y++)
    {
      MapCell c;
      c.Coordinates.X = x;
      c.Coordinates.Y = y;
      c.Image = '#';
      c.Visited = false;
      row.push_back(c);
    }

    map.push_back(row);
  }

  return map;
}

std::vector<std::vector<MapCell>> LevelBuilder::CreateRandomlyFilledMap(int w, int h, int chance)
{
  std::vector<std::vector<MapCell>> map;

  for (int x = 0; x < w; x++)
  {
    std::vector<MapCell> row;
    for (int y = 0; y < h; y++)
    {
      bool isWall = Util::Rolld100(chance);

      MapCell c;
      c.Coordinates.X = x;
      c.Coordinates.Y = y;
      c.Image = isWall ? '#' : '.';
      c.Visited = false;
      row.push_back(c);
    }

    map.push_back(row);
  }

  return map;
}

std::vector<Position> LevelBuilder::GetRandomCell(Position p)
{
  std::vector<Position> res;

  std::map<RoomEdgeEnum, Position> offsetsBySide =
  {
    { RoomEdgeEnum::NORTH, { -1, 0 } },
    { RoomEdgeEnum::SOUTH, { 1, 0 }  },
    { RoomEdgeEnum::EAST,  { 0, 1 }  },
    { RoomEdgeEnum::WEST,  { 0, -1 } }
  };

  std::vector<Position> candidates;

  for (auto& kvp : offsetsBySide)
  {
    Position newPos;
    newPos.X = p.X + kvp.second.X;
    newPos.Y = p.Y + kvp.second.Y;

    if (IsInsideMap(newPos)
     && IsDeadEnd(newPos)
     && !_map[newPos.X][newPos.Y].Visited)
    {
      candidates.push_back(newPos);
    }
  }

  if (candidates.size() != 0)
  {
    int index = RNG::Instance().RandomRange(0, candidates.size());
    Position cp = candidates[index];
    _map[cp.X][cp.Y].Visited = true;
    _map[cp.X][cp.Y].Image = '.';
    res.push_back(cp);
  }

  return res;
}

// Cell is "valid" if it touches only one floor tile
bool LevelBuilder::IsDeadEnd(Position p)
{  
  int lx = p.X - 1;
  int ly = p.Y - 1;
  int hx = p.X + 1;
  int hy = p.Y + 1;

  int count = 0;

  if (_map[lx][p.Y].Image == '.') count++;
  if (_map[hx][p.Y].Image == '.') count++;
  if (_map[p.X][ly].Image == '.') count++;
  if (_map[p.X][hy].Image == '.') count++;

  return (count == 1);
}

void LevelBuilder::FillDeadEnds()
{
  for (int x = 0; x < _mapSize.X; x++)
  {
    for (int y = 0; y < _mapSize.Y; y++)
    {
      if (IsInsideMap({ x, y }) && IsDeadEnd({ x, y }))
      {
        _map[x][y].Image = '#';
      }
    }
  }
}

void LevelBuilder::CutProblemCorners()
{
  for (int x = 0; x < _mapSize.X; x++)
  {
    for (int y = 0; y < _mapSize.Y; y++)
    {
      Position p(x, y);
      CheckIfProblemCorner(p);
    }
  }
}

/// If we found situation like this:
///
/// #.#..#
/// #.#B.#
/// #.A#.#
/// #.....
/// #.....
///
/// we replace random wall around A (which is '.') with empty space
/// to disallow diagonal walking from A to B
void LevelBuilder::CheckIfProblemCorner(Position p)
{
  int lx = p.X - 1;
  int ly = p.Y - 1;
  int hx = p.X + 1;
  int hy = p.Y + 1;

  int count = 0;

  if (lx < 0 || ly < 0 || hx > _mapSize.X - 1 || hy > _mapSize.Y - 1)
  {
    return;
  }

  std::vector<Position> diagonals =
  {
    { lx, ly },
    { hx, ly },
    { lx, hy },
    { hx, hy }
  };

  std::vector<std::vector<Position>> choices =
  {
    { { -1,  0 },  { 0, -1 } },
    { {  1,  0 },  { 0, -1 } },
    { { -1,  0 },  { 0,  1 } },
    { {  1,  0 },  { 0,  1 } }
  };

  for (int i = 0; i < diagonals.size(); i++)
  {
    Position d = diagonals[i];

    int wallX1 = p.X + choices[i][0].X;
    int wallY1 = p.Y + choices[i][0].Y;
    int wallX2 = p.X + choices[i][1].X;
    int wallY2 = p.Y + choices[i][1].Y;

    bool hasWalls = (_map[wallX1][wallY1].Image == '#'
                  && _map[wallX2][wallY2].Image == '#');

    if (_map[p.X][p.Y].Image == '.'
     && _map[d.X][d.Y].Image == '.'
     && hasWalls)
    {
      int rndIndex = RNG::Instance().RandomRange(0, 2);
      Position choice = choices[i][rndIndex];
      _map[p.X + choice.X][p.Y + choice.Y].Image = '.';
      break;
    }
  }
}

void LevelBuilder::FillMapRaw()
{
  MapRaw.clear();

  for (int x = 0; x < _mapSize.X; x++)
  {
    std::vector<char> row;
    for (int y = 0; y < _mapSize.Y; y++)
    {
      row.push_back(_map[x][y].Image);
    }

    MapRaw.push_back(row);
  }
}

Position LevelBuilder::GetRandomPerpendicularDir(Position dir)
{
  Position res;

  std::vector<std::vector<Position>> choicesByDir =
  {
    { { 0, 1 }, { 0, -1 } },
    { { 0, 1 }, { 0, -1 } },
    { { 1, 0 }, { -1, 0 } },
    { { 1, 0 }, { -1, 0 } }
  };

  std::vector<Position> directions =
  {
    { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 }
  };

  for (int i = 0; i < directions.size(); i++)
  {
    if (dir.X == directions[i].X && dir.Y == directions[i].Y)
    {
      int index = RNG::Instance().RandomRange(0, 2);
      res = choicesByDir[i][index];
      break;
    }
  }

  return res;
}

std::vector<Position> LevelBuilder::GetRandomDir(Position pos)
{
  std::vector<Position> res;

  std::vector<Position> directions =
  {
    { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 }
  };

  while (!directions.empty())
  {
    int index = RNG::Instance().RandomRange(0, directions.size());
    Position newDir = directions[index];
    int nx = pos.X + newDir.X;
    int ny = pos.Y + newDir.Y;

    if (IsInsideMap({ nx, ny })
     && IsDeadEnd({ nx, ny }))
    {
      res.push_back(newDir);
      break;
    }

    directions.erase(directions.begin() + index);
  }

  return res;
}

std::vector<Position> LevelBuilder::TryToGetPerpendicularDir(Position pos, Position lastDir)
{
  std::vector<Position> res;

  std::vector<Position> directions =
  {
    { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 }
  };

  std::vector<std::vector<Position>> choicesByDir =
  {
    { { 0, 1 }, { 0, -1 } },
    { { 0, 1 }, { 0, -1 } },
    { { 1, 0 }, { -1, 0 } },
    { { 1, 0 }, { -1, 0 } }
  };

  std::vector<Position> selectedPair;

  //auto str = Util::StringFormat("Trying to get perpendicular dir to %i %i", lastDir.X, lastDir.Y);
  //Logger::Instance().Print(str);

  for (int i = 0; i < directions.size(); i++)
  {
    if (lastDir.X == directions[i].X && lastDir.Y == directions[i].Y)
    {
      selectedPair = choicesByDir[i];

      /*
      Logger::Instance().Print("\tFound pairs:");

      for (auto& i : selectedPair)
      {
        auto str = Util::StringFormat("\t%i %i", i.X, i.Y);
        Logger::Instance().Print(str);
      }
      */

      break;
    }
  }

  while (selectedPair.size() != 0)
  {
    int index = RNG::Instance().RandomRange(0, selectedPair.size());

    int x = pos.X + selectedPair[index].X;
    int y = pos.Y + selectedPair[index].Y;

    //auto str = Util::StringFormat("\t Checking %i %i...", x, y);
    //Logger::Instance().Print(str);

    //str = Util::StringFormat("\t Index %i", index);
    //Logger::Instance().Print(str);

    if (IsInsideMap({ x, y })
     && IsDeadEnd({ x, y })
     && !_map[x][y].Visited)
    {
      //auto str = Util::StringFormat("\t Selected dir %i %i", selectedPair[index].X, selectedPair[index].Y);
      //Logger::Instance().Print(str);

      res.push_back(selectedPair[index]);
      break;
    }

    selectedPair.erase(selectedPair.begin() + index);
  }

  return res;
}

void LevelBuilder::Reset()
{
  MapChunks.clear();
  MapLayout.clear();
  MapRaw.clear();

  _map.clear();
  _visitedCells.clear();
  _roomsForLevel.clear();

  while (!_rooms.empty())
  {
    _rooms.pop();
  }

  _roomsCount = 0;
}
