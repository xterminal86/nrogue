#include "game-objects-factory.h"

#include "constants.h"

#include "application.h"
#include "rng.h"
#include "map.h"
#include "printer.h"
#include "monsters-inc.h"
#include "items-factory.h"

#include "ai-component.h"
#include "ai-npc.h"

#include "shrine-component.h"
#include "stairs-component.h"
#include "door-component.h"

#include "game-object-info.h"
#include "go-timed-destroyer.h"

void GameObjectsFactory::InitSpecific()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

GameObject* GameObjectsFactory::CreateGameObject(int x, int y, ItemType objType)
{
  // NOTE: no random items allowed

  GameObject* go = nullptr;

  switch (objType)
  {
    case ItemType::COINS:
      go = ItemsFactory::Instance().CreateMoney();
      break;

    case ItemType::HEALING_POTION:
      go = ItemsFactory::Instance().CreateHealingPotion();
      break;

    case ItemType::MANA_POTION:
      go = ItemsFactory::Instance().CreateManaPotion();
      break;

    case ItemType::NP_POTION:
      go = ItemsFactory::Instance().CreateNeutralizePoisonPotion();
      break;

    case ItemType::HUNGER_POTION:
      go = ItemsFactory::Instance().CreateHungerPotion();
      break;

    case ItemType::EXP_POTION:
      go = ItemsFactory::Instance().CreateExpPotion();
      break;

    case ItemType::STR_POTION:
    case ItemType::DEF_POTION:
    case ItemType::MAG_POTION:
    case ItemType::RES_POTION:
    case ItemType::SKL_POTION:
    case ItemType::SPD_POTION:
      go = ItemsFactory::Instance().CreateStatPotion(GlobalConstants::StatNameByPotionType.at(objType));
      break;

    case ItemType::GEM:
    {
      // The deeper you go, the more is the chance to get actual gem
      // and not worthless glass.
      int curLevel = Map::Instance().CurrentLevel->DungeonLevel;
      float chance = ((float)curLevel / (float)MapType::THE_END) * 90.0f;
      go = ItemsFactory::Instance().CreateGem(x, y, GemType::RANDOM, (int)chance);
    }
    break;

    case ItemType::RETURNER:
      go = ItemsFactory::Instance().CreateReturner(x, y);
      break;

    case ItemType::REPAIR_KIT:
      go = ItemsFactory::Instance().CreateRepairKit(x, y);
      break;

    case ItemType::NOTHING:
      go = ItemsFactory::Instance().CreateDummyItem("Dummy", 'O', Colors::YellowColor, Colors::BlackColor, { "" });
      break;

    default:
      DebugLog("CreateGameObject(): object type %i is not handled!", objType);
      break;
  }

  if (go != nullptr)
  {
    go->PosX = x;
    go->PosY = y;
  }

  return go;
}

GameObject* GameObjectsFactory::CreateMonster(int x, int y, GameObjectType monsterType)
{
  // FIXME: monsters stats are too unbalanced: they become too op very fast.
  // Change them to talents based, like player?
  GameObject* go = nullptr;

  switch (monsterType)
  {
    case GameObjectType::RAT:
      go = MonstersInc::Instance().CreateRat(x, y);
      break;

    case GameObjectType::BAT:
      go = MonstersInc::Instance().CreateBat(x, y);
      break;

    case GameObjectType::VAMPIRE_BAT:
      go = MonstersInc::Instance().CreateVampireBat(x, y);
      break;

    case GameObjectType::SPIDER:
      go = MonstersInc::Instance().CreateSpider(x, y);
      break;

    case GameObjectType::HEROBRINE:
      go = MonstersInc::Instance().CreateHerobrine(x, y);
      break;

    case GameObjectType::TROLL:
      go = MonstersInc::Instance().CreateTroll(x, y);
      break;

    case GameObjectType::MAD_MINER:
      go = MonstersInc::Instance().CreateMadMiner(x, y);
      break;

    case GameObjectType::SHELOB:
      go = MonstersInc::Instance().CreateShelob(x, y);
      break;

    default:
      DebugLog("CreateMonster(): monster type %i is not handled!", monsterType);
      break;
  }

  if (go != nullptr)
  {
    go->Type = monsterType;
    go->PosX = x;
    go->PosY = y;
  }

  return go;
}

GameObject* GameObjectsFactory::CreateShrine(int x, int y, ShrineType type, int timeout)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->PosX = x;
  go->PosY = y;
  go->Image = '/';

  go->FgColor = Colors::ShrineColorsByType.at(type).first;
  go->BgColor = Colors::ShrineColorsByType.at(type).second;

  ShrineComponent* sc = go->AddComponent<ShrineComponent>(type, timeout, timeout);

  go->ObjectName = GlobalConstants::ShrineNameByType.at(type);
  go->FogOfWarName = "?Shrine?";
  go->InteractionCallback = std::bind(&ShrineComponent::Interact, sc);

  return go;
}

GameObject* GameObjectsFactory::CreateNPC(int x, int y, NPCType npcType, bool standing, ServiceType serviceType)
{
  char img = '@';

  #ifdef USE_SDL
  img = GlobalConstants::CP437IndexByType[NameCP437::FACE_2];
  #endif

  GameObject* go = new GameObject(Map::Instance().CurrentLevel, x, y, img, "#FFFFFF");

  go->IsLiving = true;
  go->Type = GameObjectType::NPC;

  go->Move(0, 0);

  AIComponent* aic = go->AddComponent<AIComponent>();
  AINPC* ainpc = aic->AddModel<AINPC>();
  ainpc->Init(npcType, standing, serviceType);

  std::string goColor = (ainpc->Data.IsMale) ? "#FFFFFF" : "#FF00FF";
  go->FgColor = goColor;

  aic->ChangeModel<AINPC>();

  return go;
}

GameObject* GameObjectsFactory::CreateRemains(GameObject* from)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel, from->PosX, from->PosY, '%', from->FgColor, from->BgColor);

  go->Type = GameObjectType::REMAINS;

  // Living creatures leave decomposable corpses
  if (from->IsLiving)
  {
    //from->Attrs.HP.OriginalValue * 2;
    go->AddComponent<TimedDestroyerComponent>(200);
  }

  auto str = Util::StringFormat("%s's remains", from->ObjectName.data());
  go->ObjectName = str;

  go->Attrs.Indestructible = false;
  go->Attrs.HP.Reset(1);

  return go;
}

void GameObjectsFactory::CreateStairs(MapLevelBase* levelWhereCreate, int x, int y, int image, MapType leadsTo)
{
  auto tile = levelWhereCreate->MapArray[x][y].get();

  auto c = tile->AddComponent<StairsComponent>();
  StairsComponent* stairs = static_cast<StairsComponent*>(c);
  stairs->LeadsTo = leadsTo;

  tile->ObjectName = (image == '>') ? "Stairs Down" : "Stairs Up";
  tile->FgColor = Colors::WhiteColor;
  tile->BgColor = Colors::DoorHighlightColor;
  tile->Image = image;
}

GameObject* GameObjectsFactory::CreateDummyObject(int x,
                                                  int y,
                                                  const std::string& objName,
                                                  char image,
                                                  const std::string& fgColor,
                                                  const std::string& bgColor)
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

GameObject* GameObjectsFactory::CreateContainer(const std::string& name, const std::string& bgColor, int image, int x, int y)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->ObjectName = name;
  go->PosX = x;
  go->PosY = y;
  go->Image = image;
  go->FgColor = "#FFFFFF";
  go->BgColor = bgColor;
  go->Blocking = true;
  go->BlocksSight = true;

  ContainerComponent* cc = go->AddComponent<ContainerComponent>();

  go->InteractionCallback = std::bind(&ContainerComponent::Interact, cc);

  return go;
}

GameObject* GameObjectsFactory::CreateDoor(int x, int y,
                                           bool isOpen,
                                           DoorMaterials material,
                                           const std::string& doorName,
                                           int hitPoints,
                                           const std::string& fgOverrideColor,
                                           const std::string& bgOverrideColor)
{
  const std::map<DoorMaterials, int> doorDefByMat =
  {
    { DoorMaterials::WOOD,  5  },
    { DoorMaterials::STONE, 10 },
    { DoorMaterials::IRON,  15 }
  };

  const std::map<DoorMaterials, std::string> doorPrefixByMat =
  {
    { DoorMaterials::WOOD,  "Wooden" },
    { DoorMaterials::STONE, "Stone"  },
    { DoorMaterials::IRON,  "Iron"   }
  };

  std::string doorNameTotal;
  if (doorName.empty())
  {
    doorNameTotal = doorPrefixByMat.at(material) + " Door";
  }
  else
  {
    doorNameTotal = doorName;
  }

  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->PosX = x;
  go->PosY = y;

  if (hitPoints > 0)
  {
    go->Attrs.Indestructible = false;
    go->Attrs.Def.Set(doorDefByMat.at(material));
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

GameObject* GameObjectsFactory::CreateStaticObject(int x, int y, const GameObjectInfo& objectInfo, int hitPoints, GameObjectType type)
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

ItemComponent* GameObjectsFactory::CloneItem(ItemComponent* copyFrom)
{
  GameObject* copy = CloneObject(copyFrom->OwnerGameObject);

  ItemComponent* ic = copy->AddComponent<ItemComponent>();
  ic->Data = copyFrom->Data;

  return ic;
}

GameObject* GameObjectsFactory::CloneObject(GameObject* copyFrom)
{
  GameObject* copy = new GameObject(Map::Instance().CurrentLevel);

  // NOTE: don't forget to copy any newly added fields

  copy->VisibilityRadius = copyFrom->VisibilityRadius;
  copy->PosX             = copyFrom->PosX;
  copy->PosY             = copyFrom->PosY;
  copy->Special          = copyFrom->Special;
  copy->Blocking         = copyFrom->Blocking;
  copy->BlocksSight      = copyFrom->BlocksSight;
  copy->Revealed         = copyFrom->Revealed;
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

GameObject* GameObjectsFactory::CreateBreakableObjectWithRandomLoot(int x,
                                                                    int y,
                                                                    char image,
                                                                    const std::string& objName,
                                                                    const std::string& fgColor,
                                                                    const std::string& bgColor)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->PosX = x;
  go->PosY = y;

  go->FgColor = fgColor;
  go->BgColor = bgColor;
  go->Image = image;
  go->ObjectName = objName;
  go->Blocking = true;

  go->Attrs.Indestructible = false;
  go->Attrs.HP.Reset(1);

  int dungeonLevel = Map::Instance().CurrentLevel->DungeonLevel;
  int maxLevel = (int)MapType::THE_END;

  ContainerComponent* cc = go->AddComponent<ContainerComponent>(maxLevel + 1);
  cc->CanBeOpened = false;

  int maxItems = maxLevel;

  int nothingChance = maxLevel - dungeonLevel;
  int somethingChance = dungeonLevel;

  float failScale = 1.25f;

  std::map<ItemType, int> weights =
  {
    { ItemType::NOTHING, nothingChance },
    { ItemType::DUMMY, somethingChance }
  };

  for (int i = 0; i < maxItems; i++)
  {
    auto res = Util::WeightedRandom(weights);
    if (res.first != ItemType::NOTHING)
    {
      auto item = ItemsFactory::Instance().CreateRandomItem(0, 0);

      // If there is already such stackable item in inventory,
      // delete the game object we just created.
      if (item != nullptr && cc->Add(item))
      {
        delete item;
      }
    }
    else
    {
      maxItems = (int)((float)maxItems / failScale);
    }
  }

  return go;
}
