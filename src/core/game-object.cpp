#include "game-object.h"

#include "printer.h"
#include "game-objects-factory.h"
#include "map-level-base.h"
#include "map.h"
#include "application.h"

GameObject::GameObject(MapLevelBase* levelOwner)
{
  _levelOwner = levelOwner;
}

void GameObject::Init(MapLevelBase* levelOwner, int x, int y, int avatar, const std::string& fgColor, const std::string& bgColor)
{
  PosX = x;
  PosY = y;
  Image = avatar;
  FgColor = fgColor;
  BgColor = bgColor;

  Attrs.ActionMeter = GlobalConstants::TurnReadyValue;

  _levelOwner = levelOwner;

  // _currentCell->Occupied is not set to true by default,
  // see game-object.h comments for Occupied field.
  _currentCell = _levelOwner->MapArray[PosX][PosY].get();
  _previousCell = _levelOwner->MapArray[PosX][PosY].get();
}

bool GameObject::Move(int dx, int dy)
{
  int nx = PosX + dx;
  int ny = PosY + dy;

  bool isBlocked = Map::Instance().CurrentLevel->IsCellBlocking({ nx, ny });  
  bool isOccupied = Map::Instance().CurrentLevel->MapArray[nx][ny]->Occupied;
  if (!isBlocked && !isOccupied)
  {
    MoveGameObject(dx, dy);
    return true;
  }
  
  return false;
}

bool GameObject::MoveTo(int x, int y)
{
  bool isBlocked = Map::Instance().CurrentLevel->IsCellBlocking({ x, y });
  if (!isBlocked)
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

void GameObject::MakeTile(GameObjectInfo t)
{
  Blocking = t.IsBlocking;
  BlocksSight = t.BlocksSight;
  Image = t.Image;
  FgColor = t.FgColor;
  BgColor = t.BgColor;
  ObjectName = t.ObjectName;
  FogOfWarName = t.FogOfWarName;
  Type = GameObjectType::GROUND;
}

void GameObject::ReceiveDamage(GameObject* from, int amount, bool isMagical)
{  
  if (!Attrs.Indestructible)
  {
    Attrs.HP.CurrentValue -= amount;

    auto str = Util::StringFormat("%s was hit for %i damage", ObjectName.data(), amount);
    Printer::Instance().AddMessage(str);

    if (Attrs.HP.CurrentValue <= 0)
    {
      if (Type != GameObjectType::HARMLESS)
      {
        auto go = GameObjectsFactory::Instance().CreateRemains(this);
        _levelOwner->InsertGameObject(go);
      }

      IsDestroyed = true;

      std::string verb = (Type == GameObjectType::HARMLESS)
                       ? "destroyed"
                       : "killed";

      auto msg = Util::StringFormat("%s was %s", ObjectName.data(), verb.data());
      Printer::Instance().AddMessage(msg);
    }
  }
  else
  {    
    if (Type != GameObjectType::GROUND)
    {
      auto str = Util::StringFormat("%s not even scratched!", ObjectName.data());
      Printer::Instance().AddMessage(str);
    }
  }
}

void GameObject::WaitForTurn()
{
  int speedTickBase = GlobalConstants::TurnTickValue;

  int speedAttr = Attrs.Spd.Get();

  int playerSpd = Application::Instance().PlayerInstance.Attrs.Spd.Get();

  int minSpdIncr = GlobalConstants::TurnTickValue / 10;
  int scale = (speedAttr - playerSpd) * GlobalConstants::TurnTickValue;
  int speedIncrement = speedTickBase + speedAttr * scale;

  // In impossible case that speed penalties
  // are too great that speed increment is negative
  speedIncrement = (speedIncrement <= 0) ? minSpdIncr : speedIncrement;

  // In towns SPD is ignored
  if (Map::Instance().CurrentLevel->Peaceful)
  {
    Attrs.ActionMeter = GlobalConstants::TurnReadyValue - 1;
  }  

  Attrs.ActionMeter += speedIncrement;
}

void GameObject::FinishTurn()
{
  Attrs.ActionMeter -= GlobalConstants::TurnReadyValue;

  if (Attrs.ActionMeter < 0)
  {
    Attrs.ActionMeter = 0;
  }

  _healthRegenTurnsCounter++;

  if (_healthRegenTurnsCounter >= HealthRegenTurns)
  {
    _healthRegenTurnsCounter = 0;
    Attrs.HP.Add(1);
  }
}

void GameObject::MoveGameObject(int dx, int dy)
{
  _previousCell = Map::Instance().CurrentLevel->MapArray[PosX][PosY].get();
  _previousCell->Occupied = false;

  PosX += dx;
  PosY += dy;

  _currentCell = Map::Instance().CurrentLevel->MapArray[PosX][PosY].get();
  _currentCell->Occupied = true;
}
