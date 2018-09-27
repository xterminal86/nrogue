#ifndef TILE_H
#define TILE_H

class Tile
{
public:
  Tile() {}

  void Init(bool blocking, bool blockSight = false)
  {
    _blocking = blocking;
    _blockSight = blocking ? true : blockSight;
  }

  void Blocking(bool value)
  {
    _blocking = value;
  }

  void BlockSight(bool value)
  {
    _blockSight = value;
  }

  bool IsBlocking() { return _blocking; }
  bool IsBlockSight() { return _blockSight; }

private:
  bool _blocking;
  bool _blockSight;
};

#endif