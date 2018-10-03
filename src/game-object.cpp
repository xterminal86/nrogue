#include "game-object.h"

#include "map.h"
#include "printer.h"

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

void GameObject::Draw()
{
  Printer::Instance().Print(_posX + Map::Instance().MapOffsetX, _posY + Map::Instance().MapOffsetY, _avatar, _htmlColor);
}

void GameObject::Update()
{
  for (auto& c : _components)
  {
    c.second.get()->Update();
  }
}
