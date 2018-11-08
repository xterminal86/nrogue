#include "game-object.h"

#include "printer.h"
#include "game-objects-factory.h"
#include "map-level-base.h"

GameObject::GameObject(MapLevelBase* levelOwner)
{
  _levelOwner = levelOwner;
}

void GameObject::Init(MapLevelBase* levelOwner, int x, int y, chtype avatar, const std::string& fgColor, const std::string& bgColor)
{
  PosX = x;
  PosY = y;
  Image = avatar;
  FgColor = fgColor;
  BgColor = bgColor;

  Attrs.ActionMeter = 100;

  _levelOwner = levelOwner;

  // _currentCell->Occupied is not set to true by default,
  // see game-object.h comments for Occupied field.
  _currentCell = _levelOwner->MapArray[PosX][PosY].get();
}

bool GameObject::Move(int dx, int dy)
{
  if (!_levelOwner->MapArray[PosX + dx][PosY + dy]->Occupied &&
      !_levelOwner->MapArray[PosX + dx][PosY + dy]->Blocking)
  {
    _previousCell = _levelOwner->MapArray[PosX][PosY].get();
    _previousCell->Occupied = false;

    PosX += dx;
    PosY += dy;
    
    _currentCell = _levelOwner->MapArray[PosX][PosY].get();
    _currentCell->Occupied = true;

    return true;
  }
  
  return false;
}

bool GameObject::MoveTo(int x, int y)
{
  if (!_levelOwner->MapArray[x][y]->Occupied &&
      !_levelOwner->MapArray[x][y]->Blocking)
  {
    // When we change level, previous position (PosX and PosY)
    // is pointing to the stairs down on previous level,
    // which may not have the same dimensions as the level
    // we just arrived in. So to avoid going outside array bounds,
    // we have to perform a check.
    //
    // Unblocking of stairs in such case is done directly in ChangeLevel(),
    // so we just skip this case here.

    if (PosX < _levelOwner->MapSize.X && PosY < _levelOwner->MapSize.Y)
    {
      _previousCell = _levelOwner->MapArray[PosX][PosY].get();
      _previousCell->Occupied = false;
    }

    PosX = x;
    PosY = y;

    _currentCell = _levelOwner->MapArray[PosX][PosY].get();
    _currentCell->Occupied = true;

    return true;
  }

  return false;
}

void GameObject::Draw(const std::string& overrideColorFg, const std::string& overrideColorBg)
{  
  Printer::Instance().PrintFB(PosX + _levelOwner->MapOffsetX,
                              PosY + _levelOwner->MapOffsetY,
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

void GameObject::ReceiveDamage(GameObject* from, int amount)
{
  if (!Attrs.Indestructible)
  {
    Attrs.HP.CurrentValue -= amount;

    auto str = Util::StringFormat("%s was hit for %i damage", ObjectName.data(), amount);
    Printer::Instance().AddMessage(str);

    if (Attrs.HP.CurrentValue <= 0)
    {
      auto go = GameObjectsFactory::Instance().CreateRemains(this);
      _levelOwner->InsertGameObject(go);

      IsDestroyed = true;

      auto msg = Util::StringFormat("%s was killed", ObjectName.data());
      Printer::Instance().AddMessage(msg);
    }
  }
}

void GameObject::WaitForTurn()
{
  int amount = (Attrs.Spd.CurrentValue + 1) * 10;
  Attrs.ActionMeter += amount;
}

void GameObject::FinishTurn()
{
  Attrs.ActionMeter -= 100;

  if (Attrs.ActionMeter < 0)
  {
    Attrs.ActionMeter = 0;
  }
}
