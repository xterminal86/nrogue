#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <string>
#include <map>
#include <memory>

#include <ncurses.h>

#include "tile.h"

#include "component.h"

class GameObject
{
public:
  GameObject(int x, int y, chtype avatar, const std::string& htmlColor);
  virtual ~GameObject() = default;

  bool Move(int dx, int dy);  

  void Draw();

  template <typename T>
  inline Component* AddComponent()
  {   
    // TODO: no check for component already added
    
    auto cp = std::make_unique<T>();    
            
    cp.get()->OwnerGameObject = this;

    // cp is null after std::move
    _components[typeid(T).hash_code()] = std::move(cp);
                
    return _components[typeid(T).hash_code()].get();
  }
    
  template <typename T>
  inline Component* GetComponent()
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

  void Update();

  int PosX;
  int PosY;

private:
  chtype _avatar;
  std::string _htmlColor;
  
  std::map<size_t, std::unique_ptr<Component>> _components;

  Tile* _previousCell = nullptr;
  Tile* _currentCell = nullptr;
};

#endif
