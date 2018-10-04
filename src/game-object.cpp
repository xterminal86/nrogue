#include "game-object.h"

#include "map.h"
#include "printer.h"

GameObject::GameObject(int x, int y, chtype avatar, const std::string& htmlColor)
{
  PosX = x;
  PosY = y;
  _avatar = avatar;
  _htmlColor = htmlColor;

  _currentCell = &Map::Instance().MapArray[PosX][PosY];
}


bool GameObject::Move(int dx, int dy)
{
  if (!Map::Instance().MapArray[PosX + dx][PosY + dy].Blocking)
  {
    _previousCell = &Map::Instance().MapArray[PosX][PosY];

    PosX += dx;
    PosY += dy;
    
    _currentCell = &Map::Instance().MapArray[PosX][PosY];

    return true;
  }
  
  return false;
}

void GameObject::Draw()
{
  Printer::Instance().Print(PosX + Map::Instance().MapOffsetX, PosY + Map::Instance().MapOffsetY, _avatar, _htmlColor);
}

void GameObject::Update()
{
  for (auto& c : _components)
  {
    c.second.get()->Update();
  }
}
