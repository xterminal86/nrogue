#include "game-object.h"

#include "map.h"
#include "printer.h"

void GameObject::Init(int x, int y, chtype avatar, const std::string& fgColor, const std::string& bgColor)
{
  PosX = x;
  PosY = y;
  Image = avatar;
  FgColor = fgColor;
  BgColor = bgColor;

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

void GameObject::Draw(const std::string& overrideColorFg, const std::string& overrideColorBg)
{  
  Printer::Instance().PrintFB(PosX + Map::Instance().MapOffsetX,
                              PosY + Map::Instance().MapOffsetY,
                              Image,
                              (overrideColorFg.length() == 0) ? FgColor : overrideColorFg,
                              (overrideColorBg.length() == 0) ? BgColor : overrideColorBg);
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
  FgColor = GlobalConstants::FloorColor;
  ObjectName = "Floor";
}

void GameObject::CreateWall()
{
  Blocking = true;
  BlockSight = true;
  Image = '#'; // ACS_BLOCK;
  FgColor = GlobalConstants::WallColor;
  ObjectName = "Wall";  
}

void GameObject::CreateTree()
{
  Blocking = false;
  BlockSight = true;
  Image = 'T';
  FgColor = GlobalConstants::TreeColor;
  ObjectName = "Tree";
}

void GameObject::CreateMountain()
{
  Blocking = true;
  BlockSight = true;
  Image = ' ';
  FgColor = "";
  //FgColor = GlobalConstants::WallColor;
  BgColor = "#444444";
  ObjectName = "Mountains";
}
