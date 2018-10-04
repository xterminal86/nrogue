#include "map.h"

#include "printer.h"
#include "util.h"

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
