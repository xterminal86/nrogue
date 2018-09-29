#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "map.h"
#include "printer.h"
#include "util.h"
#include "component.h"

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
  
  void AddComponent(Component* c)
  {    
    _components[c->Hash()].reset(c);
  }
    
  const Component* GetComponent(size_t hash)
  {
    for (auto& c : _components)
    {
      if (c.second.get()->Hash() == hash)
      {
        return c.second.get();
      }
    }
    
    return nullptr;
  }
  
  int PosX() { return _posX; }
  int PosY() { return _posY; }

  int VisibilityRadius = 1;
  
private:
  int _posX;
  int _posY;
  char _avatar;
  std::string _htmlColor;
  
  std::map<size_t, std::unique_ptr<Component>> _components;
};

#endif
