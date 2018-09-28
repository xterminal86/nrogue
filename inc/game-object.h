#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "map.h"
#include "printer.h"
#include "util.h"

class GameObject
{
public:
  GameObject(int x, int y, char avatar, const std::string& htmlColor)
  {
    _posX = x;
    _posY = y;
    _avatar = avatar;
    _htmlColor = htmlColor;
  }

  bool Move(int dx, int dy)
  {
    if (!Map::Instance().MapArray[_posX + dx][_posY + dy].Blocking)
    {
      _posX += dx;
      _posY += dy;
      
      return true;
    }
    
    return false;
  }

  void Draw()
  {    
    std::string ch = { _avatar };
    Printer::Instance().Print(_posX + Map::Instance().MapOffsetX, _posY + Map::Instance().MapOffsetY, ch, Printer::kAlignLeft, _htmlColor);
  }

  void Clear()
  {
    std::string eraser = " ";
    Printer::Instance().Print(_posX + Map::Instance().MapOffsetX, _posY + Map::Instance().MapOffsetY, eraser, Printer::kAlignLeft, "#000000");
  }

  int PosX() { return _posX; }
  int PosY() { return _posY; }

  int VisibilityRadius = 1;

  void CheckVisibility()
  {
    auto map = Map::Instance().MapArray;

    for (int x = 0; x < GlobalConstants::MapX; x++)
    {
      for (int y = 0; y < GlobalConstants::MapY; y++)
      {
        map[x][y].Visible = false;        
      }
    }

    int lx = _posX - VisibilityRadius;
    int ly = _posY - VisibilityRadius;
    int hx = _posX + VisibilityRadius;
    int hy = _posY + VisibilityRadius;

    lx = Util::Clamp(lx, 0, GlobalConstants::MapX - 1);
    ly = Util::Clamp(ly, 0, GlobalConstants::MapY - 1);
    hx = Util::Clamp(hx, 0, GlobalConstants::MapX - 1);
    hy = Util::Clamp(hy, 0, GlobalConstants::MapY - 1);

    for (int x = lx; x <= hx; x++)
    {
      for (int y = ly; y <= hy; y++)
      {
        map[x][y].Visible = true; 

        if (!map[x][y].Revealed)
        {
          map[x][y].Revealed = true;
        }
      }
    }    
  }

private:
  int _posX;
  int _posY;
  char _avatar;
  std::string _htmlColor;
};

#endif