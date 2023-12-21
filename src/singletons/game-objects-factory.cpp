#include "game-objects-factory.h"

#include "constants.h"

#include "application.h"
#include "rng.h"
#include "map.h"
#include "printer.h"
#include "monsters-inc.h"
#include "items-factory.h"

#include "shrine-component.h"
#include "stairs-component.h"
#include "door-component.h"

#include "game-object-info.h"
#include "timed-destroyer-component.h"

void GameObjectsFactory::InitSpecific()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

// =============================================================================

GameObject* GameObjectsFactory::CreateShrine(int x, int y,
                                             ShrineType type,
                                             int timeout)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->PosX = x;
  go->PosY = y;
  go->Image = '/';

  go->FgColor = Colors::ShrineColorsByType.at(type).first;
  go->BgColor = Colors::ShrineColorsByType.at(type).second;

  go->Type = GameObjectType::SHRINE;

  bool oneTimeUse = (type != ShrineType::POTENTIAL);

  ShrineComponent* sc = go->AddComponent<ShrineComponent>(type, timeout, oneTimeUse);

  go->ObjectName = GlobalConstants::ShrineNameByType.at(type);
  go->FogOfWarName = "?Shrine?";
  go->InteractionCallback = std::bind(&ShrineComponent::Interact, sc);

  return go;
}

// =============================================================================

GameObject* GameObjectsFactory::CreateRemains(GameObject* from)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel, from->PosX, from->PosY, '%', from->FgColor, from->BgColor);

  go->Type = GameObjectType::REMAINS;

  go->RemainsOf = from->ObjectId();

  //
  // Living creatures leave decomposable corpses.
  //
  if (from->IsLiving)
  {
    //int timeout = from->Attrs.HP.Max().Get() * 10;
    go->AddComponent<TimedDestroyerComponent>(200);
  }

  std::string objName = from->ObjectName;

  ItemComponent* ic = from->GetComponent<ItemComponent>();
  if (ic != nullptr)
  {
    objName = (!ic->Data.IsIdentified)
              ? ic->Data.UnidentifiedName
              : from->ObjectName;
  }

  auto str = Util::StringFormat("%s's remains", objName.data());
  go->ObjectName = str;

  go->Attrs.Indestructible = false;
  go->Attrs.HP.Reset(1);

  return go;
}

// =============================================================================

void GameObjectsFactory::CreateStairs(MapLevelBase* levelWhereCreate,
                                      int x, int y,
                                      int image,
                                      MapType leadsTo)
{
  auto tile = levelWhereCreate->MapArray[x][y].get();

  auto c = tile->AddComponent<StairsComponent>();
  StairsComponent* stairs = static_cast<StairsComponent*>(c);
  stairs->LeadsTo = leadsTo;

  tile->ObjectName = (image == '>') ? "Stairs Down" : "Stairs Up";
  tile->FgColor = Colors::WhiteColor;
  tile->BgColor = Colors::DoorHighlightColor;
  tile->Image = image;
  tile->Type = GameObjectType::STAIRS;
}

// =============================================================================

GameObject* GameObjectsFactory::CreateDummyObject(int x,
                                                  int y,
                                                  const std::string& objName,
                                                  char image,
                                                  const uint32_t& fgColor,
                                                  const uint32_t& bgColor)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->Image = image;
  go->ObjectName = objName;
  go->PosX = x;
  go->PosY = y;
  go->FgColor = fgColor;
  go->BgColor = bgColor;

  return go;
}

// =============================================================================

GameObject* GameObjectsFactory::CreateChest(int x, int y, bool isLocked)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->ObjectName  = "Iron Chest";
  go->PosX        = x;
  go->PosY        = y;
  go->Image       = 'C';
  go->FgColor     = Colors::BlackColor;
  go->BgColor     = Colors::IronColor;
  go->Blocking    = true;
  go->BlocksSight = true;
  go->Type        = GameObjectType::CONTAINER;

  ContainerComponent* cc = go->AddComponent<ContainerComponent>();

  //cc->CanBeOpened = !isLocked;

  int chance = 100;

  const double scale = 1.6;

  for (int i = 0; i < 10; i++)
  {
    if (Util::Rolld100(chance))
    {
      GameObject* go = ItemsFactory::Instance().CreateRandomItem(0, 0);
      cc->Add(go);
      chance = (int)((double)chance / scale);
    }
    else
    {
      break;
    }
  }

  go->InteractionCallback = std::bind(&ContainerComponent::Interact, cc);

  return go;
}

// =============================================================================

GameObject* GameObjectsFactory::CreateContainer(int x,
                                                int y,
                                                int image,
                                                const std::string& name,
                                                const uint32_t& bgColor)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->ObjectName  = name;
  go->PosX        = x;
  go->PosY        = y;
  go->Image       = image;
  go->FgColor     = Colors::WhiteColor;
  go->BgColor     = bgColor;
  go->Blocking    = true;
  go->BlocksSight = true;
  go->Type        = GameObjectType::CONTAINER;

  ContainerComponent* cc = go->AddComponent<ContainerComponent>();

  go->InteractionCallback = std::bind(&ContainerComponent::Interact, cc);

  return go;
}

// =============================================================================

GameObject* GameObjectsFactory::CreateDoor(int x, int y,
                                           bool isOpen,
                                           DoorMaterials material,
                                           const std::string& doorName,
                                           int hitPoints,
                                           const uint32_t& fgOverrideColor,
                                           const uint32_t& bgOverrideColor)
{
  std::string doorNameTotal;
  if (doorName.empty())
  {
    doorNameTotal = _doorPrefixByMat.at(material) + " Door";
  }
  else
  {
    doorNameTotal = doorName;
  }

  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->PosX = x;
  go->PosY = y;

  go->Type = GameObjectType::DOOR;

  if (hitPoints > 0)
  {
    go->Attrs.Indestructible = false;
    go->Attrs.Def.Set(_doorDefByMat.at(material));
    go->Attrs.HP.Reset(hitPoints);
  }

  DoorComponent* dc = go->AddComponent<DoorComponent>();

  dc->Material        = material;
  dc->IsOpen          = isOpen;
  dc->FgColorOverride = fgOverrideColor;
  dc->BgColorOverride = bgOverrideColor;

  dc->UpdateDoorState();

  dc->OwnerGameObject->InteractionCallback = std::bind(&DoorComponent::Interact, dc);
  dc->OwnerGameObject->ObjectName = doorNameTotal;

  return go;
}

// =============================================================================

GameObject* GameObjectsFactory::CreateStaticObject(int x,
                                                   int y,
                                                   const GameObjectInfo& objectInfo,
                                                   int hitPoints,
                                                   GameObjectType type)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->PosX         = x;
  go->PosY         = y;
  go->Image        = objectInfo.Image;
  go->ObjectName   = objectInfo.ObjectName;
  go->FogOfWarName = objectInfo.FogOfWarName;
  go->FgColor      = objectInfo.FgColor;
  go->BgColor      = objectInfo.BgColor;
  go->Blocking     = objectInfo.IsBlocking;
  go->BlocksSight  = objectInfo.BlocksSight;
  go->Type         = type;

  if (hitPoints > 0)
  {
    go->Attrs.Indestructible = false;
    go->Attrs.HP.Reset(hitPoints);
  }

  return go;
}

// =============================================================================

ItemComponent* GameObjectsFactory::CloneItem(ItemComponent* copyFrom)
{
  GameObject* copy = CloneObject(copyFrom->OwnerGameObject);

  ItemComponent* ic = copy->AddComponent<ItemComponent>();
  ic->Data = copyFrom->Data;

  return ic;
}

// =============================================================================

GameObject* GameObjectsFactory::CloneObject(GameObject* copyFrom)
{
  GameObject* copy = new GameObject(Map::Instance().CurrentLevel);

  // NOTE: don't forget to copy any newly added fields

  copy->VisibilityRadius = copyFrom->VisibilityRadius;
  copy->PosX             = copyFrom->PosX;
  copy->PosY             = copyFrom->PosY;
  copy->ZoneMarker       = copyFrom->ZoneMarker;
  copy->Special          = copyFrom->Special;
  copy->Blocking         = copyFrom->Blocking;
  copy->BlocksSight      = copyFrom->BlocksSight;
  copy->Revealed         = copyFrom->Revealed;
  copy->Corporeal        = copyFrom->Corporeal;
  copy->Visible          = copyFrom->Visible;
  copy->Occupied         = copyFrom->Occupied;
  copy->IsDestroyed      = copyFrom->IsDestroyed;
  copy->Image            = copyFrom->Image;
  copy->FgColor          = copyFrom->FgColor;
  copy->BgColor          = copyFrom->BgColor;
  copy->ObjectName       = copyFrom->ObjectName;
  copy->FogOfWarName     = copyFrom->FogOfWarName;

  copy->InteractionCallback = copyFrom->InteractionCallback;

  copy->StackObjectId    = copyFrom->StackObjectId;
  copy->Attrs            = copyFrom->Attrs;
  copy->HealthRegenTurns = copyFrom->HealthRegenTurns;
  copy->Type             = copyFrom->Type;
  copy->IsLiving         = copyFrom->IsLiving;

  copy->_activeEffects           = copyFrom->_activeEffects;
  copy->_previousCell            = copyFrom->_previousCell;
  copy->_currentCell             = copyFrom->_currentCell;
  copy->_healthRegenTurnsCounter = copyFrom->_healthRegenTurnsCounter;
  copy->_manaRegenTurnsCounter   = copyFrom->_manaRegenTurnsCounter;

  return copy;
}

// =============================================================================

GameObject* GameObjectsFactory::CreateBreakableObjectWithRandomLoot(int x,
                                                                    int y,
                                                                    char image,
                                                                    const std::string& objName,
                                                                    const uint32_t& fgColor,
                                                                    const uint32_t& bgColor)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->PosX = x;
  go->PosY = y;

  go->FgColor     = fgColor;
  go->BgColor     = bgColor;
  go->Image       = image;
  go->ObjectName  = objName;
  go->Blocking    = true;
  go->BlocksSight = true;

  go->Attrs.Indestructible = false;
  go->Attrs.HP.Reset(1);

  int dungeonLevel = Map::Instance().CurrentLevel->DungeonLevel;
  int maxLevel = (int)MapType::THE_END;

  ContainerComponent* cc = go->AddComponent<ContainerComponent>(maxLevel + 1);
  cc->CanBeOpened = false;

  int maxItems = RNG::Instance().RandomRange(1, 10);

  int nothingChance   = (int)((double)maxLevel * 0.4);
  int somethingChance = dungeonLevel;

  double failScale = 1.25;

  const std::unordered_map<ItemType, int> weights =
  {
    { ItemType::NOTHING, nothingChance },
    { ItemType::DUMMY, somethingChance }
  };

  for (int i = 0; i < maxItems; i++)
  {
    auto res = Util::WeightedRandom(weights);
    if (res.first != ItemType::NOTHING)
    {
      //
      // NOTE: Not all objects may have been added
      // to the factory yet, so check against nullptr is needed.
      //
      auto item = ItemsFactory::Instance().CreateRandomItem(0, 0);
      if (item != nullptr)
      {
        ItemComponent* ic = item->GetComponent<ItemComponent>();
        //
        // If cc->Add() is true that means 'item' is a stackable object
        // and appropriate stack to merge into was found
        // inside the 'cc' container.
        //
        if (!Util::CanBeSpawned(ic) || cc->Add(item))
        {
          //
          // Yes, we're deleting the object we've just created.
          //
          delete item;
        }
      }
    }
    else
    {
      maxItems = (int)((double)maxItems / failScale);
    }
  }

  return go;
}

// =============================================================================

void GameObjectsFactory::CreateTrigger(TriggerType triggerType,
                                       TriggerUpdateType updateType,
                                       const std::function<bool ()>& condition,
                                       const std::function<void ()>& handler)
{
  GameObject* triggerObject = new GameObject(Map::Instance().CurrentLevel);
  triggerObject->AttachTrigger(triggerType, condition, handler);
  Map::Instance().CurrentLevel->PlaceTrigger(triggerObject, updateType);
}
