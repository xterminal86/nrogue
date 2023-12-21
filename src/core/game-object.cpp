#include "game-object.h"

#include "application.h"
#include "printer.h"
#include "game-objects-factory.h"
#include "items-factory.h"
#include "item-component.h"
#include "map-level-base.h"
#include "map.h"
#include "game-object-info.h"
#include "ai-component.h"
#include "trigger-component.h"
#include "serializer.h"
#include "blackboard.h"
#include "gid-generator.h"
#include "door-component.h"

#ifdef DEBUG_BUILD
#include "dev-console.h"

std::unordered_map<uint64_t, GameObject*> GameObjectsById;
#endif

GameObject::GameObject(MapLevelBase* levelOwner)
{
  _levelOwner = levelOwner;
  VisibilityRadius.Set(0);

  _objectId = GID::Instance().GenerateGlobalId();

#ifdef DEBUG_BUILD
  GameObjectsById[_objectId] = this;
  HexAddressString = Util::StringFormat("0x%X", this);
#endif
}

// =============================================================================

GameObject::GameObject(MapLevelBase *levelOwner,
                       int x,
                       int y,
                       int avatar,
                       const uint32_t& htmlColor,
                       const uint32_t& bgColor)
{
  _objectId = GID::Instance().GenerateGlobalId();

  Init(levelOwner, x, y, avatar, htmlColor, bgColor);

#ifdef DEBUG_BUILD
  GameObjectsById[_objectId] = this;
  HexAddressString = Util::StringFormat("0x%X", this);
#endif
}

// =============================================================================

GameObject::~GameObject()
{
  //
  // Assuming walls, trees and the like don't use blackboard.
  //
  AIComponent* ai = GetComponent<AIComponent>();
  if (ai != nullptr)
  {
    Blackboard::Instance().Remove(_objectId);
  }

  if (Util::IsFunctionValid(OnDestroy))
  {
    OnDestroy(this);
  }

#ifdef DEBUG_BUILD
  GameState* s = Application::Instance().GetGameStateRefByName(GameStates::DEV_CONSOLE);
  if (s != nullptr)
  {
    DevConsole* dc = static_cast<DevConsole*>(s);
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

// =============================================================================

void GameObject::Init(MapLevelBase* levelOwner,
                      int x,
                      int y,
                      int avatar,
                      const uint32_t& fgColor,
                      const uint32_t& bgColor)
{
  PosX = x;
  PosY = y;
  Image = avatar;
  FgColor = fgColor;
  BgColor = bgColor;

  Attrs.ActionMeter = GlobalConstants::TurnReadyValue;

  _levelOwner = levelOwner;

  //
  // _currentCell->Occupied is not set to true by default,
  // see game-object.h comments for Occupied field.
  //
  if (_levelOwner != nullptr)
  {
    _currentCell  = _levelOwner->MapArray[PosX][PosY].get();
    _previousCell = _levelOwner->MapArray[PosX][PosY].get();
  }
}

// =============================================================================

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

// =============================================================================

bool GameObject::MoveTo(int x, int y, bool force)
{
  bool canMoveTo = CanMoveTo({ x, y });
  if (canMoveTo || force)
  {
    //
    // When we change level, previous position (PosX and PosY)
    // is pointing to the stairs down on previous level,
    // which may not have the same dimensions as the level
    // we just arrived in. So to avoid going outside array bounds,
    // we have to perform a check.
    //
    // Unblocking of stairs in such case is done directly in ChangeLevel(),
    // so we just skip this case here.
    //
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

// =============================================================================

bool GameObject::MoveTo(const Position& pos, bool force)
{
  //DebugLog("MoveTo(%i;%i)\n\n", pos.X, pos.Y);
  return MoveTo(pos.X, pos.Y, force);
}

// =============================================================================

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

// =============================================================================

void GameObject::Draw(const uint32_t& overrideColorFg,
                      const uint32_t& overrideColorBg,
                      int imageOverride)
{
  bool dontDraw = (FgColor == Colors::None
                && BgColor == Colors::None
                && overrideColorFg == Colors::None
                && overrideColorBg == Colors::None);

  if (dontDraw)
  {
    return;
  }

  Printer::Instance().PrintFB(PosX + _levelOwner->MapOffsetX,
                              PosY + _levelOwner->MapOffsetY,
                              (imageOverride != -1)
                              ? imageOverride
                              : Image,
                              overrideColorFg == Colors::None
                              ? FgColor
                              : overrideColorFg,
                              overrideColorBg == Colors::None
                              ? BgColor
                              : overrideColorBg);
}

// =============================================================================

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

// =============================================================================

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

// =============================================================================

//
// It is implied, that ApplyBonus() / UnapplyBonus() methods
// give effects from items only, which is reflected by
// ItemComponent* argument.
//
void GameObject::ApplyBonus(ItemComponent* itemRef,
                            const ItemBonusStruct& bonus)
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
    {
      if (IsLiving)
      {
        AddEffect(bonus);
      }
    }
    break;

    case ItemBonusType::REFLECT:
    case ItemBonusType::MANA_SHIELD:
    case ItemBonusType::INVISIBILITY:
    case ItemBonusType::THORNS:
    case ItemBonusType::TELEPATHY:
    case ItemBonusType::TRUE_SEEING:
    case ItemBonusType::LEVITATION:
      AddEffect(bonus);
      break;
  }
}

// =============================================================================

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

// =============================================================================

void GameObject::UnapplyBonus(ItemComponent* itemRef,
                              const ItemBonusStruct& bonus)
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
    case ItemBonusType::TRUE_SEEING:
    case ItemBonusType::LEVITATION:
      RemoveEffect(bonus.Type, bonus.Id);
      break;
  }
}

// =============================================================================

const Position& GameObject::GetPosition()
{
  _position.X = PosX;
  _position.Y = PosY;

  return _position;
}

// =============================================================================

void GameObject::MakeTile(const GameObjectInfo& t,
                          GameObjectType typeOverride)
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

// =============================================================================

bool GameObject::ReceiveDamage(GameObject* from,
                               int amount,
                               bool isMagical,
                               bool ignoreArmor,
                               bool directDamage,
                               bool suppressLog)
{
  if (directDamage)
  {
    Attrs.HP.AddMin(-amount);
    return true;
  }

  int dmgSuccess = false;

  //
  // Static objects should display their object's name.
  //
  std::string objName = ObjectName;

  //
  // Items should display their ID-dependent name.
  //
  ItemComponent* ic = GetComponent<ItemComponent>();
  if (ic != nullptr)
  {
    objName = (ic->Data.IsIdentified)
             ? ic->Data.IdentifiedName
             : ic->Data.UnidentifiedName;
  }

  //
  // Actors should display their avatar.
  //
  AIComponent* aic = GetComponent<AIComponent>();
  if (aic != nullptr)
  {
    objName = Util::GetGameObjectDisplayCharacter(this);
  }

  std::queue<std::string> logMessages;

  std::string str;

  int dmgReturned = 0;

  //
  // See comments in MapLevelBase::PlaceWall()
  //
  if (!Attrs.Indestructible && Type != GameObjectType::PICKAXEABLE)
  {
    if (isMagical)
    {
      str = Util::ProcessMagicalDamage(this, from, amount);
      if (!str.empty())
      {
        logMessages.push(str);
      }
    }
    else
    {
      auto msgs = Util::ProcessPhysicalDamage(this, from, amount, ignoreArmor);
      for (auto& m : msgs)
      {
        logMessages.push(m);
      }

      dmgReturned = Util::ProcessThorns(this, amount);

      if (dmgReturned != 0)
      {
        if (Util::IsPlayer(from))
        {
          static_cast<Player*>(from)->ReceiveDamage(this,
                                                    dmgReturned,
                                                    true,
                                                    true,
                                                    true,
                                                    false);
        }
        else
        {
          from->ReceiveDamage(this, dmgReturned, true, true, true, true);
        }
      }
    }

    if (!HasNonZeroHP())
    {
      //
      // Actors will display their full name on death.
      //
      if (aic != nullptr)
      {
        objName = ObjectName;
      }

      MarkAndCreateRemains();

      std::string verb = (Type == GameObjectType::HARMLESS
                       || Type == GameObjectType::REMAINS)
                       ? "destroyed"
                       : "killed";

      logMessages.push(Util::StringFormat("%s was %s", objName.data(), verb.data()));
    }

    dmgSuccess = true;
  }
  else
  {
    if (Type != GameObjectType::GROUND)
    {
      logMessages.push(Util::StringFormat(Strings::FmtSNotEvenScratched,
                                          objName.data()));
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

// =============================================================================

bool GameObject::CanAct()
{
  //
  // If actor was killed by a trigger (e.g. mine trap or something),
  // and actor could make several turns because of high SPD,
  // its update should end prematurely.
  //
  // ( see Map::UpdateAll() )
  //
  return (HasNonZeroHP() && (Attrs.ActionMeter >= GlobalConstants::TurnReadyValue));
}

// =============================================================================

bool GameObject::ShouldSkipTurn()
{
  int speed = Attrs.Spd.Get();

  if (speed >= 0 || _skipTurnsCounter >= std::abs(speed))
  {
    return false;
  }

  return true;
}

// =============================================================================

void GameObject::WaitForTurn()
{
  int actionIncrement = GetActionIncrement();

  //
  // In towns SPD is ignored.
  //
  if (Map::Instance().CurrentLevel->Peaceful)
  {
    Attrs.ActionMeter = GlobalConstants::TurnReadyValue;
  }
  else
  {
    //
    // If SPD is < 0, skip std::abs(SPD) amount of turns
    // without gaining action meter.
    //
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

// =============================================================================

int GameObject::GetActionIncrement()
{
  //
  // +1 is because if SPD is 0 we must add TurnTickValue,
  // but if SPD is 1 we must multiply TurnTickValue by (SPD + 1)
  // to get different value.
  //
  int totalSpeed = Attrs.Spd.Get() + 1;

  //
  // If SPD is currently -1 because of modifiers, we still can get 0
  //
  int actionIncrement = (totalSpeed <= 0)
                      ? GlobalConstants::TurnTickValue
                      : totalSpeed * GlobalConstants::TurnTickValue;

  return actionIncrement;
}

// =============================================================================

void GameObject::TileStandingCheck()
{
  if (IsOnDangerousTile())
  {
    Attrs.HP.Reset(0);
  }
}

// =============================================================================

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

// =============================================================================

bool GameObject::IsOnTile(GameObjectType tileType)
{
  return (_currentCell->Type == tileType);
}

// =============================================================================

bool GameObject::HasNonZeroHP()
{
  return (Attrs.HP.Min().Get() > 0);
}

// =============================================================================

void GameObject::CheckPerish()
{
  if (!HasNonZeroHP())
  {
    MarkAndCreateRemains();
  }
}

// =============================================================================

void GameObject::FinishTurn()
{
  ConsumeEnergy();
  ProcessNaturalRegenHP();
  ProcessNaturalRegenMP();
  TileStandingCheck();

  //
  // Moved to WaitForTurn for GameObjects
  // because otherwise effects processing
  // will be done only once after player has finished his turn.
  //
  //ProcessEffects();

  ProcessItemsEffects();

  //
  // If actor's SPD is high, it can make several turns
  // and skip trigger position related activation,
  // so we have to check triggers every turn.
  //
  Map::Instance().UpdateTriggers(TriggerUpdateType::FINISH_TURN);

  CheckPerish();
}

// =============================================================================

void GameObject::ConsumeEnergy()
{
  Attrs.ActionMeter -= GlobalConstants::TurnReadyValue;

  //
  // Just in case...
  //
  if (Attrs.ActionMeter < 0)
  {
    Attrs.ActionMeter = 0;
  }
}

// =============================================================================

void GameObject::DropItemsHeld()
{
  //
  // Check for monster's inventory and drop all items from there if any.
  //
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
      ic->Data.IsEquipped = false;
    }
  }

  //
  // Money as well.
  //
  if (Money > 0)
  {
    auto money = ItemsFactory::Instance().CreateMoney(Money);
    money->PosX = PosX;
    money->PosY = PosY;
    Map::Instance().CurrentLevel->PlaceGameObject(money);
  }
}

// =============================================================================

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

// =============================================================================

void GameObject::ProcessNaturalRegenMP()
{
  _manaRegenTurnsCounter++;

  double turnsManaRegen = 0.0;
  if (Attrs.Mag.Get() <= 0)
  {
    _manaRegenTurnsCounter = 0;
  }
  else
  {
    turnsManaRegen = 1.0 / (double)Attrs.Mag.Get();
    int turnsInt = (int)(turnsManaRegen * 100.0);
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

// =============================================================================

void GameObject::MoveGameObject(int dx, int dy)
{
  _previousCell = Map::Instance().CurrentLevel->MapArray[PosX][PosY].get();
  _previousCell->Occupied = false;

  PosX += dx;
  PosY += dy;

  _currentCell = Map::Instance().CurrentLevel->MapArray[PosX][PosY].get();
  _currentCell->Occupied = true;
}

// =============================================================================

void GameObject::AddEffect(const ItemBonusStruct& effectToAdd)
{
  if (IsImmune(effectToAdd))
  {
    return;
  }

  uint64_t id = effectToAdd.Id;

  //
  // If there are no effects so far, add it.
  //
  if (_activeEffects[id].empty())
  {
    _activeEffects[id].push_back(effectToAdd);
  }
  else
  {
    //
    // Cumulative effect should be added regardless.
    //
    if (effectToAdd.Cumulative)
    {
      _activeEffects[id].push_back(effectToAdd);
    }
    else
    {
      bool isDifferent = true;

      size_t ind = 0;
      for (size_t i = 0; i < _activeEffects[id].size(); i++)
      {
        if (_activeEffects[id][i].Type == effectToAdd.Type)
        {
          isDifferent = false;
          ind = i;
          break;
        }
      }

      //
      // If this is a different type of effect, it should be added.
      //
      if (isDifferent)
      {
        _activeEffects[id].push_back(effectToAdd);
      }
      else
      {
        //
        // Otherwise there can be only one.
        //
        _activeEffects[id][ind] = effectToAdd;
      }
    }
  }

  ApplyEffect(effectToAdd);

  /*
  #ifdef DEBUG_BUILD
  auto str = Util::StringFormat("%s gained %s (duration %i period %i)",
                                ObjectName.data(),
                                GlobalConstants::BonusDisplayNameByType.count(effectToAdd.Type) == 1 ?
                                GlobalConstants::BonusDisplayNameByType.at(effectToAdd.Type).data() :
                                "<effect name not found>",
                                effectToAdd.Duration,
                                effectToAdd.Period);
  Logger::Instance().Print(str);
  DebugLog(str.data());
  #endif
  */
}

// =============================================================================

bool GameObject::IsImmune(const ItemBonusStruct& effectToAdd)
{
  bool res = false;

  switch (effectToAdd.Type)
  {
    case ItemBonusType::POISONED:
      res = HasEffect(ItemBonusType::POISON_IMMUNE);
      break;

    case ItemBonusType::PARALYZE:
      res = HasEffect(ItemBonusType::FREE_ACTION);
      break;
  }

  return res;
}

// =============================================================================

//
// "Effect"-type methods are implied to be dealing with externally
// applied effects only (e.g. being affected by a spell or a shrine).
//
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

    //
    // REGEN is 1 HP in a period.
    //
    case ItemBonusType::REGEN:
      Attrs.HungerSpeed.AddModifier(e.Id, e.BonusValue * 5);
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
      if (Util::IsPlayer(this))
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

    case ItemBonusType::POISON_IMMUNE:
    {
      if (HasEffect(ItemBonusType::POISONED))
      {
        if (Util::IsPlayer(this))
        {
          Printer::Instance().AddMessage("The poison disperses!");
        }

        DispelEffectsAllOf(ItemBonusType::POISONED);
      }
    }
    break;

    case ItemBonusType::FREE_ACTION:
    {
      if (HasEffect(ItemBonusType::PARALYZE))
      {
        if (Util::IsPlayer(this))
        {
          Printer::Instance().AddMessage("You can move again!");
        }

        DispelEffectsAllOf(ItemBonusType::PARALYZE);
      }
    }
    break;

    case ItemBonusType::WEAKNESS:
    {
      for (auto& kvp : _weaknessPenaltyStats)
      {
        int penalty = kvp.second.OriginalValue() / 2;
        if (penalty == 0)
        {
          penalty = 1;
        }

        kvp.second.AddModifier(e.Id, -penalty);
      }

      if (Util::IsPlayer(this))
      {
        Printer::Instance().AddMessage("You feel weak!");
      }
    }
    break;
  }
}

// =============================================================================

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

    case ItemBonusType::WEAKNESS:
    {
      for (auto& kvp : _weaknessPenaltyStats)
      {
        kvp.second.RemoveModifier(e.Id);
      }
    }
    break;
  }
}

// =============================================================================

void GameObject::RemoveEffect(const ItemBonusType& type, const uint64_t& causer)
{
  //
  // Loop goes from end to start to avoid potential skipping
  // during erasing when there are adjacent elements.
  //
  for (int i = _activeEffects.size() - 1; i >= 0; i--)
  {
    auto it = _activeEffects.begin();
    std::advance(it, i);

    bool shouldErase = false;
    for (ItemBonusStruct& bonus : it->second)
    {
      if (bonus.Type == type && bonus.Id == causer)
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

// =============================================================================

void GameObject::AttachTrigger(TriggerType type,
                               const std::function<bool ()>& condition,
                               const std::function<void ()>& handler)
{
  AddComponent<TriggerComponent>(type, condition, handler);
}

// =============================================================================

void GameObject::DispelEffectFirstFound(const ItemBonusType& t)
{
  for (int i = _activeEffects.size() - 1; i >= 0; i--)
  {
    auto it = _activeEffects.begin();
    std::advance(it, i);

    bool shouldErase = false;
    for (ItemBonusStruct& bonus : it->second)
    {
      if (bonus.Type == t && !bonus.Persistent)
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

// =============================================================================

void GameObject::DispelEffectsAllOf(const ItemBonusType& type)
{
  for (int i = _activeEffects.size() - 1; i >= 0; i--)
  {
    auto it = _activeEffects.begin();
    std::advance(it, i);

    bool shouldErase = false;
    for (ItemBonusStruct& bonus : it->second)
    {
      if (bonus.Type == type && !bonus.Persistent)
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

#ifdef DEBUG_BUILD

// =============================================================================

void GameObject::DispelEffects()
{
  for (int i = _activeEffects.size() - 1; i >= 0; i--)
  {
    auto it = _activeEffects.begin();
    std::advance(it, i);

    bool shouldErase = false;
    for (ItemBonusStruct& bonus : it->second)
    {
      if (!bonus.Persistent)
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

#endif

// =============================================================================

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

// =============================================================================

void GameObject::ProcessEffects()
{
  auto ProcessEffect = [this](ItemBonusStruct& ibs)
  {
    if (ibs.Period > 0)
    {
      ibs.EffectCounter++;

      if ((ibs.EffectCounter % ibs.Period) == 0)
      {
        ibs.EffectCounter = 0;
        EffectAction(ibs);
      }
    }
    else
    {
      EffectAction(ibs);
    }
  };

  for (int i = _activeEffects.size() - 1; i >= 0; i--)
  {
    auto it = _activeEffects.begin();
    std::advance(it, i);

    auto& ae = _activeEffects[it->first];
    for (int j = ae.size() - 1; j >= 0; j--)
    {
      if (ae[j].Duration > 0)
      {
        ProcessEffect(ae[j]);

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
        ProcessEffect(ae[j]);
      }
    }

    if (ae.empty())
    {
      _activeEffects.erase(it);
    }
  }
}

// =============================================================================

void GameObject::ProcessItemsEffects()
{
  ContainerComponent* inventory = GetComponent<ContainerComponent>();
  if (inventory != nullptr)
  {
    for (auto& item : inventory->Contents)
    {
      ItemComponent* ic = item->GetComponent<ItemComponent>();
      if (ic != nullptr)
      {
        for (auto& bonus : ic->Data.Bonuses)
        {
          switch (bonus.Type)
          {
            case ItemBonusType::SELF_REPAIR:
            {
              bonus.EffectCounter++;

              if ((bonus.EffectCounter % bonus.Period) == 0)
              {
                bonus.EffectCounter = 0;
                ic->Data.Durability.AddMin(bonus.BonusValue);
              }
            }
            break;
          }
        }
      }
    }
  }
}

// =============================================================================

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

// =============================================================================

void GameObject::MarkAndCreateRemains()
{
  //
  // Loot is not created on dangerous tiles.
  //
  bool tileDangerous = Map::Instance().IsTileDangerous({ PosX, PosY });

  //
  // Incorporeal monsters don't leave remains.
  //
  if (!tileDangerous && Corporeal)
  {
    // Destroying remains should not spawn another remains.
    if (Type != GameObjectType::REMAINS)
    {
      auto go = GameObjectsFactory::Instance().CreateRemains(this);
      _levelOwner->PlaceGameObject(go);
    }

    DropItemsHeld();
  }

  IsDestroyed = true;
}

// =============================================================================

IR GameObject::Interact()
{
  if (Util::IsFunctionValid(InteractionCallback))
  {
    return InteractionCallback();
  }

  return { InteractionResult::UNDEFINED, GameStates::UNDEIFNED };
}

// =============================================================================

void GameObject::SetLevelOwner(MapLevelBase *levelOwner)
{
  _levelOwner = levelOwner;
}

// =============================================================================

size_t GameObject::ComponentsSize()
{
  return _components.size();
}

// =============================================================================

void GameObject::AwardExperience(int amount)
{
  int amnt = amount;

  if (amount > 0)
  {
    amnt = amount * (Attrs.Exp.Talents + 1);
  }

  Attrs.Exp.AddMin(amnt);

  if (Attrs.Exp.Min().Get() >= Attrs.Exp.Max().Get())
  {
    int overflow = amnt - Attrs.Exp.Max().Get();

    LevelUp();

    int expToLvlUp = Util::GetExpForNextLevel(Attrs.Lvl.Get());

    overflow = Util::Clamp(overflow, 0, expToLvlUp - 1);

    Attrs.Exp.SetMin(overflow);
    Attrs.Exp.SetMax(expToLvlUp);
  }
  else if (amnt < 0
        && Attrs.Lvl.Get() != 1
        && Attrs.Exp.Min().Get() <= 0)
  {
    LevelDown();

    int expToLvlUp = Util::GetExpForNextLevel(Attrs.Lvl.Get());

    //
    // Actual subtraction since amnt < 0
    //
    int underflow = expToLvlUp + amnt;

    underflow = Util::Clamp(underflow, 0, expToLvlUp - 1);

    Attrs.Exp.SetMin(underflow);
    Attrs.Exp.SetMax(expToLvlUp);
  }
}

// =============================================================================

void GameObject::LevelUp(int baseHpOverride)
{
  int gainedLevel = Attrs.Lvl.Get() + 1;

  if (_levelUpHistory.count(gainedLevel) == 1)
  {
    LevelUpFromHistory(gainedLevel, true);
  }
  else
  {
    LevelUpNatural(gainedLevel, baseHpOverride);
  }

  Attrs.Lvl.Add(1);
}

// =============================================================================

void GameObject::LevelDown()
{
  int levelFrom = Attrs.Lvl.Get();

  LevelUpFromHistory(levelFrom, false);

  Attrs.Lvl.Add(-1);
  if (Attrs.Lvl.OriginalValue() <= 1)
  {
    Attrs.Lvl.Set(1);
  }
}

// =============================================================================

void GameObject::LevelUpFromHistory(int gainedLevel, bool positive)
{
  auto& data = _levelUpHistory[gainedLevel];

  const std::unordered_map<PlayerStats, Attribute&> mainAttrs =
  {
    { PlayerStats::STR, Attrs.Str },
    { PlayerStats::DEF, Attrs.Def },
    { PlayerStats::MAG, Attrs.Mag },
    { PlayerStats::RES, Attrs.Res },
    { PlayerStats::SKL, Attrs.Skl },
    { PlayerStats::SPD, Attrs.Spd }
  };

  for (auto& kvp : data)
  {
    if (mainAttrs.count(kvp.first) == 1)
    {
      int toAdd = positive ? kvp.second : -kvp.second;
      mainAttrs.at(kvp.first).Add(toAdd);
    }
  }

  Attrs.HP.AddMax(positive
                ? data[PlayerStats::HP]
               : -data[PlayerStats::HP]);

  Attrs.MP.AddMax(positive
                ? data[PlayerStats::MP]
               : -data[PlayerStats::MP]);
}

// =============================================================================

void GameObject::LevelUpNatural(int gainedLevel, int baseHpOverride)
{
  _levelUpHistory[gainedLevel] =
  {
    { PlayerStats::STR, 0 },
    { PlayerStats::DEF, 0 },
    { PlayerStats::MAG, 0 },
    { PlayerStats::RES, 0 },
    { PlayerStats::SKL, 0 },
    { PlayerStats::SPD, 0 },
    { PlayerStats::HP,  0 },
    { PlayerStats::MP,  0 }
  };

  for (auto& i : _mainAttributesByIndex)
  {
    auto kvp = i.second;

    if (CanRaiseAttribute(kvp.second))
    {
      kvp.second.Add(1);
      _levelUpHistory[gainedLevel][kvp.first] = 1;
    }
  }

  // HP and MP

  int minRndHp = (Attrs.HP.Talents + 1);
  int maxRndHp = 2 * minRndHp;

  int hpToAdd = RNG::Instance().RandomRange(minRndHp, maxRndHp + 1);

  if (baseHpOverride != -1)
  {
    hpToAdd = baseHpOverride;
  }

  Attrs.HP.AddMax(hpToAdd);

  _levelUpHistory[gainedLevel][PlayerStats::HP] = hpToAdd;

  int minRndMp = Attrs.Mag.OriginalValue();
  int maxRndMp = Attrs.Mag.OriginalValue() + Attrs.MP.Talents;

  int mpToAdd = RNG::Instance().RandomRange(minRndMp, maxRndMp + 1);
  Attrs.MP.AddMax(mpToAdd);

  _levelUpHistory[gainedLevel][PlayerStats::MP] = mpToAdd;
}

// =============================================================================

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

// =============================================================================

const std::unordered_map<uint64_t, std::vector<ItemBonusStruct>>& GameObject::GetActiveEffects()
{
  return _activeEffects;
}

// =============================================================================

const std::map<int, std::map<PlayerStats, int>>& GameObject::GetLevelUpHistory()
{
  return _levelUpHistory;
}

// =============================================================================

const uint64_t& GameObject::ObjectId()
{
  return _objectId;
}

// =============================================================================

void GameObject::MaskToBoolFlags(const uint16_t mask)
{
  std::map<int, bool&> traverseMap =
  {
    { 0, Special     },
    { 1, Blocking    },
    { 2, BlocksSight },
    { 3, Revealed    },
    { 4, Corporeal   },
    { 5, Visible     },
    { 6, IsLiving    }
  };

  uint64_t index = 0x1;

  for (auto& kvp : traverseMap)
  {
    kvp.second = (mask & index);
    index <<= 1;
  }
}

// =============================================================================

void GameObject::Serialize(NRS& section)
{
  namespace SK = Strings::SerializationKeys;

  NRS* ptr = nullptr;

  switch (Type)
  {
    //
    // NOTE: will probably contain a lot of common code with actors.
    //
    case GameObjectType::PLAYER:
    {
      ptr = &section[SK::Player];

      Player* p = static_cast<Player*>(this);

      NRS& n = *ptr;

      n[SK::Class].SetInt(p->SelectedClass);
      n[SK::Name].SetString(p->Name);
      n[SK::Pos].SetInt(PosX, 0);
      n[SK::Pos].SetInt(PosY, 1);
      n[SK::Owner].SetInt((int)_levelOwner->MapType_);
    }
    break;

    // -------------------------------------------------------------------------

    case GameObjectType::DOOR:
    {
      ptr = &section;

      DoorComponent* dc = GetComponent<DoorComponent>();
      if (dc != nullptr)
      {
        NRS& n = *ptr;
      }
    }
    break;

    // -------------------------------------------------------------------------

    default:
      break;
  }

  if (ptr != nullptr)
  {
    NRS& n = *ptr;

    n[SK::Id].SetUInt(_objectId);
    n[SK::Image].SetInt(Image);
    n[SK::Color].SetString(Util::NumberToHexString(FgColor), 0);
    n[SK::Color].SetString(Util::NumberToHexString(BgColor), 1);
    n[SK::Pos].SetInt(PosX, 0);
    n[SK::Pos].SetInt(PosY, 1);

    uint16_t mask = Util::BoolFlagsToMask({
                                            Special,
                                            Blocking,
                                            BlocksSight,
                                            Revealed,
                                            Corporeal,
                                            Visible,
                                            IsLiving
                                          });

    n[SK::Mask].SetUInt(mask);
    n[SK::Type].SetInt((int)Type);
  }
}

// =============================================================================

const GameObject::SaveDataMinimal& GameObject::GetSaveDataMinimal()
{
  _sdm.Type       = Type;
  _sdm.ZoneMarker = ZoneMarker;
  _sdm.Image      = Image;
  _sdm.PosX       = PosX;
  _sdm.PosY       = PosY;
  _sdm.FgColor    = FgColor;
  _sdm.BgColor    = BgColor;
  _sdm.Name       = ObjectName;
  _sdm.FowName    = FogOfWarName;
  _sdm.Mask       = Util::BoolFlagsToMask({
                                            Special,
                                            Blocking,
                                            BlocksSight,
                                            Revealed,
                                            Corporeal,
                                            Visible,
                                            IsLiving
                                          });

  return _sdm;
}

// =============================================================================

#ifdef DEBUG_BUILD
std::vector<std::string> GameObject::DebugInfo()
{
  std::vector<std::string> res;

  res.push_back("{");

  std::string str = Util::StringFormat("  [0x%X]", this);
  res.push_back(str);

  str = Util::StringFormat("  ObjName: %s", ObjectName.data());
  res.push_back(str);

  str = Util::StringFormat("  ObjectId: %llu", _objectId);
  res.push_back(str);

  std::string ch = { (char)Image };
  str = Util::StringFormat("  Image: '%s'", ch.data());

  res.push_back(str);

  str = Util::StringFormat("  Position: { %i, %i }", PosX, PosY);
  res.push_back(str);

  str = Util::StringFormat("  FgColor: %06X", FgColor);
  res.push_back(str);

  str = Util::StringFormat("  BgColor: %06X", BgColor);
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
    str = Util::StringFormat("    %llu (%zu):", kvp.first, kvp.second.size());
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
