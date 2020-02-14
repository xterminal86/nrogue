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

bool GameObject::MoveTo(int x, int y, bool force)
{
  bool isBlocked = Map::Instance().CurrentLevel->IsCellBlocking({ x, y });
  bool isOccupied = Map::Instance().CurrentLevel->MapArray[x][y]->Occupied;  
  if ((!isBlocked && !isOccupied) || force)
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

bool GameObject::MoveTo(const Position &pos, bool force)
{
  //printf("MoveTo(%i;%i)\n\n", pos.X, pos.Y);
  return MoveTo(pos.X, pos.Y, force);
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

        std::string verb = (Type == GameObjectType::HARMLESS
                         || Type == GameObjectType::REMAINS)
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

  if (Type != GameObjectType::PLAYER)
  {
    ProcessEffects();
  }
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

  ProcessNaturalRegenHP();
  ProcessNaturalRegenMP();

  // Moved to WaitForTurn for GameObjects because otherwise effects processing
  // will be done only once after player has finished his turn.
  //ProcessEffects();

  if (!IsAlive())
  {
    MarkAndCreateRemains();
  }
}

void GameObject::ProcessNaturalRegenHP()
{
  _healthRegenTurnsCounter++;

  if (_healthRegenTurnsCounter >= HealthRegenTurns)
  {
    _healthRegenTurnsCounter = 0;
    Attrs.HP.AddMin(1);
  }
}

void GameObject::ProcessNaturalRegenMP()
{
  _manaRegenTurnsCounter++;

  float turnsManaRegen = 0.0f;
  if (Attrs.Mag.Get() <= 0)
  {
    _manaRegenTurnsCounter = 0;
  }
  else
  {
    turnsManaRegen = 1.0f / (float)Attrs.Mag.Get();
    int turnsInt = (int)(turnsManaRegen * 100.0f);
    if (turnsInt < 5)
    {
      turnsInt = 5;
    }

    if (_manaRegenTurnsCounter > turnsInt)
    {
      Attrs.MP.AddMin(1);
      _manaRegenTurnsCounter = 0;
    }
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

void GameObject::AddEffect(const ItemBonusStruct& effectToAdd)
{
  uint64_t id = effectToAdd.Id;

  if (effectToAdd.Cumulative)
  {
    _activeEffects[id].push_back(effectToAdd);
  }
  else
  {
    if (_activeEffects[id].empty())
    {
      _activeEffects[id].push_back(effectToAdd);
    }
    else
    {
      // There can be only one.
      _activeEffects[id][0] = effectToAdd;
    }
  }

  ApplyEffect(effectToAdd);
}

void GameObject::ApplyEffect(const ItemBonusStruct& e)
{  
  switch (e.Type)
  {      
    case ItemBonusType::STR:
    case ItemBonusType::DEF:
    case ItemBonusType::MAG:
    case ItemBonusType::RES:
    case ItemBonusType::SPD:
    case ItemBonusType::SKL:      
      _attributesRefsByBonus.at(e.Type).AddModifier(e.Id, e.BonusValue);
      break;

    case ItemBonusType::BLINDNESS:
    case ItemBonusType::ILLUMINATED:
      VisibilityRadius.AddModifier(e.Id, e.BonusValue);
      break;

    case ItemBonusType::FROZEN:
      Attrs.Spd.AddModifier(e.Id, -e.BonusValue);
      break;

    case ItemBonusType::BURNING:
    {
      Printer::Instance().AddMessage("You catch fire!");

      ItemBonusStruct eff;
      eff.Type = ItemBonusType::ILLUMINATED;
      eff.Duration = e.Duration;
      eff.Id = e.Id;
      eff.BonusValue = 10;

      AddEffect(eff);
    }
    break;
  }
}

void GameObject::UnapplyEffect(const ItemBonusStruct& e)
{
  switch (e.Type)
  {
    case ItemBonusType::STR:
    case ItemBonusType::DEF:
    case ItemBonusType::MAG:
    case ItemBonusType::RES:
    case ItemBonusType::SPD:
    case ItemBonusType::SKL:
      _attributesRefsByBonus.at(e.Type).RemoveModifier(e.Id);
      break;

    case ItemBonusType::BLINDNESS:
    case ItemBonusType::ILLUMINATED:
      VisibilityRadius.RemoveModifier(e.Id);
      break;

    case ItemBonusType::FROZEN:
      Attrs.Spd.RemoveModifier(e.Id);
      break;    
  }
}

void GameObject::RemoveEffect(const ItemBonusStruct& t)
{
  for (int i = 0; i < _activeEffects.size(); i++)
  {
    auto it = _activeEffects.begin();
    std::advance(it, i);

    bool shouldErase = false;
    for (auto& item : it->second)
    {
      if (item.Id == t.Id)
      {        
        UnapplyEffect(item);
        shouldErase = true;
        break;
      }
    }

    if (shouldErase)
    {      
      _activeEffects.erase(it);
    }
  }
}

void GameObject::DispelEffect(const ItemBonusType& t)
{
  for (int i = 0; i < _activeEffects.size(); i++)
  {
    auto it = _activeEffects.begin();
    std::advance(it, i);

    bool shouldErase = false;
    for (auto& item : it->second)
    {
      if (item.Type == t && !item.FromItem)
      {
        UnapplyEffect(item);
        shouldErase = true;
      }
    }

    if (shouldErase)
    {
      _activeEffects.erase(it);
    }
  }
}

bool GameObject::HasEffect(const ItemBonusType& t)
{  
  for (auto& kvp : _activeEffects)
  {
    for (auto& i : kvp.second)
    {
      if (i.Type == t)
      {
        return true;
      }
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

    auto& v = _activeEffects[it->first];
    for (int j = 0; j < v.size(); j++)
    {
      if (v[j].Duration > 0)
      {
        if (v[j].Period > 0)
        {
          v[j].EffectCounter++;

          if (v[j].EffectCounter % v[j].Period == 0)
          {
            v[j].EffectCounter = 0;
            EffectAction(v[j]);
          }
        }
        else
        {
          EffectAction(v[j]);
        }

        v[j].Duration--;
      }
      else if (v[j].Duration == 0)
      {        
        UnapplyEffect(v[j]);
        v.erase(v.begin() + j);
      }
      else if (v[j].Duration == -1)
      {
        if (v[j].Period > 0)
        {
          v[j].EffectCounter++;

          if (v[j].EffectCounter % v[j].Period == 0)
          {
            v[j].EffectCounter = 0;
            EffectAction(v[j]);
          }
        }
        else
        {
          EffectAction(v[j]);
        }
      }
    }

    if (v.empty())
    {
      _activeEffects.erase(it);      
    }
  }
}

void GameObject::EffectAction(const ItemBonusStruct& e)
{
  switch (e.Type)
  {
    case ItemBonusType::BURNING:
    case ItemBonusType::POISONED:
    case ItemBonusType::REGEN:
      Attrs.HP.AddMin(e.BonusValue);
      break;

    case ItemBonusType::PARALYZE:
      Attrs.ActionMeter = 0;
      break;
  }
}

void GameObject::MarkAndCreateRemains()
{
  // Destroying remains should not spawn another remains.
  if (Type != GameObjectType::REMAINS)
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

  int minRndHp = (Attrs.HP.Talents + 1);
  int maxRndHp = 2 * (Attrs.HP.Talents + 1);

  int hpToAdd = RNG::Instance().RandomRange(minRndHp, maxRndHp + 1);

  if (baseHpOverride != -1)
  {
    Attrs.HP.AddMax(baseHpOverride);
  }
  else
  {
    Attrs.HP.AddMax(hpToAdd);
  }

  int minRndMp = Attrs.Mag.OriginalValue();
  int maxRndMp = Attrs.Mag.OriginalValue() + Attrs.MP.Talents;

  int mpToAdd = RNG::Instance().RandomRange(minRndMp, maxRndMp + 1);
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

const std::map<uint64_t, std::vector<ItemBonusStruct>>& GameObject::Effects()
{
  return _activeEffects;
}

uint64_t GameObject::ObjectId()
{
  return _objectId;
}
