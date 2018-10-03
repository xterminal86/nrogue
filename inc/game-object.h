#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <string>
#include <map>
#include <memory>

#include <ncurses.h>

#include "component.h"

class GameObject
{
public:
  GameObject(int x, int y, chtype avatar, const std::string& htmlColor);

  bool Move(int dx, int dy);  

  void Draw();

  template <typename T>
  Component* AddComponent()
  {   
    // TODO: no check for component already added
    
    auto cp = std::make_unique<T>();    
            
    cp.get()->Owner = this;

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
  
  void Update();

private:
  int _posX;
  int _posY;
  chtype _avatar;
  std::string _htmlColor;
  
  std::map<size_t, std::unique_ptr<Component>> _components;  
};

#endif
