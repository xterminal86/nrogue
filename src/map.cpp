#include "map.h"

void Map::CreateMap()
{  
  for (int x = 0; x < GlobalConstants::MapX; x++)
  {
    for (int y = 0; y < GlobalConstants::MapY; y++)
    {
      MapArray[x][y].Init(false);

      MapArray[x][y].SetFloor();      
    }
  }

  for (int x = 0; x < GlobalConstants::MapX; x++)
  {
    MapArray[x][0].SetWall();
    MapArray[x][GlobalConstants::MapY - 1].SetWall();    
  }

  for (int y = 0; y < GlobalConstants::MapY; y++)
  {
    MapArray[0][y].SetWall();
    MapArray[GlobalConstants::MapX - 1][y].SetWall();    
  }

  PlayerStartX = 1;
  PlayerStartY = 1;

  /*
  std::vector<Rect> rooms;

  int numRooms = 0;

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

  std::mt19937_64 g1(seed);

  for (int i = 0; i < GlobalConstants::MaxRooms; i++)
  { 
    int w = (g1() % GlobalConstants::RoomMaxSize) + GlobalConstants::RoomMinSize;
    int h = (g1() % GlobalConstants::RoomMaxSize) + GlobalConstants::RoomMinSize;
    int x = g1() % (GlobalConstants::MapX - 2);
    int y = g1() % (GlobalConstants::MapY - 2);

    if (w > GlobalConstants::RoomMaxSize)
    {
      w = GlobalConstants::RoomMaxSize;
    }

    if (h > GlobalConstants::RoomMaxSize)
    {
      h = GlobalConstants::RoomMaxSize;
    }

    Rect room(x, y, w, h);

    bool failed = false;

    for (auto& r : rooms)
    {
      if (r.Intersects(room))
      {
        failed = true;
        break;
      }
    }

    if (!failed)
    {
      CreateRoom(room);
      auto center = room.Center();

      if (numRooms == 0)
      {
        PlayerStartX = center.first;
        PlayerStartY = center.second;

        //mvprintw(30, 30, "%i %i", PlayerStartX, PlayerStartY);  
      }
      else
      {
        auto prev = rooms[numRooms - 1].Center();
        
        int coin = g1() % 2;

        if (coin == 0)
        {
          CreateTunnelH(prev.first, center.first, prev.second);
          CreateTunnelV(prev.second, center.second, prev.first);
        }
        else
        {
          CreateTunnelV(prev.second, center.second, prev.first);
          CreateTunnelH(prev.first, center.first, prev.second);
        }
      }

      rooms.push_back(room);

      numRooms++;
    }
  }
  */
}

void Map::CreateRoom(Rect& r)
{
  for (int x = r.X1 + 1; x < r.X2; x++)
  {
    for (int y = r.Y1 + 1; y < r.Y2; y++)
    {
      MapArray[x][y].Blocking = false;
      MapArray[x][y].BlockSight = false;
    }
  }
}

void Map::CreateTunnelH(int x1, int x2, int y)
{
  int min = std::min(x1, x2);
  int max = std::max(x1, x2);

  if (min > max)
  {
    int tmp = min;
    min = max;
    max = tmp;
  }

  for (int x = min; x <= max; x++)
  {
    MapArray[x][y].Blocking = false;
    MapArray[x][y].BlockSight = false;
  }
}

void Map::CreateTunnelV(int y1, int y2, int x)
{
  int min = std::min(y1, y2);
  int max = std::max(y1, y2);

  if (min > max)
  {
    int tmp = min;
    min = max;
    max = tmp;
  }

  for (int y = min; y <= max; y++)
  {
    MapArray[x][y].Blocking = false;
    MapArray[x][y].BlockSight = false;
  }
}

void Map::Draw(int playerX, int playerY)
{
  int tw = Printer::Instance().TerminalWidth;
  int th = Printer::Instance().TerminalHeight;
    
  auto mapCells = Util::GetRectAroundPoint(playerX, playerY, tw / 2, th / 2);
  for (auto& cell : mapCells)
  {
    int x = cell.X;
    int y = cell.Y;
    
    if (MapArray[x][y].Visible)
      {        
        Printer::Instance().Print(x + MapOffsetX, y + MapOffsetY, 
                                  MapArray[x][y].Image, 
                                  Printer::kAlignLeft, 
                                  MapArray[x][y].Color);      
      }
      else
      {
        auto tileColor = MapArray[x][y].Revealed ? "#222222" : "#000000";

        Printer::Instance().Print(x + MapOffsetX, y + MapOffsetY, 
                                  MapArray[x][y].Image, 
                                  Printer::kAlignLeft, 
                                  tileColor);      
      }
  }
}
