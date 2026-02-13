#include "game-object.h"

#include "application.h"
#include "blackboard.h"
#include "gid-generator.h"
#include "items-factory.h"
#include "printer.h"
#include "map.h"
#include "rng.h"
#include "game-objects-factory.h"

#include "map-level-base.h"
#include "game-object-info.h"
#include "serializer.h"

#include "ai-component.h"
#include "door-component.h"
#include "container-component.h"
#include "trigger-component.h"
#include "item-component.h"

#ifdef DEBUG_BUILD
#include "dev-console.h"
#endif

GameObject::GameObject(MapLevelBase* levelOwner)
{
  LevelOwner = levelOwner;
  VisibilityRadius.Set(0);

  _objectId = Game::gGid.GenerateGlobalId();

#ifdef DEBUG_BUILD
  GameObjectsById[_objectId] = this;
  HexAddressString = Util::StringFormat("0x%" PRIXLEAST64, this);

  //
  // If we don't use any bullshit like multiple or virtual inheritance,
  // address of the derived class will be the same as its base class.
  // So we can assign it nicely here.
  //
  AnyObjectByAddr[this] = this;
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
  _objectId = Game::gGid.GenerateGlobalId();

  Init(levelOwner, x, y, avatar, htmlColor, bgColor);

#ifdef DEBUG_BUILD
  GameObjectsById[_objectId] = this;
  AnyObjectByAddr[this] = this;
  HexAddressString = Util::StringFormat("0x%" PRIXLEAST64, this);
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
    Game::gBB.Remove(_objectId);
  }

  if (Util::IsFunctionValid(OnDestroy))
  {
    OnDestroy(this);
  }

#ifdef DEBUG_BUILD
  GameState* s =
      Game::gApp.GetGameStateRefByName(GameStates::DEV_CONSOLE);
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

  AnyObjectByAddr.erase(this);
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

  LevelOwner = levelOwner;

  //
  // _currentCell->Occupied is not set to true by default,
  // see game-object.h comments for Occupied field.
  //
  if (LevelOwner != nullptr)
  {
    _currentCell  = LevelOwner->MapArray[PosX][PosY].get();
    _previousCell = LevelOwner->MapArray[PosX][PosY].get();
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
    if (PosX < LevelOwner->MapSize.X && PosY < LevelOwner->MapSize.Y)
    {
      _previousCell = LevelOwner->MapArray[PosX][PosY].get();
      _previousCell->Occupied = false;
    }

    PosX = x;
    PosY = y;

    //DebugLog("MoveTo(%i, %i)\n", x, y);

    _currentCell = LevelOwner->MapArray[PosX][PosY].get();
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

  auto curLvl = Game::gMap.CurrentLevel;

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

bool GameObject::IsSwimming()
{
  bool isFlying       = HasEffect(ItemBonusType::LEVITATION);
  bool isWaterWalking = HasEffect(ItemBonusType::WATER_WALKING);
  bool canSwim        = (GlobalConstants::CanSwimMap.count(Type) == 1
                      && GlobalConstants::CanSwimMap.at(Type) == true);
  bool isOnDeepWater  = (_currentCell->Type == GameObjectType::DEEP_WATER);

  return (isOnDeepWater && canSwim && !isFlying && !isWaterWalking);
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

  uint32_t fgColor = FgColor;
  uint32_t bgColor = BgColor;

  if (overrideColorFg != Colors::None && overrideColorFg != FgColor)
  {
    fgColor = overrideColorFg;
  }

  if (overrideColorBg != Colors::None && overrideColorBg != BgColor)
  {
    bgColor = overrideColorBg;
  }

  //
  // SDL build uses transparency as Colors::None,
  // so without this check it will cause objects from breakable containers
  // to overlay with container's remains which doesn't look good.
  //
  if (bgColor == Colors::None)
  {
    bgColor = Colors::Black;
  }

  int x = PosX + LevelOwner->MapOffsetX;
  int y = PosY + LevelOwner->MapOffsetY;

#ifdef USE_SDL
  //
  // If no custom tileset loaded or GameObject has no graphic tile set.
  //
  bool useGraphicsTile =
      (Game::gApp.AppData.UseGraphics && Graphic.Tile != GraphicTiles::NONE);

  if (useGraphicsTile)
  {
    Game::gPrnt.DrawGraphicsTileExt(x, y, Graphic);
  }
  else
  {
    Game::gPrnt.DrawSubstituteGraphicsTile(x,
                                           y,
                                           (int)NameCP437::BLOCK,
                                           bgColor);

    Game::gPrnt.DrawSubstituteGraphicsTile(x,
                                           y,
                                           (imageOverride != -1)
                                           ? imageOverride
                                           : Image,
                                           fgColor);
  }
#else
  Game::gPrnt.PrintFB(x,
                      y,
                      (imageOverride != -1)
                      ? imageOverride
                      : Image,
                      fgColor,
                      bgColor);
#endif
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
      _attributesRefsByBonus.at(bonus.Type).AddModifier(
            itemRef->OwnerGameObject->ObjectId(),
            bonus.BonusValue
      );
      break;

    case ItemBonusType::HP:
    case ItemBonusType::MP:
      _rangedAttributesRefsByBonus.at(bonus.Type).Max().AddModifier(
            itemRef->OwnerGameObject->ObjectId(),
            bonus.BonusValue
      );
      _rangedAttributesRefsByBonus.at(bonus.Type).CheckOverflow();
      break;

    case ItemBonusType::VISIBILITY:
      VisibilityRadius.AddModifier(itemRef->OwnerGameObject->ObjectId(),
                                   bonus.BonusValue);
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
      _attributesRefsByBonus.at(bonus.Type).RemoveModifier(
            itemRef->OwnerGameObject->ObjectId()
      );
      break;

    case ItemBonusType::HP:
    case ItemBonusType::MP:
      _rangedAttributesRefsByBonus.at(bonus.Type).Max().RemoveModifier(
            itemRef->OwnerGameObject->ObjectId()
      );
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
  Graphic      = t.Graphic;
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

      std::string verb = !Util::ShouldAwardExp(Type) ? "destroyed" : "killed";

      logMessages.push(Util::StringFormat("%s was %s",
                                          objName.data(),
                                          verb.data()));
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

  bool tileVisible =
      Game::gMap.CurrentLevel->MapArray[PosX][PosY]->Visible;

  if (!suppressLog && tileVisible)
  {
    while (!logMessages.empty())
    {
      auto msg = logMessages.front();
      Game::gPrnt.AddMessage(msg);
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
  return (HasNonZeroHP()
      && (Attrs.ActionMeter >= GlobalConstants::TurnReadyValue));
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
  if (Game::gMap.CurrentLevel->Peaceful)
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

  _currentCell = LevelOwner->MapArray[PosX][PosY].get();

  switch (_currentCell->Type)
  {
    case GameObjectType::DEEP_WATER:
    {
      res = !IsSwimming();
    }
    break;

    case GameObjectType::LAVA:
    {
      res = !(HasEffect(ItemBonusType::LAVA_IMMUNE)
           || HasEffect(ItemBonusType::WATER_WALKING)
           || HasEffect(ItemBonusType::LEVITATION));
    }
    break;

    case GameObjectType::CHASM:
    {
      res = !HasEffect(ItemBonusType::LEVITATION);
    }
    break;

    default:
      break;
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
  Game::gMap.UpdateTriggers(TriggerUpdateType::FINISH_TURN);

  //
  // NOTE: do not call Game::gMap.RemoveDestroyed() to reduce iterations over
  // collections on actors. We will do this only on player turns.
  //

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
      ic->OwnerGameObject->LevelOwner = Game::gMap.CurrentLevel;
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
    auto money = Game::gIF.CreateMoney(Money);
    money->PosX = PosX;
    money->PosY = PosY;
    Game::gMap.CurrentLevel->PlaceGameObject(money);
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
  _previousCell = Game::gMap.CurrentLevel->MapArray[PosX][PosY].get();
  _previousCell->Occupied = false;

  PosX += dx;
  PosY += dy;

  _currentCell = Game::gMap.CurrentLevel->MapArray[PosX][PosY].get();
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
  auto str =
    Util::StringFormat("%s gained %s (duration %i period %i)",
                       ObjectName.data(),
                       GlobalConstants::BonusDisplayNameByType
                       .count(effectToAdd.Type) == 1 ?
                       GlobalConstants::BonusDisplayNameByType
                       .at(effectToAdd.Type).data() :
                       "<effect name not found>",
                       effectToAdd.Duration,
                       effectToAdd.Period);
  Game::gLogger.Print(str);
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
        Game::gPrnt.AddMessage("You catch fire!");
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
          Game::gPrnt.AddMessage("The poison disperses!");
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
          Game::gPrnt.AddMessage("You can move again!");
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
        Game::gPrnt.AddMessage("You feel weak!");
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

void GameObject::RemoveEffect(const ItemBonusType& type,
                              const uint64_t& causer)
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
        DebugLog("\t%s ProcessEffects() [%s] duration %i = %i - 1",
                 ObjectName.data(),
                 GlobalConstants::BonusDisplayNameByType.at(ae[j].Type).data(),
                 ae[j].Duration,
                 ae[j].Duration);
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
  bool tileDangerous = Game::gMap.IsTileDangerous({ PosX, PosY });

  //
  // If object is already marked as destroyed, don't do anything.
  // This can happen in situation when fast monster can break a container
  // twice because he has actions left after his FinishTurn().
  // Since actual cleanup of destroyed objects only happens on player's
  // FinishTurn(), there can basically be a "double free" situation.
  // First time object gets destroyed its items dropped, but during second
  // attempt items are no longer present in a container (they became nullptr
  // since they were transferred to the floor).
  // Incorporeal monsters also don't leave remains (duh).
  //
  if (!IsDestroyed && !tileDangerous && Corporeal)
  {
    //
    // Destroying remains should not spawn another remains.
    //
    if (Type != GameObjectType::REMAINS)
    {
      auto go = Game::gGOF.CreateRemains(this);
      LevelOwner->PlaceGameObject(go);
      DropItemsHeld();
    }
  }

  Destroy();
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

  int hpToAdd = Game::gRng.RandomRange(minRndHp, maxRndHp + 1);

  if (baseHpOverride != -1)
  {
    hpToAdd = baseHpOverride;
  }

  Attrs.HP.AddMax(hpToAdd);

  _levelUpHistory[gainedLevel][PlayerStats::HP] = hpToAdd;

  int minRndMp = Attrs.Mag.OriginalValue();
  int maxRndMp = Attrs.Mag.OriginalValue() + Attrs.MP.Talents;

  int mpToAdd = Game::gRng.RandomRange(minRndMp, maxRndMp + 1);
  Attrs.MP.AddMax(mpToAdd);

  _levelUpHistory[gainedLevel][PlayerStats::MP] = mpToAdd;
}

// =============================================================================

bool GameObject::CanRaiseAttribute(Attribute& attr)
{
  bool customChance = (attr.RaiseProbability >= 0);
  int chance = customChance
               ? attr.RaiseProbability
               : GlobalConstants::AttributeMinimumRaiseChance;

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

const std::unordered_map<uint64_t, std::vector<ItemBonusStruct>>&
GameObject::GetActiveEffects()
{
  return _activeEffects;
}

// =============================================================================

const std::map<int, std::map<PlayerStats, int>>&
GameObject::GetLevelUpHistory()
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
      n[SK::Owner].SetInt((int)LevelOwner->MapType_);
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

        std::ignore = n;

        // TODO:
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

std::string GameObject::SaveDataMinimal::ToStringKey() const
{
  std::stringstream ss;

  ss << (int)Type
     << (int)ZoneMarker
     << Image
     << Util::NumberToHexString(FgColor)
     << Util::NumberToHexString(BgColor)
     << Name
     << FowName
     << Mask;

  return ss.str();
}

// =============================================================================

void GameObject::Destroy()
{
  Attrs.HP.SetMin(0);
  IsDestroyed = true;
  Game::gMap.AddGameObjectToDestroyQueue(this);
}

// =============================================================================

#ifdef DEBUG_BUILD
StringV GameObject::Dump(size_t indent)
{
  const std::string spaces(indent, ' ');

  StringV res;

  res.push_back( I_OBJ_START(spaces, this) );

  res.push_back( I_STR(spaces, ObjectName) );
  res.push_back( I_STR(spaces, FogOfWarName) );
  res.push_back( I_ULL(spaces, _objectId) );

  std::string ch = { (char)Image };
  res.push_back( I_STR_NAMED(spaces, STRINGIFY(Image), ch) );

  Position pos = { PosX, PosY };
  res.push_back( I_STR_NAMED(spaces, STRINGIFY(Position), pos.ToString()) );

  res.push_back( I_CLR(spaces, FgColor) );
  res.push_back( I_CLR(spaces, BgColor) );

  auto vr = VisibilityRadius.Dump(STRINGIFY(VisibilityRadius), indent + 2);
  for (auto& i : vr)
  {
    res.push_back(i);
  }

  res.push_back( I_INT(spaces, Money) );
  res.push_back( I_INT(spaces, ZoneMarker) );

  res.push_back( I_BOOL(spaces, Special) );
  res.push_back( I_BOOL(spaces, Blocking) );
  res.push_back( I_BOOL(spaces, BlocksSight) );
  res.push_back( I_BOOL(spaces, Revealed) );
  res.push_back( I_BOOL(spaces, Corporeal) );
  res.push_back( I_BOOL(spaces, Visible) );
  res.push_back( I_BOOL(spaces, Occupied) );
  res.push_back( I_BOOL(spaces, IsDestroyed) );
  res.push_back( I_BOOL(spaces, IsLiving) );

  res.push_back( I_ULL(spaces, StackObjectId) );

  auto str = Attrs.Dump(STRINGIFY(Attrs), indent + 2);
  for (auto& i : str)
  {
    res.push_back(i);
  }

  res.push_back( I_INT(spaces, HealthRegenTurns) );
  res.push_back( I_INT(spaces, Type) );
  res.push_back( I_ULL(spaces, RemainsOf) );
  res.push_back( I_PTR(spaces, _previousCell) );
  res.push_back( I_PTR(spaces, _currentCell) );
  res.push_back( I_PTR(spaces, LevelOwner) );
  res.push_back( I_INT(spaces, _healthRegenTurnsCounter) );
  res.push_back( I_INT(spaces, _manaRegenTurnsCounter) );
  res.push_back( I_INT(spaces, _skipTurnsCounter) );

  auto c = DumpComponents(indent);
  for (auto& line : c)
  {
    res.push_back(line);
  }

  auto e = DumpEffects(indent);
  for (auto& line : e)
  {
    res.push_back(line);
  }

  res.push_back( I_OBJ_END(spaces) );

  return res;
}

StringV GameObject::DumpComponents(size_t indent)
{
  const std::string spaces(indent, ' ');
  const std::string spaces2(indent + 2, ' ');

  StringV res;

  if (_components.empty())
  {
    res.push_back( I_EMPTY(spaces, STRINGIFY(_components)) );
  }
  else
  {
    res.push_back( I_OBJ_START_NAMED(spaces2, STRINGIFY(_components)) );

    for (auto& kvp : _components)
    {
      //
      // Display only addresses for brevity, you can inspect them separately.
      //
      Component* c = kvp.second.get();
      std::string name = (c != nullptr) ? (typeid(*c).name()) : "0x0";

      res.push_back( I_PTR_NAMED(spaces2, name.data(), c) );
    }

    res.push_back( I_OBJ_END(spaces2) );
  }

  return res;
}

StringV GameObject::DumpEffects(size_t indent)
{
  const std::string spaces(indent, ' ');
  const std::string spaces2(indent + 2, ' ');

  StringV res;

  if (_activeEffects.empty())
  {
    res.push_back( I_EMPTY(spaces, STRINGIFY(_activeEffects)) );
  }
  else
  {
    res.push_back( I_OBJ_START_NAMED(spaces2, STRINGIFY(_activeEffects)) );

    for (auto& kvp : _activeEffects)
    {
      for (auto& e : kvp.second)
      {
        auto lines = e.Dump(indent + 4);
        for (auto& line : lines)
        {
          res.push_back(line);
        }
      }
    }

    res.push_back( I_OBJ_END(spaces2) );
  }

  return res;
}
#endif
