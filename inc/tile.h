#ifndef TILE_H
#define TILE_H

class Tile
{
public:
  bool Blocking;
  bool BlockSight;
  
  void Init(bool blocking, bool blockSight = false)
  {
    Blocking = blocking;
    BlockSight = blocking ? true : blockSight;
  }  
};

#endif