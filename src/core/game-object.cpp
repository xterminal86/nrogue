#include "game-object.h"

#include "printer.h"
#include "game-objects-factory.h"
#include "map-level-base.h"
#include "map.h"
#include "application.h"
#include "game-object-info.h"

GameObject::GameObject(MapLevelBase* levelOwner)
{
  _levelOwner = levelOwner;
  VisibilityRadius.Set(0);
}

GameObject::GameObject(MapLevelBase *levelOwner,
                       int x,
                       int y,
                       int avatar,
                       const std::string &htmlColor,
                       const std::string &bgColor)
{
  Init(levelOwner, x, y, avatar, htmlColor, bgColor);
}

void GameObject::Init(MapLevelBase* levelOwner,
                      int x,
                      int y,
                      int avatar,
                      const std::string& fgColor,
                      const std::string& bgColor)
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
  bool isOccupied = Map::Instance().CurrentLevel->MapArray[x][y]->Occupied;
  if (!isBlocked && !isOccupied)
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

bool GameObject::MoveTo(const Position &pos)
{
  return MoveTo(pos.X, pos.Y);
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

void GameObject::MakeTile(const GameObjectInfo& t)
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
  // TODO: isMagical for enemies' armor damage

  if (!Attrs.Indestructible)
  {
    Attrs.HP.CurrentValue -= amount;

    auto str = Util::StringFormat("%s was hit for %i damage", ObjectName.data(), amount);
    Printer::Instance().AddMessage(str);

    if (!IsAlive())
    {
      MarkAndCreateRemains();

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
  // In order to enhance the difference between two GameObjects
  // with slightly different stats (SPD 1 and SPD 2)
  // and to reduce number of idle waiting cycles,
  // we introduce a little scaling for speed incrementation.
  int spdIncrScale = GlobalConstants::TurnTickValue / 4;

  int speedTickBase = GlobalConstants::TurnTickValue;
  int speedAttr = Attrs.Spd.Get() * spdIncrScale;
  int speedIncrement = speedTickBase + speedAttr;

  // In impossible case that speed penalties
  // are too great that speed increment is negative
  speedIncrement = (speedIncrement <= 0) ? spdIncrScale : speedIncrement;

  // In towns SPD is ignored
  if (Map::Instance().CurrentLevel->Peaceful)
  {
    speedIncrement = 1;
    Attrs.ActionMeter = GlobalConstants::TurnReadyValue - 1;
  }

  Attrs.ActionMeter += speedIncrement;
}

bool GameObject::IsAlive()
{
  return (Attrs.HP.CurrentValue > 0);
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

  ProcessEffects();

  if (!IsAlive())
  {
    MarkAndCreateRemains();
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

void GameObject::AddEffect(const Effect &effectToAdd)
{
  if (HasEffect(effectToAdd.Type))
  {
    if (effectToAdd.Cumulative)
    {
      _activeEffects[effectToAdd.Type].Power += effectToAdd.Power;
      _activeEffects[effectToAdd.Type].Duration += effectToAdd.Duration;
    }
    else
    {
      _activeEffects[effectToAdd.Type].Power = effectToAdd.Power;
      _activeEffects[effectToAdd.Type].Duration = effectToAdd.Duration;
    }

    ApplyEffect(_activeEffects[effectToAdd.Type]);
  }
  else
  {
    ApplyEffect(effectToAdd);
    _activeEffects[effectToAdd.Type] = effectToAdd;
  }
}

void GameObject::AddEffect(EffectType type,
                           int power,
                           int duration,
                           bool cumulative,
                           bool ignoresArmor)
{
  if (HasEffect(type))
  {
    if (cumulative)
    {
      _activeEffects[type].Power += power;
      _activeEffects[type].Duration += duration;
    }
    else
    {
      _activeEffects[type].Power = power;
      _activeEffects[type].Duration = duration;
    }

    ApplyEffect(_activeEffects[type]);
  }
  else
  {
    Effect e = { type, power, duration, cumulative, ignoresArmor };
    ApplyEffect(e);
    _activeEffects[type] = e;
  }
}

void GameObject::ApplyEffect(const Effect& e)
{
  switch (e.Type)
  {
    case EffectType::ILLUMINATED:
    {
      VisibilityRadius.Modifier = e.Power;
    }
    break;
  }
}

void GameObject::UnapplyEffect(const Effect& e)
{
  switch (e.Type)
  {
    case EffectType::ILLUMINATED:
    {
      VisibilityRadius.Modifier = 0;
    }
    break;
  }
}

void GameObject::RemoveEffect(EffectType t)
{  
  if (HasEffect(t))
  {
    UnapplyEffect(_activeEffects[t]);
    _activeEffects.erase(t);
  }
}

bool GameObject::HasEffect(EffectType t)
{  
  return (_activeEffects.count(t) != 0);
}

void GameObject::ProcessEffects()
{
  for (int i = 0; i < _activeEffects.size(); i++)
  {
    auto it = _activeEffects.begin();
    std::advance(it, i);

    if (it->second.Duration > 0)
    {
      EffectAction(it->second);
      it->second.Duration--;
    }
    else if (it->second.Duration == 0)
    {
      UnapplyEffect(it->second);
      _activeEffects.erase(it);
    }
    else if (it->second.Duration == -1)
    {
      EffectAction(it->second);
    }
  }
}

void GameObject::EffectAction(const Effect& e)
{
  switch (e.Type)
  {
    case EffectType::POISONED:
      Attrs.HP.Add(-e.Power);
      break;

    case EffectType::REGEN:
      Attrs.HP.Add(e.Power);
      break;
  }
}

void GameObject::MarkAndCreateRemains()
{
  if (Type != GameObjectType::HARMLESS)
  {
    auto go = GameObjectsFactory::Instance().CreateRemains(this);
    _levelOwner->InsertGameObject(go);
  }

  IsDestroyed = true;
}

bool GameObject::Interact()
{
  // http://www.cplusplus.com/reference/functional/function/target_type/
  //
  // Return value
  // The type_info object that corresponds to the type of the target,
  // or typeid(void) if the object is an empty function.

  if (InteractionCallback.target_type() != typeid(void))
  {
    InteractionCallback();

    return true;
  }

  return false;
}

void GameObject::SetLevelOwner(MapLevelBase *levelOwner)
{
  _levelOwner = levelOwner;
}

size_t GameObject::ComponentsSize()
{
  return _components.size();
}

void GameObject::LevelUp(int baseHpOverride)
{
  std::map<int, std::pair<std::string, Attribute&>> mainAttributes =
  {
    { 0, { "STR", Attrs.Str } },
    { 1, { "DEF", Attrs.Def } },
    { 2, { "MAG", Attrs.Mag } },
    { 3, { "RES", Attrs.Res } },
    { 4, { "SKL", Attrs.Skl } },
    { 5, { "SPD", Attrs.Spd } }
  };

  for (auto& i : mainAttributes)
  {
    auto kvp = i.second;

    if (CanRaiseAttribute(kvp.second))
    {
      kvp.second.OriginalValue++;
      kvp.second.CurrentValue = kvp.second.OriginalValue;
    }
  }

  // HP and MP

  int baseHp = (baseHpOverride == -1) ? Attrs.HP.Talents : baseHpOverride;
  int minRndHp = (baseHp + 1);
  int maxRndHp = 2 * (baseHp + 1);

  int hpToAdd = RNG::Instance().RandomRange(minRndHp, maxRndHp);
  Attrs.HP.OriginalValue += hpToAdd;
  Attrs.HP.CurrentValue = Attrs.HP.OriginalValue;

  int minRndMp = Attrs.Mag.OriginalValue;
  int maxRndMp = Attrs.Mag.OriginalValue + Attrs.MP.Talents;

  int mpToAdd = RNG::Instance().RandomRange(minRndMp, maxRndMp);
  Attrs.MP.OriginalValue += mpToAdd;
  Attrs.MP.CurrentValue = Attrs.MP.OriginalValue;

  Attrs.Lvl.OriginalValue++;
  Attrs.Lvl.CurrentValue = Attrs.Lvl.OriginalValue;
}

bool GameObject::CanRaiseAttribute(Attribute& attr)
{
  int chance = GlobalConstants::AttributeMinimumRaiseChance;

  int iterations = attr.Talents;
  for (int i = 0; i < iterations; i++)
  {
    if (Util::Rolld100(chance))
    {
      return true;
    }

    chance += GlobalConstants::AttributeIncreasedRaiseStep;
  }

  return Util::Rolld100(chance);
}

const std::map<EffectType, Effect>& GameObject::Effects()
{
  return _activeEffects;
}
