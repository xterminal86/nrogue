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
  if (!Map::Instance().MapArray[PosX + dx][PosY + dy].Occupied &&
      !Map::Instance().MapArray[PosX + dx][PosY + dy].Blocking)
  {
    _previousCell = &Map::Instance().MapArray[PosX][PosY];
    _previousCell->Occupied = false;

    PosX += dx;
    PosY += dy;
    
    _currentCell = &Map::Instance().MapArray[PosX][PosY];
    _currentCell->Occupied = true;

    return true;
  }
  
  return false;
}

void GameObject::Draw(const std::string& overrideColor)
{  
  Printer::Instance().PrintFB(PosX + Map::Instance().MapOffsetX,
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
  HtmlColor = GlobalConstants::FloorColor;
  ObjectName = "Floor";
}

void GameObject::CreateWall()
{
  Blocking = true;
  BlockSight = true;
  Image = '#'; // ACS_BLOCK;
  HtmlColor = GlobalConstants::WallColor;
  ObjectName = "Wall";  
}

void GameObject::CreateTree()
{
  Blocking = false;
  BlockSight = true;
  Image = 'T';
  HtmlColor = GlobalConstants::TreeColor;
  ObjectName = "Tree";
}

void GameObject::CreateMountain()
{
  Blocking = true;
  BlockSight = true;
  Image = 'M';
  HtmlColor = GlobalConstants::WallColor;
  ObjectName = "Mountains";
}
