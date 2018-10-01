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

  for (int x = 10; x < 40; x++)
  {
    MapArray[x][10].SetWall();
  }
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
                                  MapArray[x][y].Color);      
      }
      else
      {
        auto tileColor = MapArray[x][y].Revealed ? "#222222" : "#000000";

        Printer::Instance().Print(x + MapOffsetX, y + MapOffsetY, 
                                  MapArray[x][y].Image,
                                  tileColor);      
      }
  }
}
