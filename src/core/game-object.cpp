#include "game-object.h"

#include "printer.h"
#include "game-objects-factory.h"
#include "map-level-base.h"
#include "map.h"
#include "application.h"
#include "game-object-info.h"
#include "ai-component.h"
#include "blackboard.h"

#ifdef DEBUG_BUILD
#include "dev-console.h"
#endif

GameObject::GameObject(MapLevelBase* levelOwner)
{
  _levelOwner = levelOwner;
  VisibilityRadius.Set(0);

  _objectId = Application::GetNewId();

#ifdef DEBUG_BUILD
  HexAddressString = Util::StringFormat("0x%X", this);
#endif
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

#ifdef DEBUG_BUILD
  HexAddressString = Util::StringFormat("0x%X", this);
#endif
}

GameObject::~GameObject()
{
  // Assuming walls, trees and the like don't use blackboard
  AIComponent* ai = GetComponent<AIComponent>();
  if (ai != nullptr)
  {
    Blackboard::Instance().Remove(_objectId);
  }

#ifdef DEBUG_BUILD
  GameState* s = Application::Instance().GetGameStateRefByName(GameStates::DEV_CONSOLE);
  DevConsole* dc = static_cast<DevConsole*>(s);
  if (dc != nullptr)
  {
    for (auto& kvp : dc->_objectHandles)
    {
      if (kvp.second == this)
      {
        kvp.second = nullptr;
        break;
      }
    }
  }
#endif
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

  bool canMoveTo = CanMoveTo({ nx, ny });
  if (canMoveTo)
  {
    MoveGameObject(dx, dy);
    return true;
  }

  return false;
}

bool GameObject::MoveTo(int x, int y, bool force)
{
  bool canMoveTo = CanMoveTo({ x, y });
  if (canMoveTo || force)
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

    //DebugLog("MoveTo(%i, %i)\n", x, y);

    _currentCell = _levelOwner->MapArray[PosX][PosY].get();
    _currentCell->Occupied = true;

    return true;
  }

  return false;
}

bool GameObject::MoveTo(const Position &pos, bool force)
{
  //DebugLog("MoveTo(%i;%i)\n\n", pos.X, pos.Y);
  return MoveTo(pos.X, pos.Y, force);
}

bool GameObject::CanMoveTo(const Position& pos)
{
  bool res = true;

  auto curLvl = Map::Instance().CurrentLevel;

  bool isBlocked  = curLvl->IsCellBlocking(pos);
  bool isOccupied = curLvl->MapArray[pos.X][pos.Y]->Occupied;

  res = (!isBlocked && !isOccupied);

  if (!isBlocked && !isOccupied)
  {
    //
    // Check whether we still can move by levitating over the tile.
    //
    auto tileType = curLvl->MapArray[pos.X][pos.Y]->Type;

    bool isDangerous = (tileType == GameObjectType::DEEP_WATER
                     || tileType == GameObjectType::CHASM
                     || tileType == GameObjectType::LAVA);

    bool levitating = HasEffect(ItemBonusType::LEVITATION);

    res = (!isDangerous || levitating);
  }

  return res;
}

void GameObject::Draw(const std::string& overrideColorFg, const std::string& overrideColorBg)
{
  if (FgColor.empty() && BgColor.empty())
  {
    return;
  }

  Printer::Instance().PrintFB(PosX + _levelOwner->MapOffsetX,
                              PosY + _levelOwner->MapOffsetY,
                              Image,
                              overrideColorFg.empty() ? FgColor : overrideColorFg,
                              overrideColorBg.empty() ? BgColor : overrideColorBg);
}

void GameObject::Update()
{
  for (std::pair<const size_t, std::unique_ptr<Component>>& c : _components)
  {
    if (c.second->IsEnabled)
    {
      c.second->Update();
    }
  }
}

void GameObject::ApplyBonuses(ItemComponent* itemRef)
{
  for (auto& i : itemRef->Data.Bonuses)
  {
    if (i.Period > 0)
    {
      i.EffectCounter = 0;
    }

    ApplyBonus(itemRef, i);
  }
}

void GameObject::ApplyBonus(ItemComponent* itemRef, const ItemBonusStruct& bonus)
{
  switch (bonus.Type)
  {
    case ItemBonusType::STR:
    case ItemBonusType::DEF:
    case ItemBonusType::MAG:
    case ItemBonusType::RES:
    case ItemBonusType::SKL:
    case ItemBonusType::SPD:
      _attributesRefsByBonus.at(bonus.Type).AddModifier(itemRef->OwnerGameObject->ObjectId(), bonus.BonusValue);
      break;

    case ItemBonusType::HP:
    case ItemBonusType::MP:
      _rangedAttributesRefsByBonus.at(bonus.Type).Max().AddModifier(itemRef->OwnerGameObject->ObjectId(), bonus.BonusValue);
      _rangedAttributesRefsByBonus.at(bonus.Type).CheckOverflow();
      break;

    case ItemBonusType::VISIBILITY:
      VisibilityRadius.AddModifier(itemRef->OwnerGameObject->ObjectId(), bonus.BonusValue);
      break;

    case ItemBonusType::REGEN:
    case ItemBonusType::REFLECT:
    case ItemBonusType::MANA_SHIELD:
    case ItemBonusType::INVISIBILITY:
    case ItemBonusType::THORNS:
    case ItemBonusType::TELEPATHY:
    case ItemBonusType::LEVITATION:
      AddEffect(bonus);
      break;
  }
}

void GameObject::UnapplyBonuses(ItemComponent* itemRef)
{
  for (auto& i : itemRef->Data.Bonuses)
  {
    if (i.Period > 0)
    {
      i.EffectCounter = 0;
    }

    UnapplyBonus(itemRef, i);
  }
}

void GameObject::UnapplyBonus(ItemComponent* itemRef, const ItemBonusStruct& bonus)
{
  switch (bonus.Type)
  {
    case ItemBonusType::STR:
    case ItemBonusType::DEF:
    case ItemBonusType::MAG:
    case ItemBonusType::RES:
    case ItemBonusType::SKL:
    case ItemBonusType::SPD:
      _attributesRefsByBonus.at(bonus.Type).RemoveModifier(itemRef->OwnerGameObject->ObjectId());
      break;

    case ItemBonusType::HP:
    case ItemBonusType::MP:
      _rangedAttributesRefsByBonus.at(bonus.Type).Max().RemoveModifier(itemRef->OwnerGameObject->ObjectId());
      _rangedAttributesRefsByBonus.at(bonus.Type).CheckOverflow();
      break;

    case ItemBonusType::VISIBILITY:
      VisibilityRadius.RemoveModifier(itemRef->OwnerGameObject->ObjectId());
      break;

    case ItemBonusType::REGEN:
    case ItemBonusType::REFLECT:
    case ItemBonusType::MANA_SHIELD:
    case ItemBonusType::INVISIBILITY:
    case ItemBonusType::THORNS:
    case ItemBonusType::TELEPATHY:
    case ItemBonusType::LEVITATION:
      RemoveEffect(bonus);
      break;
  }
}

void GameObject::MakeTile(const GameObjectInfo& t, GameObjectType typeOverride)
{
  Blocking     = t.IsBlocking;
  BlocksSight  = t.BlocksSight;
  Image        = t.Image;
  FgColor      = t.FgColor;
  BgColor      = t.BgColor;
  ObjectName   = t.ObjectName;
  FogOfWarName = t.FogOfWarName;
  Type         = typeOverride;
}

bool GameObject::ReceiveDamage(GameObject* from,
                               int amount,
                               bool isMagical,
                               bool suppressLog,
                               const std::string& logMsgOverride)
{
  // TODO: isMagical for enemies' armor damage

  int dmgSuccess = false;

  std::string objName = ObjectName;
  ItemComponent* ic = GetComponent<ItemComponent>();
  if (ic != nullptr)
  {
    objName = (ic->Data.IsIdentified) ? ic->Data.IdentifiedName : ic->Data.UnidentifiedName;
  }

  std::string who;
  if (from == nullptr)
  {
    who = "God";
  }
  else
  {
    who = (from->Type == GameObjectType::PLAYER) ? "@" : from->ObjectName;
  }

  std::queue<std::string> logMessages;

  std::string str;

  bool messageOverride = !logMsgOverride.empty();

  if (!Attrs.Indestructible)
  {
    if (amount != 0)
    {
      Attrs.HP.AddMin(-amount);

      str = Util::StringFormat("%s => %s (%i)", who.data(), objName.data(), amount);
      logMessages.push(messageOverride ? logMsgOverride : str);

      if (!IsAlive())
      {
        MarkAndCreateRemains();

        std::string verb = (Type == GameObjectType::HARMLESS
                         || Type == GameObjectType::REMAINS)
                         ? "destroyed"
                         : "killed";

        str = Util::StringFormat("%s was %s", objName.data(), verb.data());
        logMessages.push(messageOverride ? logMsgOverride : str);
      }
    }
    else
    {
      str = Util::StringFormat("%s => %s (0)", who.data(), ObjectName.data());
      logMessages.push(messageOverride ? logMsgOverride : str);
    }

    dmgSuccess = true;
  }
  else
  {
    if (Type != GameObjectType::GROUND)
    {
      str = Util::StringFormat("%s not even scratched!", objName.data());
      logMessages.push(messageOverride ? logMsgOverride : str);
    }
  }

  bool tileVisible = Map::Instance().CurrentLevel->MapArray[PosX][PosY]->Visible;

  if (!suppressLog && tileVisible)
  {
    while (!logMessages.empty())
    {
      auto msg = logMessages.front();
      Printer::Instance().AddMessage(msg);
      logMessages.pop();
    }
  }

  return dmgSuccess;
}

bool GameObject::CanAct()
{
  return (Attrs.ActionMeter >= GlobalConstants::TurnReadyValue);
}

bool GameObject::ShouldSkipTurn()
{
  int speed = Attrs.Spd.Get();

  if (speed >= 0 || _skipTurnsCounter >= std::abs(speed))
  {
    return false;
  }

  return true;
}

void GameObject::WaitForTurn()
{
  int actionIncrement = GetActionIncrement();

  // In towns SPD is ignored
  if (Map::Instance().CurrentLevel->Peaceful)
  {
    Attrs.ActionMeter = GlobalConstants::TurnReadyValue;
  }
  else
  {
    // If SPD is < 0, skip std::abs(SPD) amount of turns
    // without gaining action meter
    if (ShouldSkipTurn())
    {
      _skipTurnsCounter++;
    }
    else
    {
      _skipTurnsCounter = 0;
      Attrs.ActionMeter += actionIncrement;
    }
  }

  if (Type != GameObjectType::PLAYER)
  {
    ProcessEffects();
  }
}

int GameObject::GetActionIncrement()
{
  // +1 is because if SPD is 0 we must add TurnTickValue,
  // but if SPD is 1 we must multiply TurnTickValue by (SPD + 1)
  // to get different value
  int totalSpeed = Attrs.Spd.Get() + 1;

  // If SPD is currently -1 because of modifiers, we still can get 0
  int actionIncrement = (totalSpeed <= 0)
                      ? GlobalConstants::TurnTickValue
                      : totalSpeed * GlobalConstants::TurnTickValue;

  return actionIncrement;
}

void GameObject::TileStandingCheck()
{
  if (IsOnDangerousTile())
  {
    Attrs.HP.Reset(0);
  }
}

bool GameObject::IsOnDangerousTile()
{
  bool res = false;

  if (HasEffect(ItemBonusType::LEVITATION) == false)
  {
    if (_currentCell->Type == GameObjectType::LAVA
     || _currentCell->Type == GameObjectType::DEEP_WATER
     || _currentCell->Type == GameObjectType::CHASM)
    {
      res = true;
    }
  }

  return res;
}

bool GameObject::IsOnTile(GameObjectType tileType)
{
  return (_currentCell->Type == tileType);
}

bool GameObject::IsAlive()
{
  return (Attrs.HP.Min().Get() > 0);
}

void GameObject::CheckPerish()
{
  if (!IsAlive())
  {
    MarkAndCreateRemains();
  }
}

void GameObject::FinishTurn()
{
  ConsumeEnergy();
  ProcessNaturalRegenHP();
  ProcessNaturalRegenMP();
  TileStandingCheck();

  // Moved to WaitForTurn for GameObjects because otherwise effects processing
  // will be done only once after player has finished his turn.
  //ProcessEffects();

  CheckPerish();
}

void GameObject::ConsumeEnergy()
{
  Attrs.ActionMeter -= GlobalConstants::TurnReadyValue;

  // Just in case
  if (Attrs.ActionMeter < 0)
  {
    Attrs.ActionMeter = 0;
  }
}

void GameObject::DropItemsHeld()
{
  // Check for monster's inventory and drop all items from there if any
  ContainerComponent* cc = GetComponent<ContainerComponent>();
  if (cc != nullptr)
  {
    for (auto& i : cc->Contents)
    {
      GameObject* obj = i.release();
      ItemComponent* ic = obj->GetComponent<ItemComponent>();
      ic->OwnerGameObject->SetLevelOwner(Map::Instance().CurrentLevel);
      ic->Transfer();
      ic->OwnerGameObject->PosX = PosX;
      ic->OwnerGameObject->PosY = PosY;

      /*
      //
      // If item was equipped, remove reference to it.
      //
      // If object is to be destroyed, we (theoretically)
      // don't need to access its equipment slots and it will
      // be destroyed without problems during Map::RemoveDestroyed()
      // since EquipmentCategory just stores pointers
      // without owning them, but better be safe than sorry.
      //
      if (ic->Data.EqCategory != EquipmentCategory::NOT_EQUIPPABLE)
      {
        if (EquipmentByCategory.count(ic->Data.EqCategory) == 1)
        {
          for (auto& slot : EquipmentByCategory[ic->Data.EqCategory])
          {
            if (slot == ic)
            {
              slot = nullptr;
              break;
            }
          }
        }
      }
      */
    }
  }
}

void GameObject::ProcessNaturalRegenHP()
{
  _healthRegenTurnsCounter++;

  if (_healthRegenTurnsCounter >= HealthRegenTurns)
  {
    _healthRegenTurnsCounter = 0;

    if (IsLiving)
    {
      Attrs.HP.AddMin(1);
    }
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

#ifndef RELEASE_BUILD
  DebugLog("%s gained %s (duration %i period %i)",
           ObjectName.data(),
           GlobalConstants::BonusDisplayNameByType.count(effectToAdd.Type) == 1 ?
           GlobalConstants::BonusDisplayNameByType.at(effectToAdd.Type).data() :
           "<effect name not found>",
           effectToAdd.Duration,
           effectToAdd.Period);
#endif
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
      if (this == &Application::Instance().PlayerInstance)
      {
        Printer::Instance().AddMessage("You catch fire!");
      }

      ItemBonusStruct eff;
      eff.Cumulative = true;
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
  // Loop goes from end to start to avoid potential skipping
  // during erasing when there are adjacent elements.
  for (int i = _activeEffects.size() - 1; i >= 0; i--)
  {
    auto it = _activeEffects.begin();
    std::advance(it, i);

    bool shouldErase = false;
    for (ItemBonusStruct& bonus : it->second)
    {
      if (bonus.Id == t.Id)
      {
        UnapplyEffect(bonus);
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

void GameObject::RemoveEffectFirstFound(const ItemBonusType& type)
{
  for (int i = _activeEffects.size() - 1; i >= 0; i--)
  {
    auto it = _activeEffects.begin();
    std::advance(it, i);

    bool shouldErase = false;
    for (ItemBonusStruct& bonus : it->second)
    {
      if (bonus.Type == type)
      {
        UnapplyEffect(bonus);
        shouldErase = true;
        break;
      }
    }

    if (shouldErase)
    {
      _activeEffects.erase(it);
      break;
    }
  }
}

void GameObject::RemoveEffectAllOf(const ItemBonusType& type)
{
  for (int i = _activeEffects.size() - 1; i >= 0; i--)
  {
    auto it = _activeEffects.begin();
    std::advance(it, i);

    bool shouldErase = false;
    for (ItemBonusStruct& bonus : it->second)
    {
      if (bonus.Type == type)
      {
        UnapplyEffect(bonus);
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

void GameObject::AttachTrigger(TriggerType type,
                               const std::function<bool ()>& condition,
                               const std::function<void ()>& handler)
{
  TriggerComponent* tc = AddComponent<TriggerComponent>();
  tc->Setup({ type, condition, handler });
}

void GameObject::DispelEffect(const ItemBonusType& t)
{
  for (int i = _activeEffects.size() - 1; i >= 0; i--)
  {
    auto it = _activeEffects.begin();
    std::advance(it, i);

    bool shouldErase = false;
    for (ItemBonusStruct& bonus : it->second)
    {
      if (bonus.Type == t && !bonus.FromItem)
      {
        UnapplyEffect(bonus);
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
  for (int i = _activeEffects.size() - 1; i >= 0; i--)
  {
    auto it = _activeEffects.begin();
    std::advance(it, i);

    auto& ae = _activeEffects[it->first];
    for (int j = ae.size() - 1; j >= 0; j--)
    {
      if (ae[j].Duration > 0)
      {
        if (ae[j].Period > 0)
        {
          ae[j].EffectCounter++;

          if ((ae[j].EffectCounter % ae[j].Period) == 0)
          {
            ae[j].EffectCounter = 0;
            EffectAction(ae[j]);
          }
        }
        else
        {
          EffectAction(ae[j]);
        }

/*
#ifndef RELEASE_BUILD
        DebugLog("\t%s ProcessEffects() [%s] duration %i = %i - 1", ObjectName.data(), GlobalConstants::BonusDisplayNameByType.at(ae[j].Type).data(), ae[j].Duration, ae[j].Duration);
#endif
*/

        ae[j].Duration--;
      }
      else if (ae[j].Duration == 0)
      {
        UnapplyEffect(ae[j]);
        ae.erase(ae.begin() + j);
      }
      else if (ae[j].Duration == -1)
      {
        if (ae[j].Period > 0)
        {
          ae[j].EffectCounter++;

          if ((ae[j].EffectCounter % ae[j].Period) == 0)
          {
            ae[j].EffectCounter = 0;
            EffectAction(ae[j]);
          }
        }
        else
        {
          EffectAction(ae[j]);
        }
      }
    }

    if (ae.empty())
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
  //
  // Loot is not created on dangerous tiles
  //
  bool tileDangerous = Map::Instance().IsTileDangerous({ PosX, PosY });
  if (!tileDangerous)
  {
    // Destroying remains should not spawn another remains.
    if (Type != GameObjectType::REMAINS)
    {
      auto go = GameObjectsFactory::Instance().CreateRemains(this);
      _levelOwner->InsertGameObject(go);
    }

    DropItemsHeld();
  }

  IsDestroyed = true;
}

IR GameObject::Interact()
{
  if (Util::IsFunctionValid(InteractionCallback))
  {
    return InteractionCallback();
  }

  return { InteractionResult::UNDEFINED, GameStates::UNDEIFNED };
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
  bool customChance = (attr.RaiseProbability >= 0);
  int chance = customChance ? attr.RaiseProbability : GlobalConstants::AttributeMinimumRaiseChance;

  if (!customChance)
  {
    int iterations = attr.Talents;
    for (int i = 0; i < iterations; i++)
    {
      if (Util::Rolld100(chance))
      {
        return true;
      }

      chance += GlobalConstants::AttributeIncreasedRaiseStep;
    }
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

#ifdef DEBUG_BUILD
std::vector<std::string> GameObject::DebugInfo()
{
  std::vector<std::string> res;

  std::string str = Util::StringFormat("  ObjName: %s", ObjectName.data());

  res.push_back("{");
  res.push_back(str);

  str = Util::StringFormat("  ObjectId: %lu", _objectId);
  res.push_back(str);

  std::string ch = { (char)Image };
  str = Util::StringFormat("  Image: '%s'", ch.data());

  res.push_back(str);

  str = Util::StringFormat("  Position: { %i, %i }", PosX, PosY);
  res.push_back(str);

  str = Util::StringFormat("  FgColor: %s", FgColor.data());
  res.push_back(str);

  str = Util::StringFormat("  BgColor: %s", BgColor.data());
  res.push_back(str);

  str = Util::StringFormat("  Components: %zu", _components.size());
  res.push_back(str);

  for (auto& kvp : _components)
  {
    str = Util::StringFormat("    %s [0x%X]",
                             typeid(*kvp.second.get()).name(),
                             kvp.second.get());
    res.push_back(str);
  }

  str = Util::StringFormat("  Effects: %zu", _activeEffects.size());
  res.push_back(str);

  for (auto& kvp : _activeEffects)
  {
    str = Util::StringFormat("    %lu (%zu):", kvp.first, kvp.second.size());
    res.push_back(str);

    for (ItemBonusStruct& i : kvp.second)
    {
      auto lines = i.ToStrings();
      res.push_back("      " + lines[0]);
    }
  }

  res.push_back("}");

  return res;
}
#endif
