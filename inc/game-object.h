#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "map.h"
#include "printer.h"
#include "util.h"
#include "component.h"
#include "logger.h"

class GameObject
{
public:
  GameObject(int x, int y, chtype avatar, const std::string& htmlColor)
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
    Printer::Instance().Print(_posX + Map::Instance().MapOffsetX, _posY + Map::Instance().MapOffsetY, _avatar, _htmlColor);
  }

  void Clear()
  {    
    Printer::Instance().Print(_posX + Map::Instance().MapOffsetX, _posY + Map::Instance().MapOffsetY, ' ', "#000000");
  }
  
  template <typename T>
  Component* AddComponent()
  {   
    // TODO: no check for component already added
    
    auto cp = std::make_unique<T>();    
            
    // cp is null after std::move
    _components[typeid(T).hash_code()] = std::move(cp);
                
    return _components[typeid(T).hash_code()].get();
  }
    
  template <typename T>
  Component* GetComponent()
  {
    for (auto& c : _components)
    {
      if (c.second.get()->Hash() == typeid(T).hash_code())
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
  chtype _avatar;
  std::string _htmlColor;
  
  std::map<size_t, std::unique_ptr<Component>> _components;

  void DiscoverCell(int x, int y)
  {
    auto map = Map::Instance().MapArray;
  
    map[x][y].Visible = true;

    if (!map[x][y].Revealed)
    {
      map[x][y].Revealed = true;
    }               
  }
};

#endif
