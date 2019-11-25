#include "game-object.h"

#include "printer.h"
#include "game-objects-factory.h"
#include "map-level-base.h"
#include "map.h"
#include "application.h"
#include "game-object-info.h"
#include "blackboard.h"

GameObject::GameObject(MapLevelBase* levelOwner)
{
  _levelOwner = levelOwner;
  VisibilityRadius.Set(0);

  _objectId = Application::GetNewId();
}

GameObject::GameObject(MapLevelBase *levelOwner,
                       int x,
                       int y,
                       int avatar,
                       const std::string &htmlColor,
                       const std::string &bgColor)
{
  _objectId = Application::GetNewId();

  Init(levelOwner, x, y, avatar, htmlColor, bgColor);
}

GameObject::~GameObject()
{
  Blackboard::Instance().Remove(_objectId);
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

    //printf("MoveTo(%i, %i)\n", x, y);

    _currentCell = _levelOwner->MapArray[PosX][PosY].get();
    _currentCell->Occupied = true;

    return true;
  }

  return false;
}

bool GameObject::MoveTo(const Position &pos)
{
  //printf("MoveTo(%i;%i)\n\n", pos.X, pos.Y);
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

void GameObject::MakeTile(const GameObjectInfo& t, GameObjectType typeOverride)
{
  Blocking = t.IsBlocking;
  BlocksSight = t.BlocksSight;
  Image = t.Image;
  FgColor = t.FgColor;
  BgColor = t.BgColor;
  ObjectName = t.ObjectName;
  FogOfWarName = t.FogOfWarName;
  Type = typeOverride;
}

bool GameObject::ReceiveDamage(GameObject* from, int amount, bool isMagical, const std::string& logMsgOverride)
{  
  // TODO: isMagical for enemies' armor damage

  int dmgSuccess = false;

  std::string objName = ObjectName;
  ItemComponent* ic = GetComponent<ItemComponent>();
  if (ic != nullptr)
  {
    objName = (ic->Data.IsIdentified) ? ic->Data.IdentifiedName : ic->Data.UnidentifiedName;
  }

  if (!Attrs.Indestructible)
  {
    if (amount != 0)
    {
      Attrs.HP.AddMin(-amount);

      auto str = Util::StringFormat("%s was hit for %i damage", ObjectName.data(), amount);
      Printer::Instance().AddMessage((logMsgOverride.length() == 0) ? str : logMsgOverride);

      if (!IsAlive())
      {
        MarkAndCreateRemains();

        std::string verb = (Type == GameObjectType::HARMLESS)
                         ? "destroyed"
                         : "killed";

        auto msg = Util::StringFormat("%s was %s", objName.data(), verb.data());
        Printer::Instance().AddMessage(msg);
      }
    }
    else
    {
      auto str = Util::StringFormat("%s was hit for no damage", ObjectName.data());
      Printer::Instance().AddMessage((logMsgOverride.length() == 0) ? str : logMsgOverride);
    }

    dmgSuccess = true;
  }
  else
  {    
    if (Type != GameObjectType::GROUND)
    {
      auto str = Util::StringFormat("%s not even scratched!", objName.data());
      Printer::Instance().AddMessage(str);
    }
  }

  return dmgSuccess;
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
  speedIncrement = (speedIncrement <= 0) ? 5 : speedIncrement;

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
  return (Attrs.HP.Min().Get() > 0);
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
    Attrs.HP.AddMin(1);
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

void GameObject::AddEffect(uint64_t effectGiver, const Effect &effectToAdd)
{
  if (_activeEffects.count(effectGiver) == 0)
  {
    _activeEffects[effectGiver] = effectToAdd;
  }
  else
  {
    if (effectToAdd.Cumulative)
    {
      _activeEffects[effectGiver].Duration += effectToAdd.Duration;
      _activeEffects[effectGiver].Power += effectToAdd.Power;
    }
    else
    {
      _activeEffects[effectGiver] = effectToAdd;
    }
  }

  ApplyEffect(_activeEffects[effectGiver]);
}

void GameObject::AddEffect(uint64_t effectGiver,
                           EffectType type,
                           int power,
                           int duration,
                           bool cumulative)
{
  Effect e = { type, power, duration, cumulative };

  if (_activeEffects.count(effectGiver) == 0)
  {
    _activeEffects[effectGiver] = e;
  }
  else
  {
    if (cumulative)
    {
      _activeEffects[effectGiver].Duration += duration;
      _activeEffects[effectGiver].Power += power;
    }
    else
    {
      _activeEffects[effectGiver] = e;
    }
  }

  ApplyEffect(_activeEffects[effectGiver]);
}

void GameObject::ApplyEffect(const Effect& e)
{
  switch (e.Type)
  {
    case EffectType::TELEPATHY:
    case EffectType::ILLUMINATED:
      VisibilityRadius.AddModifier(ObjectId(), e.Power);
      break;

    case EffectType::FROZEN:
      Attrs.Spd.AddModifier(ObjectId(), -e.Power);
      break;
  }
}

void GameObject::UnapplyEffect(const Effect& e)
{
  switch (e.Type)
  {
    case EffectType::TELEPATHY:
    case EffectType::ILLUMINATED:
      VisibilityRadius.RemoveModifier(ObjectId());
      break;

    case EffectType::FROZEN:
      Attrs.Spd.RemoveModifier(ObjectId());
      break;
  }
}

void GameObject::RemoveEffect(uint64_t itemId, EffectType t, int usePowerAsAdditionalInfo)
{  
  if (usePowerAsAdditionalInfo != -1)
  {
    for (int i = 0; i < _activeEffects.size(); i++)
    {
      auto it = _activeEffects.begin();
      std::advance(it, i);

      if (it->second.Type == t && it->second.Power == usePowerAsAdditionalInfo)
      {
        UnapplyEffect(it->second);
        _activeEffects.erase(it);
      }
    }
  }
  else
  {
    if (_activeEffects.count(itemId) == 1)
    {
      UnapplyEffect(_activeEffects[itemId]);
      _activeEffects.erase(itemId);
    }
  }
}

void GameObject::RemoveEffectAll(EffectType t)
{
  for (int i = 0; i < _activeEffects.size(); i++)
  {
    auto it = _activeEffects.begin();
    std::advance(it, i);

    if (it->second.Type == t)
    {
      _activeEffects.erase(it);
    }
  }
}

bool GameObject::HasEffect(EffectType t)
{  
  for (auto& kvp : _activeEffects)
  {
    if (kvp.second.Type == t)
    {
      return true;
    }
  }

  return false;
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
      Attrs.HP.AddMin(-e.Power);
      break;

    case EffectType::REGEN:
      Attrs.HP.AddMin(e.Power);
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
      kvp.second.Add(1);
    }
  }

  // HP and MP

  int baseHp = (baseHpOverride == -1) ? Attrs.HP.Talents : baseHpOverride;
  if (baseHp == 0)
  {
    baseHp = 1;
  }

  int minRndHp = baseHp;
  int maxRndHp = 2 * baseHp;

  int hpToAdd = RNG::Instance().RandomRange(minRndHp, maxRndHp);
  Attrs.HP.AddMax(hpToAdd);

  int minRndMp = Attrs.Mag.OriginalValue();
  int maxRndMp = Attrs.Mag.OriginalValue() + Attrs.MP.Talents;

  int mpToAdd = RNG::Instance().RandomRange(minRndMp, maxRndMp);
  Attrs.MP.AddMax(mpToAdd);

  Attrs.Lvl.Add(1);
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

const std::map<uint64_t, Effect>& GameObject::Effects()
{
  return _activeEffects;
}

uint64_t GameObject::ObjectId()
{
  return _objectId;
}
