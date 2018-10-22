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

bool GameObject::MoveTo(int x, int y)
{
  if (!Map::Instance().MapArray[x][y].Occupied &&
      !Map::Instance().MapArray[x][y].Blocking)
  {
    _previousCell = &Map::Instance().MapArray[PosX][PosY];
    _previousCell->Occupied = false;

    PosX = x;
    PosY = y;

    _currentCell = &Map::Instance().MapArray[PosX][PosY];
    _currentCell->Occupied = true;

    return true;
  }

  return false;
}

// To allow fog of war to cover objects made of
// background colored ' ', make sure FgColor
// is set to empty string ( i.e. "" )
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

void GameObject::MakeTile(Tile t)
{
  Blocking = t.IsBlocking;
  BlocksSight = t.BlocksSight;
  Image = t.Image;
  FgColor = t.FgColor;
  BgColor = t.BgColor;
  ObjectName = t.ObjectName;
  FogOfWarName = t.FogOfWarName;
}

void GameObject::ReceiveDamage(int amount)
{
  if (!Attrs.Indestructible)
  {
    Attrs.HP.CurrentValue -= amount;

    if (Attrs.HP.CurrentValue <= 0)
    {
      IsDestroyed = true;
    }
  }
}
