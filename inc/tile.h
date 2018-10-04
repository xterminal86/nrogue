#ifndef TILE_H
#define TILE_H

#include <string>
#include <memory>

#include <ncurses.h>

class Tile
{
  public:
    bool Blocking;
    bool BlockSight;
    bool Revealed;
    bool Visible = false;
    std::string Color;
    chtype Image;

    void Init(bool blocking, bool blockSight = false, bool revealed = false, std::string color = "#000000")
    {
      Blocking = blocking;
      BlockSight = blocking ? true : blockSight;
      Revealed = revealed;
      Color = color;
    }

    void SetWall()
    {
      Blocking = true;
      BlockSight = true;
      Image = '#'; // ACS_BLOCK;
      Color = "#888888";
    }

    void SetFloor()
    {
      Blocking = false;
      BlockSight = false;
      Image = '.';
      Color = "#444444";
    }
};

#endif
