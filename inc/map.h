#ifndef MAP_H
#define MAP_H

#include <algorithm>
#include <random>
#include <chrono>

#include "singleton.h"
#include "rect.h"
#include "constants.h"
#include "tile.h"

class Map : public Singleton<Map>
{
public:  
  void Init() override
  {    
  }
  
  void CreateMap();

  void CreateRoom(Rect& r);
  
  void CreateTunnelH(int x1, int x2, int y);
  void CreateTunnelV(int y1, int y2, int x);  

  void Draw(int playerX, int playerY);

  Tile MapArray[GlobalConstants::MapX][GlobalConstants::MapY];

  int PlayerStartX;
  int PlayerStartY;

  int MapOffsetX;
  int MapOffsetY;
};

#endif
