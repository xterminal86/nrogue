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

  void Draw(int playerX, int playerY);

  Tile MapArray[GlobalConstants::MapX][GlobalConstants::MapY];

  int PlayerStartX;
  int PlayerStartY;

  int MapOffsetX;
  int MapOffsetY;
};

#endif
