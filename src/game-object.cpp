#include "game-object.h"

#include "map.h"
#include "printer.h"

void GameObject::Init(int x, int y, chtype avatar, const std::string& htmlColor)
{
  PosX = x;
  PosY = y;
  Image = avatar;
  HtmlColor = htmlColor;

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

void GameObject::Draw(const std::string& overrideColor)
{  
  Printer::Instance().Print(PosX + Map::Instance().MapOffsetX,
                              PosY + Map::Instance().MapOffsetY,
                              Image,
                              (overrideColor.length() == 0) ? HtmlColor : overrideColor);
}

void GameObject::Update()
{
  for (auto& c : _components)
  {
    c.second.get()->Update();
  }
}

void GameObject::CreateFloor()
{
  Blocking = false;
  BlockSight = false;
  Image = '.';
  HtmlColor = "#444444";
}

void GameObject::CreateWall()
{
  Blocking = true;
  BlockSight = true;
  Image = '#'; // ACS_BLOCK;
  HtmlColor = "#888888";
}
