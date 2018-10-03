#include "game-object.h"

GameObject::GameObject(int x, int y, chtype avatar, const std::string& htmlColor)
{
  _posX = x;
  _posY = y;
  _avatar = avatar;
  _htmlColor = htmlColor;
}


bool GameObject::Move(int dx, int dy)
{
  if (!Map::Instance().MapArray[_posX + dx][_posY + dy].Blocking)
  {
    _posX += dx;
    _posY += dy;
    
    return true;
  }
  
  return false;
}

void GameObject::Update()
{
  for (auto& c : _components)
  {
    c.second.get()->Update();
  }
}
