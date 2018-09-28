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

  bool Move(int dx, int dy);  
  void CheckVisibility();  

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
  
private:
  int _posX;
  int _posY;
  char _avatar;
  std::string _htmlColor;
};

#endif
