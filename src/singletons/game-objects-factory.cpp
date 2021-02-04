#include "game-objects-factory.h"

#include "constants.h"
#include "rng.h"
#include "item-component.h"
#include "shrine-component.h"
#include "ai-component.h"
#include "ai-monster-basic.h"
#include "ai-monster-bat.h"
#include "ai-monster-spider.h"
#include "ai-monster-smart.h"
#include "ai-monster-troll.h"
#include "ai-monster-herobrine.h"
#include "ai-npc.h"
#include "stairs-component.h"
#include "door-component.h"
#include "go-timed-destroyer.h"
#include "map.h"
#include "application.h"
#include "game-object-info.h"
#include "spells-processor.h"
#include "spells-database.h"
#include "printer.h"

void GameObjectsFactory::Init()
{
  if (_initialized)
  {
    return;
  }

  _playerRef = &Application::Instance().PlayerInstance;

  InitPotionColors();
  InitScrolls();

  _initialized = true;
}

GameObject* GameObjectsFactory::CreateGameObject(int x, int y, ItemType objType)
{  
  // NOTE: no random items allowed

  GameObject* go = nullptr;

  switch (objType)
  {    
    case ItemType::COINS:
      go = CreateMoney();
      break;

    case ItemType::HEALING_POTION:
      go = CreateHealingPotion();
      break;

    case ItemType::MANA_POTION:
      go = CreateManaPotion();
      break;

    case ItemType::HUNGER_POTION:
      go = CreateHungerPotion();
      break;

    case ItemType::EXP_POTION:
      go = CreateExpPotion();
      break;

    case ItemType::STR_POTION:
    case ItemType::DEF_POTION:
    case ItemType::MAG_POTION:
    case ItemType::RES_POTION:
    case ItemType::SKL_POTION:
    case ItemType::SPD_POTION:
      go = CreateStatPotion(GlobalConstants::StatNameByPotionType.at(objType));
      break;

    case ItemType::GEM:
    {
      // The deeper you go, the more is the chance to get actual gem
      // and not worthless glass.
      int gemChance = Map::Instance().CurrentLevel->DungeonLevel * 3;
      go = CreateGem(x, y, GemType::RANDOM, gemChance);
    }
    break;

    case ItemType::RETURNER:
      go = CreateReturner(x, y);
      break;

    case ItemType::REPAIR_KIT:
      go = CreateRepairKit(x, y);
      break;

    default:
      break;
  }

  // No check for nullptr, program will crash and
  // thus we will know that we forgot to add object
  // into the factory.
  go->PosX = x;
  go->PosY = y;

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
      go = CreateRat(x, y);
      break;

    case GameObjectType::BAT:
      go = CreateBat(x, y);
      break;

    case GameObjectType::SPIDER:
      go = CreateSpider(x, y);
      break;

    case GameObjectType::HEROBRINE:
      go = CreateHerobrine(x, y);
      break;

    case GameObjectType::TROLL:
      go = CreateTroll(x, y);
      break;
  }

  // No check for nullptr, program will crash and
  // thus we will know that we forgot to add object
  // into the factory.
  go->Type = monsterType;
  go->PosX = x;
  go->PosY = y;

  return go;
}

GameObject* GameObjectsFactory::CreateMoney(int amount)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->ObjectName = GlobalConstants::MoneyName;
  go->Image = '$';
  go->FgColor = GlobalConstants::CoinsColor;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.IdentifiedDescription = { "You can buy things with these." };

  int scale = Map::Instance().CurrentLevel->DungeonLevel;

  int money = (amount == 0) ? RNG::Instance().RandomRange(1, 11) * scale : amount;
  ic->Data.Cost = money;
  ic->Data.Amount = money;
  ic->Data.IsStackable = true;
  ic->Data.ItemType_ = ItemType::COINS;
  ic->Data.IsIdentified = true;
  ic->Data.IdentifiedName = GlobalConstants::MoneyName;

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateShrine(int x, int y, ShrineType type, int timeout)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->PosX = x;
  go->PosY = y;
  go->Image = '/';

  go->FgColor = GlobalConstants::ShrineColorsByType.at(type).first;
  go->BgColor = GlobalConstants::ShrineColorsByType.at(type).second;

  ShrineComponent* sc = go->AddComponent<ShrineComponent>();

  sc->Type = type;
  sc->Counter = timeout;
  sc->Timeout = timeout;

  go->ObjectName = GlobalConstants::ShrineNameByType.at(type);
  go->FogOfWarName = "?Shrine?";
  go->InteractionCallback = std::bind(&ShrineComponent::Interact, sc);

  return go;
}

GameObject* GameObjectsFactory::CreateNPC(int x, int y, NPCType npcType, bool standing)
{
  char img = '@';

  #ifdef USE_SDL
  img = GlobalConstants::CP437IndexByType[NameCP437::FACE_2];
  #endif

  GameObject* go = new GameObject(Map::Instance().CurrentLevel, x, y, img, "#FFFFFF");

  go->Move(0, 0);

  AIComponent* aic = go->AddComponent<AIComponent>();
  AINPC* ainpc = aic->AddModel<AINPC>();
  ainpc->Init(npcType, standing);

  std::string goColor = (ainpc->Data.IsMale) ? "#FFFFFF" : "#FF00FF";
  go->FgColor = goColor;

  aic->ChangeModel<AINPC>();

  return go;
}

GameObject* GameObjectsFactory::CreateRat(int x, int y, bool randomize)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel, x, y, 'r', GlobalConstants::MonsterColor);  
  go->ObjectName = "Feral Rat";
  go->Attrs.Indestructible = false;
  go->HealthRegenTurns = 30;

  // Sets Occupied flag for _currentCell
  go->Move(0, 0);

  go->IsLiving = true;

  AIComponent* ai = go->AddComponent<AIComponent>();
  //AIMonsterSmart* aimb = ai->AddModel<AIMonsterSmart>();
  AIMonsterBasic* aimb = ai->AddModel<AIMonsterBasic>();
  aimb->AgroRadius = 8;
  aimb->ConstructAI();

  ai->ChangeModel<AIMonsterBasic>();
  //ai->ChangeModel<AIMonsterSmart>();

  // Set attributes
  if (randomize)
  {
    int pl = _playerRef->Attrs.Lvl.Get();
    int dl = Map::Instance().CurrentLevel->DungeonLevel;
    int difficulty = std::max(pl, dl); //pl + dl;
    int diffOffset = RNG::Instance().RandomRange(0, 4);

    difficulty += diffOffset;

    go->Attrs.Str.Talents = 1;
    go->Attrs.Spd.Talents = 1;

    for (int i = 0; i < difficulty; i++)
    {
      go->LevelUp(2);
    }

    go->Attrs.HP.Restore();
    go->Attrs.MP.Restore();

    /*
    int randomStr = RNG::Instance().RandomRange(0 * difficulty, 1 * difficulty);
    int randomDef = RNG::Instance().RandomRange(0, 1 * difficulty);
    int randomSkl = RNG::Instance().RandomRange(0, 1 * difficulty);
    int randomHp = RNG::Instance().RandomRange(1 * difficulty, 5 * difficulty);
    int randomSpd = RNG::Instance().RandomRange(0, 2 * difficulty);

    go->Attrs.Lvl.Set(difficulty);

    go->Attrs.Str.Set(randomStr);
    go->Attrs.Def.Set(randomDef);
    go->Attrs.HP.Set(randomHp);
    go->Attrs.Spd.Set(randomSpd);
    go->Attrs.Skl.Set(randomSkl);
    */
  }

  return go;
}

GameObject* GameObjectsFactory::CreateBat(int x, int y, bool randomize)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel, x, y, 'b', GlobalConstants::MonsterColor);
  go->ObjectName = "Flying Bat";
  go->Attrs.Indestructible = false;
  go->HealthRegenTurns = 20;

  go->IsLiving = true;

  // Sets Occupied flag for _currentCell
  go->Move(0, 0);

  AIComponent* ai = go->AddComponent<AIComponent>();
  AIMonsterBat* aimb = ai->AddModel<AIMonsterBat>();
  aimb->AgroRadius = 16;
  aimb->ConstructAI();

  ai->ChangeModel<AIMonsterBat>();

  // Set attributes
  if (randomize)
  {
    int pl = _playerRef->Attrs.Lvl.Get();
    int dl = Map::Instance().CurrentLevel->DungeonLevel;
    int difficulty = std::max(pl, dl); //pl + dl;
    int diffOffset = RNG::Instance().RandomRange(0, 4);

    difficulty += diffOffset;

    go->Attrs.Def.Talents = 3;
    go->Attrs.Spd.Talents = 3;

    for (int i = 0; i < difficulty; i++)
    {
      go->LevelUp();
    }

    go->Attrs.HP.Restore();
    go->Attrs.MP.Restore();

    /*
    int randomStr = RNG::Instance().RandomRange(0 * difficulty, 1 * difficulty);
    int randomDef = RNG::Instance().RandomRange(0, 1 * difficulty);
    int randomSkl = RNG::Instance().RandomRange(1 * difficulty, 2 * difficulty);
    int randomHp = RNG::Instance().RandomRange(1 * difficulty, 3 * difficulty);
    int randomSpd = RNG::Instance().RandomRange(1 * difficulty, 2 * difficulty);

    go->Attrs.Lvl.Set(difficulty);

    go->Attrs.Str.Set(randomStr);
    go->Attrs.Def.Set(randomDef);
    go->Attrs.HP.Set(randomHp);
    go->Attrs.Spd.Set(randomSpd);
    go->Attrs.Skl.Set(randomSkl);
    */
  }

  return go;
}

GameObject* GameObjectsFactory::CreateSpider(int x, int y, bool randomize)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel, x, y, 's', GlobalConstants::MonsterColor);
  go->ObjectName = "Cave Spider";
  go->Attrs.Indestructible = false;
  go->HealthRegenTurns = 30;

  go->IsLiving = true;

  // Sets Occupied flag for _currentCell
  go->Move(0, 0);

  AIComponent* ai = go->AddComponent<AIComponent>();
  AIMonsterSpider* aims = ai->AddModel<AIMonsterSpider>();
  aims->AgroRadius = 12;
  aims->ConstructAI();

  ai->ChangeModel<AIMonsterSpider>();

  // Set attributes
  if (randomize)
  {
    int pl = _playerRef->Attrs.Lvl.Get();
    int dl = Map::Instance().CurrentLevel->DungeonLevel;
    int difficulty = std::max(pl, dl); //pl + dl;
    int diffOffset = RNG::Instance().RandomRange(0, 4);

    difficulty += diffOffset;

    go->Attrs.Str.Talents = 2;
    go->Attrs.Def.Talents = 2;
    go->Attrs.Spd.Talents = 1;

    for (int i = 0; i < difficulty; i++)
    {
      go->LevelUp(4);
    }

    go->Attrs.HP.Restore();
    go->Attrs.MP.Restore();

    /*
    int randomStr = RNG::Instance().RandomRange(1 * difficulty, 2 * difficulty);
    int randomDef = RNG::Instance().RandomRange(1 * difficulty, 2 * difficulty);
    int randomSkl = RNG::Instance().RandomRange(1 * difficulty, 2 * difficulty);
    int randomHp = RNG::Instance().RandomRange(2 * difficulty, 6 * difficulty);
    int randomSpd = RNG::Instance().RandomRange(1 * difficulty, 2 * difficulty);

    go->Attrs.Lvl.Set(difficulty);

    go->Attrs.Str.Set(randomStr);
    go->Attrs.Def.Set(randomDef);
    go->Attrs.HP.Set(randomHp);
    go->Attrs.Spd.Set(randomSpd);
    go->Attrs.Skl.Set(randomSkl);
    */
  }

  return go;
}

GameObject* GameObjectsFactory::CreateTroll(int x, int y, bool randomize)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel, x, y, 'T', GlobalConstants::MonsterColor);
  go->ObjectName = "Troll";
  go->Attrs.Indestructible = false;
  go->HealthRegenTurns = 3;

  go->IsLiving = true;

  // Sets Occupied flag for _currentCell
  go->Move(0, 0);

  AIComponent* ai = go->AddComponent<AIComponent>();
  AIMonsterTroll* aims = ai->AddModel<AIMonsterTroll>();
  aims->AgroRadius = 8;
  aims->ConstructAI();

  ai->ChangeModel<AIMonsterTroll>();

  // Set attributes
  if (randomize)
  {
    int pl = _playerRef->Attrs.Lvl.Get();
    int dl = Map::Instance().CurrentLevel->DungeonLevel;
    int difficulty = std::max(pl, dl); //pl + dl;
    int diffOffset = RNG::Instance().RandomRange(0, 4);

    difficulty += diffOffset;

    go->Attrs.Str.Set(4);

    go->Attrs.Str.Talents = 4;
    go->Attrs.Def.Talents = 3;
    go->Attrs.HP.Talents = 10;

    for (int i = 0; i < difficulty; i++)
    {
      go->LevelUp();
    }

    go->Attrs.HP.Restore();
    go->Attrs.MP.Restore();

    int str = go->Attrs.Str.Get() * 2;
    go->Attrs.Str.Set(str);

    int spd = go->Attrs.Spd.Get() / 2;
    go->Attrs.Spd.Set(spd);

    /*
    int randomStr = RNG::Instance().RandomRange(1 * difficulty, 2 * difficulty);
    int randomDef = RNG::Instance().RandomRange(1 * difficulty, 2 * difficulty);
    int randomSkl = RNG::Instance().RandomRange(1 * difficulty, 2 * difficulty);
    int randomHp = RNG::Instance().RandomRange(2 * difficulty, 6 * difficulty);
    int randomSpd = RNG::Instance().RandomRange(1 * difficulty, 2 * difficulty);

    go->Attrs.Lvl.Set(difficulty);

    go->Attrs.Str.Set(randomStr);
    go->Attrs.Def.Set(randomDef);
    go->Attrs.HP.Set(randomHp);
    go->Attrs.Spd.Set(randomSpd);
    go->Attrs.Skl.Set(randomSkl);
    */
  }

  return go;
}

GameObject* GameObjectsFactory::CreateHerobrine(int x, int y)
{
  char img = '@';

  #ifdef USE_SDL
  img = GlobalConstants::CP437IndexByType[NameCP437::FACE_2];
  #endif

  GameObject* go = new GameObject(Map::Instance().CurrentLevel, x, y, img, GlobalConstants::MonsterColor);
  go->ObjectName = "Herobrine";
  go->Attrs.Indestructible = false;
  go->HealthRegenTurns = 30;

  // Sets Occupied flag for _currentCell
  go->MoveTo(x, y);

  AIComponent* ai = go->AddComponent<AIComponent>();
  AIMonsterHerobrine* aims = ai->AddModel<AIMonsterHerobrine>();
  aims->AgroRadius = 12;
  aims->ConstructAI();

  ai->ChangeModel<AIMonsterHerobrine>();

  ContainerComponent* cc = go->AddComponent<ContainerComponent>();
  cc->MaxCapacity = 20;

  GameObject* pickaxe = CreateUniquePickaxe();
  cc->AddToInventory(pickaxe);

  GameObject* gem = CreateGem(0, 0, GemType::ORANGE_AMBER);
  cc->AddToInventory(gem);

  go->Attrs.Str.Talents = 3;
  go->Attrs.Skl.Talents = 3;
  go->Attrs.Def.Talents = 1;
  go->Attrs.Spd.Talents = 1;

  // FIXME: debug
  //go->LevelUp();

  for (int i = 1; i < 10; i++)
  {
    go->LevelUp(5);
  }

  go->Attrs.HP.Restore();
  go->Attrs.MP.Restore();

  return go;
}

GameObject* GameObjectsFactory::CreateUniquePickaxe()
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->ObjectName = "Pickaxe";
  go->Image = '(';
  go->FgColor = GlobalConstants::ItemUniqueColor;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.EqCategory = EquipmentCategory::WEAPON;
  ic->Data.ItemType_ = ItemType::WEAPON;
  ic->Data.Rarity = ItemRarity::UNIQUE;

  ic->Data.Prefix = ItemPrefix::CURSED;
  ic->Data.IsIdentified = false;

  int diceRolls = 1;
  int diceSides = 8;

  ic->Data.WeaponType_ = WeaponType::PICKAXE;

  int avgDamage = CalculateAverageDamage(diceRolls, diceSides);

  ic->Data.Damage.SetMin(diceRolls);
  ic->Data.Damage.SetMax(diceSides);

  AddRandomBonus(ic, ItemBonusType::SELF_REPAIR);

  AddBonus(ic, { ItemBonusType::SKL, 1 });
  AddBonus(ic, { ItemBonusType::SPD, 1 });

  ic->Data.Durability.Reset(30);

  ic->Data.UnidentifiedName = "?" + go->ObjectName + "?";
  ic->Data.IdentifiedName = "Block Breaker";

  auto str = Util::StringFormat("You think it'll do %d damage on average.", avgDamage);
  ic->Data.UnidentifiedDescription = { str, "You can't tell anything else." };

  ic->Data.IdentifiedDescription =
  {
    "This is quite an old but sturdy looking pickaxe,",
    "yet you can't shake the uneasy feeling about it.",
    "There are traces of blood on its head."
  };

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateRemains(GameObject* from)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel, from->PosX, from->PosY, '%', from->FgColor, from->BgColor);

  go->Type = GameObjectType::REMAINS;

  // Living creatures leave decomposable corpses
  if (from->IsLiving)
  {
    TimedDestroyerComponent* td = go->AddComponent<TimedDestroyerComponent>();
    td->Time = 200; //from->Attrs.HP.OriginalValue * 2;
  }

  auto str = Util::StringFormat("%s's remains", from->ObjectName.data());
  go->ObjectName = str;

  go->Attrs.Indestructible = false;
  go->Attrs.HP.Reset(1);

  return go;
}

bool GameObjectsFactory::HandleItemEquip(ItemComponent* item)
{
  bool res = false;

  if (item->Data.EqCategory == EquipmentCategory::NOT_EQUIPPABLE)
  {
    Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Information", { "Can't be equipped!" }, GlobalConstants::MessageBoxRedBorderColor);
    return res;
  }

  auto category = item->Data.EqCategory;

  if (category == EquipmentCategory::RING)
  {
    res = ProcessRingEquiption(item);
  }
  else
  {
    res = ProcessItemEquiption(item);
  }

  return res;
}

bool GameObjectsFactory::HandleItemUse(ItemComponent* item)
{
  bool res = false;  

  if (item->Data.UseCallback.target_type() != typeid(void))
  {
    return item->Data.UseCallback(item);    
  }
  else
  {
    switch (item->Data.ItemType_)
    {
      case ItemType::COINS:
        Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Information", { "You don't 'use' money like that." }, GlobalConstants::MessageBoxRedBorderColor);
        break;

      default:
        Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Information", { "Can't be used!" }, GlobalConstants::MessageBoxRedBorderColor);
        break;
    }
  }

  return res;
}
GameObject* GameObjectsFactory::CreateFood(int x, int y, FoodType type, ItemPrefix prefixOverride, bool isIdentified)
{
  std::string name;
  int addsHunger = 0;

  name = GlobalConstants::FoodHungerPercentageByName.at(type).first;
  int percentage = GlobalConstants::FoodHungerPercentageByName.at(type).second;
  int hungerMax = _playerRef->Attrs.HungerRate.Get();
  int hungerToAdd = ((float)hungerMax * ((float)percentage / 100.0f));

  addsHunger = hungerToAdd;

  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->FgColor = "#FFFFFF";
  go->BgColor = "#000000";
  go->Image = '%';
  go->ObjectName = name;
  go->PosX = x;
  go->PosY = y;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_ = ItemType::FOOD;
  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;
  ic->Data.Amount = 1;
  ic->Data.IsIdentified = isIdentified;

  // Use Cost field to store amount of hunger replenished
  ic->Data.Cost = addsHunger;

  ic->Data.IsStackable = true;

  std::string unidName = (type == FoodType::RATIONS || type == FoodType::IRON_RATIONS) ? "Rations" : name;
  ic->Data.UnidentifiedName = "?" + unidName + "?";

  ic->Data.UnidentifiedDescription = { "Looks edible but eat at your own risk." };
  ic->Data.IdentifiedDescription = { "Looks edible." };
  ic->Data.IdentifiedName = name;

  ic->Data.UseCallback = std::bind(&GameObjectsFactory::FoodUseHandler, this, ic);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateHealingPotion(ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  ItemType t = ItemType::HEALING_POTION;

  std::string fgColor = _gamePotionsMap[t].FgBgColor.first;
  std::string bgColor = _gamePotionsMap[t].FgBgColor.second;
  std::string name = _gamePotionsMap[t].PotionName;

  go->FgColor = fgColor;
  go->BgColor = bgColor;
  go->Image = '!';
  go->ObjectName = name;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_ = t;
  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;
  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;
  ic->Data.Cost = 25;

  ic->Data.IdentifiedDescription = { "Restores some of your health." };
  ic->Data.IdentifiedName = name;
  ic->Data.UnidentifiedName = "?" + name + "?";

  ic->Data.UseCallback = std::bind(&GameObjectsFactory::HealingPotionUseHandler, this, ic);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateManaPotion(ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  ItemType t = ItemType::MANA_POTION;

  std::string fgColor = _gamePotionsMap[t].FgBgColor.first;
  std::string bgColor = _gamePotionsMap[t].FgBgColor.second;
  std::string name = _gamePotionsMap[t].PotionName;

  go->FgColor = fgColor;
  go->BgColor = bgColor;
  go->Image = '!';
  go->ObjectName = name;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_ = t;
  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;
  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;
  ic->Data.Cost = 25;

  ic->Data.IdentifiedDescription = { "Helps you regain spiritual powers." };
  ic->Data.IdentifiedName = name;
  ic->Data.UnidentifiedName = "?" + name + "?";

  ic->Data.UseCallback = std::bind(&GameObjectsFactory::ManaPotionUseHandler, this, ic);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateHungerPotion(ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  ItemType t = ItemType::HUNGER_POTION;

  std::string fgColor = _gamePotionsMap[t].FgBgColor.first;
  std::string bgColor = _gamePotionsMap[t].FgBgColor.second;
  std::string name = _gamePotionsMap[t].PotionName;

  go->FgColor = fgColor;
  go->BgColor = bgColor;
  go->Image = '!';
  go->ObjectName = name;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_ = t;
  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;
  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;
  ic->Data.Cost = 15;

  ic->Data.IdentifiedDescription = { "Liquid food. Drink it if there's nothing else to eat." };
  ic->Data.IdentifiedName = name;
  ic->Data.UnidentifiedName = "?" + name + "?";

  ic->Data.UseCallback = std::bind(&GameObjectsFactory::HungerPotionUseHandler, this, ic);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateExpPotion(ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  ItemType t = ItemType::EXP_POTION;

  std::string fgColor = _gamePotionsMap[t].FgBgColor.first;
  std::string bgColor = _gamePotionsMap[t].FgBgColor.second;
  std::string name = _gamePotionsMap[t].PotionName;

  go->FgColor = fgColor;
  go->BgColor = bgColor;
  go->Image = '!';
  go->ObjectName = name;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_ = t;
  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;
  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;
  ic->Data.Cost = 250;

  ic->Data.IdentifiedDescription = { "They say drinking this will bring you to the next level.", "Whatever that means..." };
  ic->Data.IdentifiedName = name;
  ic->Data.UnidentifiedName = "?" + name + "?";

  ic->Data.UseCallback = std::bind(&GameObjectsFactory::ExpPotionUseHandler, this, ic);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateStatPotion(const std::string& statName, ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  ItemType t = GlobalConstants::PotionTypeByStatName.at(statName);

  std::string fgColor = _gamePotionsMap[t].FgBgColor.first;
  std::string bgColor = _gamePotionsMap[t].FgBgColor.second;
  std::string name = _gamePotionsMap[t].PotionName;

  go->FgColor = fgColor;
  go->BgColor = bgColor;
  go->Image = '!';
  go->ObjectName = name;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_ = t;
  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;
  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;
  ic->Data.Cost = 500;

  auto str = Util::StringFormat("This will affect your %s", statName.data());
  ic->Data.IdentifiedDescription = { str };

  ic->Data.IdentifiedName = name;
  ic->Data.UnidentifiedName = "?" + name + "?";

  ic->Data.UseCallback = std::bind(&GameObjectsFactory::StatPotionUseHandler, this, ic);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateRandomPotion()
{
  GameObject* go = nullptr;

  auto weights = Util::WeightedRandom(GlobalConstants::PotionsWeightTable);
  go = CreateGameObject(0, 0, weights.first);

  go->Attrs.Indestructible = false;
  go->Attrs.HP.Reset(1);

  ItemComponent* ic = go->GetComponent<ItemComponent>();

  ic->Data.IsIdentified = false;
  ic->Data.UnidentifiedDescription = { "You don't know what will happen if you drink it." };

  return go;
}

GameObject* GameObjectsFactory::CreateRandomWeapon(ItemPrefix prefixOverride)
{
  GameObject* go = nullptr;

  int isMelee = RNG::Instance().RandomRange(0, 2);
  if (isMelee == 0)
  {
    int index = RNG::Instance().RandomRange(0, GlobalConstants::WeaponNameByType.size());
    auto it = GlobalConstants::WeaponNameByType.begin();
    std::advance(it, index);
    go = CreateWeapon(0, 0, it->first, prefixOverride);
  }
  else
  {
    int index = RNG::Instance().RandomRange(0, GlobalConstants::RangedWeaponNameByType.size());
    auto it = GlobalConstants::RangedWeaponNameByType.begin();
    std::advance(it, index);
    go = CreateRangedWeapon(0, 0, it->first, prefixOverride);
  }

  ItemComponent* ic = go->GetComponent<ItemComponent>();
  TryToAddBonuses(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateRandomArmor(ItemPrefix prefixOverride)
{
  GameObject* go = nullptr;

  int index = RNG::Instance().RandomRange(0, GlobalConstants::ArmorNameByType.size());
  auto it = GlobalConstants::ArmorNameByType.begin();
  std::advance(it, index);
  go = CreateArmor(0, 0, it->first, prefixOverride);

  ItemComponent* ic = go->GetComponent<ItemComponent>();
  TryToAddBonuses(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateRandomItem(int x, int y, ItemType exclude)
{
  GameObject* go = nullptr;

  std::vector<ItemType> possibleItems =
  {
    ItemType::COINS,
    ItemType::WEAPON,
    ItemType::ARMOR,
    ItemType::POTION,
    ItemType::FOOD,
    ItemType::GEM,
    ItemType::RETURNER,
    ItemType::REPAIR_KIT,
    ItemType::WAND,
    ItemType::SCROLL,
    ItemType::ARROWS,
    ItemType::ACCESSORY,
    ItemType::SPELLBOOK
  };

  auto findRes = std::find(possibleItems.begin(), possibleItems.end(), exclude);

  if (findRes != possibleItems.end())
  {
    possibleItems.erase(findRes);
  }

  std::map<FoodType, int> foodMap =
  {
    { FoodType::APPLE,        1 },
    { FoodType::BREAD,        1 },
    { FoodType::CHEESE,       1 },
    { FoodType::PIE,          1 },
    { FoodType::MEAT,         1 },
    { FoodType::FISH,         1 },
    { FoodType::TIN,          1 },
    { FoodType::RATIONS,      1 },
    { FoodType::IRON_RATIONS, 1 }
  };

  std::map<GemType, int> gemsMap =
  {
    { GemType::WORTHLESS_GLASS, 800 },
    { GemType::BLACK_OBSIDIAN,  150 },
    { GemType::GREEN_JADE,      145 },
    { GemType::PURPLE_FLUORITE, 135 },
    { GemType::PURPLE_AMETHYST, 130 },
    { GemType::RED_GARNET,      110 },
    { GemType::WHITE_OPAL,      100 },
    { GemType::BLACK_JETSTONE,  95  },
    { GemType::ORANGE_AMBER,    75  },
    { GemType::BLUE_AQUAMARINE, 55  },
    { GemType::YELLOW_CITRINE,  55  },
    { GemType::GREEN_EMERALD,   30  },
    { GemType::BLUE_SAPPHIRE,   25  },
    { GemType::ORANGE_JACINTH,  20  },
    { GemType::RED_RUBY,        15  },
    { GemType::WHITE_DIAMOND,   15  },
  };

  std::map<ItemType, int> returnerMap =
  {
    { ItemType::RETURNER,  5 },
    { ItemType::NOTHING,  20 }
  };

  // TODO: power of randomly created item
  // should scale with dungeon level.
  int index = RNG::Instance().RandomRange(0, possibleItems.size());

  ItemType res = possibleItems[index];

  // TODO: add cases for all item types after they are decided
  switch (res)
  {
    case ItemType::COINS:
      go = CreateMoney();
      break;

    case ItemType::WEAPON:
      go = CreateRandomWeapon();
      break;

    case ItemType::ARMOR:
      go = CreateRandomArmor();
      break;

    case ItemType::POTION:
      go = CreateRandomPotion();
      break;

    case ItemType::FOOD:
    {
      auto pair = Util::WeightedRandom(foodMap);
      go = CreateFood(0, 0, pair.first);
    }
    break;

    case ItemType::GEM:
    {
      auto pair = Util::WeightedRandom(gemsMap);
      go = CreateGem(0, 0, pair.first);
    }
    break;

    case ItemType::RETURNER:
    {
      auto pair = Util::WeightedRandom(returnerMap);
      if (pair.first == ItemType::RETURNER)
      {
        go = CreateReturner(0, 0);
      }
      else
      {
        go = CreateGem(0, 0, GemType::WORTHLESS_GLASS);
      }
    }
    break;

    case ItemType::REPAIR_KIT:
    {
      go = CreateRepairKit(0, 0);
      ItemComponent* ic = go->GetComponent<ItemComponent>();
      ic->Data.IsIdentified = false;
    }
    break;

    case ItemType::ARROWS:
    {
      int isArrows = RNG::Instance().RandomRange(0, 2);
      ArrowType type = (isArrows == 0) ? ArrowType::ARROWS : ArrowType::BOLTS;
      go = CreateArrows(0, 0, type);
    }
    break;

    case ItemType::WAND:
      go = CreateRandomWand();
      break;

    case ItemType::SCROLL:
      go = CreateRandomScroll();
      break;

    case ItemType::ACCESSORY:
      go = CreateRandomAccessory(0, 0);
      break;
  }

  if (go != nullptr)
  {
    go->PosX = x;
    go->PosY = y;
  }

  return go;
}

void GameObjectsFactory::CreateStairs(MapLevelBase* levelWhereCreate, int x, int y, int image, MapType leadsTo)
{
  auto tile = levelWhereCreate->MapArray[x][y].get();

  auto c = tile->AddComponent<StairsComponent>();
  StairsComponent* stairs = static_cast<StairsComponent*>(c);
  stairs->LeadsTo = leadsTo;

  tile->ObjectName = (image == '>') ? "Stairs Down" : "Stairs Up";
  tile->FgColor = GlobalConstants::WhiteColor;
  tile->BgColor = GlobalConstants::DoorHighlightColor;
  tile->Image = image;
}

bool GameObjectsFactory::FoodUseHandler(ItemComponent* item)
{
  auto objName = (item->Data.IsIdentified) ? item->Data.IdentifiedName : item->Data.UnidentifiedName;

  auto str = Util::StringFormat("You eat %s...", objName.data());
  Printer::Instance().AddMessage(str);

  if (item->Data.Prefix == ItemPrefix::CURSED)
  {
    Printer::Instance().AddMessage("Disgusting!");

    if (Util::Rolld100(50))
    {      
      _playerRef->Attrs.Hunger += item->Data.Cost;

      Printer::Instance().AddMessage("Rotten food!");

      // NOTE: assuming player hunger meter is in order of 1000
      int dur = item->Data.Cost / 100;

      ItemBonusStruct b;
      b.Type = ItemBonusType::POISONED;
      b.BonusValue = -1;
      b.Period = 10;
      b.Duration = dur;
      b.Cumulative = true;
      b.Id = item->OwnerGameObject->ObjectId();

      _playerRef->AddEffect(b);
    }
    else
    {
      _playerRef->Attrs.Hunger -= item->Data.Cost * 0.5f;
    }
  }
  else if (item->Data.Prefix == ItemPrefix::BLESSED)
  {
    Printer::Instance().AddMessage("It's delicious!");

    _playerRef->Attrs.Hunger -= item->Data.Cost;
    _playerRef->IsStarving = false;
  }
  else
  {
    Printer::Instance().AddMessage("It tasted OK");

    _playerRef->Attrs.Hunger -= item->Data.Cost * 0.75f;
    _playerRef->IsStarving = false;
  }

  _playerRef->Attrs.Hunger = Util::Clamp(_playerRef->Attrs.Hunger, 0, _playerRef->Attrs.HungerRate.Get());

  Application::Instance().ChangeState(GameStates::MAIN_STATE);

  return true;
}

GameObject* GameObjectsFactory::CreateNote(const std::string& objName, const std::vector<std::string>& text)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->FgColor = "#000000";
  go->BgColor = "#FFFFFF";
  go->Image = '?';
  go->ObjectName = objName;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.IsBurnable = true;
  ic->Data.ItemType_ = ItemType::DUMMY;
  ic->Data.IsStackable = false;
  ic->Data.IsIdentified = true;

  ic->Data.IdentifiedDescription = text;
  ic->Data.IdentifiedName = objName;

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateDummyObject(const std::string& objName,
                                                  char image,
                                                  const std::string& fgColor,
                                                  const std::string& bgColor,
                                                  const std::vector<std::string>& descText)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->FgColor = fgColor;
  go->BgColor = bgColor;
  go->Image = image;
  go->ObjectName = objName;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_ = ItemType::DUMMY;
  ic->Data.IsStackable = false;
  ic->Data.IsIdentified = true;
  ic->Data.IdentifiedName = objName;
  ic->Data.IdentifiedDescription = descText;

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateRandomScroll(ItemPrefix prefix)
{
  int index = RNG::Instance().RandomRange(0, GlobalConstants::ScrollValidSpellTypes.size());
  SpellType type = GlobalConstants::ScrollValidSpellTypes.at(index);
  return CreateScroll(0, 0, type, prefix);
}

GameObject* GameObjectsFactory::CreateScroll(int x, int y, SpellType type, ItemPrefix prefixOverride)
{
  SpellInfo* si = SpellsDatabase::Instance().GetInfo(type);

  if (std::find(GlobalConstants::ScrollValidSpellTypes.begin(),
                GlobalConstants::ScrollValidSpellTypes.end(),
                type) == GlobalConstants::ScrollValidSpellTypes.end())
  {
    std::string name = si->SpellName;
    printf("[WARNING] Trying to create a scroll with invalid spell (%s)!\n", name.data());
    return nullptr;
  }

  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->PosX = x;
  go->PosY = y;
  go->FgColor = "#000000";
  go->BgColor = "#FFFFFF";
  go->Image = '?';
  go->ObjectName = "\"" + si->SpellName + "\"";

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.IsBurnable = true;
  ic->Data.Prefix = (prefixOverride != ItemPrefix::RANDOM) ? prefixOverride : RollItemPrefix();
  ic->Data.IsIdentified = (prefixOverride != ItemPrefix::RANDOM) ? true : false;

  ic->Data.ItemType_ = ItemType::SCROLL;
  ic->Data.IsStackable = false;
  ic->Data.SpellHeld = type;

  // NOTE: scrolls cost = base * 2, spellbooks = base * 10
  ic->Data.Cost = si->SpellBaseCost * 2;

  ic->Data.UnidentifiedName = "\"" + _gameScrollsMap[type].ScrollName + "\"";
  ic->Data.UnidentifiedDescription = { "Who knows what will happen if you read these words aloud..." };

  ic->Data.IdentifiedDescription = { "TODO:" };
  ic->Data.IdentifiedName = "Scroll of " + si->SpellName;

  SetItemName(go, ic->Data);

  ic->Data.UseCallback = std::bind(&GameObjectsFactory::ScrollUseHandler, this, ic);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateWeapon(int x, int y, WeaponType type, ItemPrefix prefix, ItemQuality quality, const std::vector<ItemBonusStruct>& bonuses)
{  
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  int dungeonLevel = Map::Instance().CurrentLevel->DungeonLevel * 0.5;
  if (dungeonLevel == 0)
  {
    dungeonLevel = 1;
  }

  go->PosX = x;
  go->PosY = y;

  go->ObjectName = GlobalConstants::WeaponNameByType.at(type);
  go->Image = ')';
  go->FgColor = "#FFFFFF";

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.EqCategory = EquipmentCategory::WEAPON;
  ic->Data.ItemType_ = ItemType::WEAPON;

  ic->Data.Prefix = (prefix != ItemPrefix::RANDOM) ? prefix : RollItemPrefix();
  ic->Data.IsIdentified = (prefix != ItemPrefix::RANDOM) ? true : false;
  ic->Data.ItemQuality_ = (quality != ItemQuality::RANDOM) ? quality : RollItemQuality();

  int avgDamage = 0;
  int baseDurability = 0;

  int diceRolls = 0;
  int diceSides = 0;

  ic->Data.WeaponType_ = type;

  switch (type)
  {
    case WeaponType::DAGGER:
    {      
      diceRolls = 1;
      diceSides = 3;

      baseDurability = 30 + 1 * (int)ic->Data.ItemQuality_;

      ic->Data.Damage.SetMin(diceRolls);
      ic->Data.Damage.SetMax(diceSides);

      AddBonus(ic, { ItemBonusType::SKL, 1 });
      AddBonus(ic, { ItemBonusType::SPD, 1 });
    }
    break;

    case WeaponType::SHORT_SWORD:
    {
      diceRolls = 1;
      diceSides = 6;

      baseDurability = 40 + 2 * (int)ic->Data.ItemQuality_;

      ic->Data.Damage.SetMin(diceRolls);
      ic->Data.Damage.SetMax(diceSides);

      AddBonus(ic, { ItemBonusType::STR, 1 });
    }
    break;

    case WeaponType::ARMING_SWORD:
    {
      diceRolls = 1;
      diceSides = 8;

      baseDurability = 50 + 3 * (int)ic->Data.ItemQuality_;

      ic->Data.Damage.SetMin(diceRolls);
      ic->Data.Damage.SetMax(diceSides);

      AddBonus(ic, { ItemBonusType::STR, 1 });
      AddBonus(ic, { ItemBonusType::DEF, 1 });
    }
    break;

    case WeaponType::LONG_SWORD:
    {
      diceRolls = 2;
      diceSides = 6;

      baseDurability = 65 + 3 * (int)ic->Data.ItemQuality_;

      ic->Data.Damage.SetMin(diceRolls);
      ic->Data.Damage.SetMax(diceSides);

      AddBonus(ic, { ItemBonusType::STR,  2 });
      AddBonus(ic, { ItemBonusType::DEF,  1 });
      AddBonus(ic, { ItemBonusType::SPD, -1 });
    }
    break;

    case WeaponType::GREAT_SWORD:
    {
      diceRolls = 3;
      diceSides = 10;

      baseDurability = 80 + 4 * (int)ic->Data.ItemQuality_;

      ic->Data.Damage.SetMin(diceRolls);
      ic->Data.Damage.SetMax(diceSides);

      AddBonus(ic, { ItemBonusType::STR,  4 });
      AddBonus(ic, { ItemBonusType::SKL, -2 });
      AddBonus(ic, { ItemBonusType::SPD, -4 });
    }
    break;

    case WeaponType::STAFF:
    {
      diceRolls = 1;
      diceSides = 6;

      baseDurability = 30 + 2 * (int)ic->Data.ItemQuality_;

      ic->Data.Damage.SetMin(diceRolls);
      ic->Data.Damage.SetMax(diceSides);

      AddBonus(ic, { ItemBonusType::DEF,  1 });
      AddBonus(ic, { ItemBonusType::SPD, -1 });
    }
    break;

    case WeaponType::PICKAXE:
    {
      diceRolls = 1;
      diceSides = 6;

      baseDurability = 10 + 2 * (int)ic->Data.ItemQuality_;

      ic->Data.Damage.SetMin(diceRolls);
      ic->Data.Damage.SetMax(diceSides);
    }
    break;
  }

  for (auto& b : bonuses)
  {
    AddBonus(ic, b);
  }

  avgDamage = CalculateAverageDamage(diceRolls, diceSides);

  int randomDurAdd = RNG::Instance().RandomRange(0, baseDurability * 0.1f) + dungeonLevel;

  int durability = baseDurability + randomDurAdd;
  ic->Data.Durability.Reset(durability);

  ic->Data.UnidentifiedName = "?" + go->ObjectName + "?";
  ic->Data.IdentifiedName = go->ObjectName;

  auto str = Util::StringFormat("You think it'll do %d damage on average.", avgDamage);
  ic->Data.UnidentifiedDescription = { str, "You can't tell anything else." };

  // *** !!!
  // Identified description for weapon is
  // returned via private helper method in ItemComponent
  // *** !!!

  BUCQualityAdjust(ic->Data);
  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

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

  cc->MaxCapacity = GlobalConstants::InventoryMaxSize;

  go->InteractionCallback = std::bind(&ContainerComponent::Interact, cc);

  return go;
}

GameObject* GameObjectsFactory::CreateGem(int x, int y, GemType type, int gemChance)
{
  GameObject* go = nullptr;

  if (type == GemType::RANDOM)
  {
    int rndStartingIndex = 0;

    if (gemChance != -1)
    {      
      rndStartingIndex = 1;
    }

    int index = RNG::Instance().RandomRange(rndStartingIndex, GlobalConstants::GemNameByType.size());
    GemType t = (GemType)index;
    if (t == GemType::WORTHLESS_GLASS)
    {
      go = CreateRandomGlass();
    }
    else
    {
      go = CreateGemHelper(t);
    }
  }
  else
  {
    if (type == GemType::WORTHLESS_GLASS)
    {
      go = CreateRandomGlass();
    }
    else
    {
      go = CreateGemHelper(type);
    }
  }

  go->PosX = x;
  go->PosY = y;

  return go;
}

GameObject* GameObjectsFactory::CreateWand(int x, int y, WandMaterials material, SpellType spellType, ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  SpellInfo* si = SpellsDatabase::Instance().GetInfo(spellType);

  auto wandColorPair = GlobalConstants::WandColorsByMaterial.at(material);
  std::string wandMaterialName = GlobalConstants::WandMaterialNamesByMaterial.at(material);
  std::string spellName = si->SpellName;
  std::string spellShortName = si->SpellShortName;

  go->PosX = x;
  go->PosY = y;

  go->Image = 'i';

  go->FgColor = wandColorPair.first;
  go->BgColor = wandColorPair.second;

  int dl = Map::Instance().CurrentLevel->DungeonLevel;

  int capacity = GlobalConstants::WandCapacityByMaterial.at(material);

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;
  ic->Data.ItemQuality_ = RollItemQuality();

  int randomness = RNG::Instance().RandomRange(0, capacity);

  randomness += ((capacity / 10) * (int)ic->Data.ItemQuality_);

  randomness /= 2;

  capacity += randomness;

  if (ic->Data.Prefix == ItemPrefix::BLESSED)
  {
    capacity *= 2;
  }

  ic->Data.WandCapacity.Reset(capacity);

  int spellCost = GlobalConstants::WandSpellCapacityCostByType.at(spellType);
  int charges = capacity / spellCost;

  ic->Data.Amount = charges;

  ic->Data.IsChargeable = true;
  ic->Data.EqCategory = EquipmentCategory::WEAPON;
  ic->Data.ItemType_ = ItemType::WAND;
  ic->Data.SpellHeld = spellType;
  ic->Data.Range = 20;
  ic->Data.Durability.Reset(1);
  ic->Data.IsIdentified = (prefixOverride != ItemPrefix::RANDOM) ? true : false;

  // Actual cost is going to be calculated in GetCost()
  ic->Data.Cost = si->SpellBaseCost;

  ic->Data.UnidentifiedName = "?" + wandMaterialName + " Wand?";
  ic->Data.IdentifiedName = wandMaterialName + " Wand of " + spellName;

  ic->Data.UnidentifiedDescription = { "You don't know what it can do" };

  auto str = Util::StringFormat("%s Wand (%s)", wandMaterialName.data(), spellShortName.data());
  go->ObjectName = str;

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateRandomWand(ItemPrefix prefixOverride)
{
  GameObject* go = nullptr;

  std::map<WandMaterials, int> materialsDistribution =
  {
    { WandMaterials::YEW_1,   12 },
    { WandMaterials::IVORY_2, 10 },
    { WandMaterials::EBONY_3,  8 },
    { WandMaterials::ONYX_4,   6 },
    { WandMaterials::GLASS_5,  4 },
    { WandMaterials::COPPER_6, 2 },
    { WandMaterials::GOLDEN_7, 1 },
  };

  auto materialPair = Util::WeightedRandom(materialsDistribution);

  std::map<SpellType, int> spellsDistribution =
  {
    { SpellType::LIGHT,         40 },
    { SpellType::STRIKE,        30 },
    { SpellType::FROST,         30 },
    { SpellType::TELEPORT,      15 },
    { SpellType::FIREBALL,       5 },
    { SpellType::LASER,          5 },
    { SpellType::LIGHTNING,      5 },
    { SpellType::MAGIC_MISSILE, 30 }
  };

  auto spellPair = Util::WeightedRandom(spellsDistribution);

  go = CreateWand(0, 0, materialPair.first, spellPair.first, prefixOverride);

  return go;
}

GameObject* GameObjectsFactory::CreateRangedWeapon(int x,
                                                   int y,
                                                   RangedWeaponType type,
                                                   ItemPrefix prefixOverride,
                                                   ItemQuality quality,
                                                   const std::vector<ItemBonusStruct>& bonuses)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->PosX = x;
  go->PosY = y;

  go->Image = ')';
  go->FgColor = "#FFFFFF";

  go->ObjectName = GlobalConstants::RangedWeaponNameByType.at(type);

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.RangedWeaponType_ = type;
  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;
  ic->Data.ItemQuality_ = (quality != ItemQuality::RANDOM) ? quality : RollItemQuality();

  switch (type)
  {
    case RangedWeaponType::SHORT_BOW:
    {
      int numRolls = 1;
      int diceType = 4;

      ic->Data.Damage.SetMin(numRolls);
      ic->Data.Damage.SetMax(diceType);
      ic->Data.Range = 6;
      ic->Data.Durability.Reset(40 + 4 * (int)ic->Data.ItemQuality_);

      // ======================================================================70
      ic->Data.UnidentifiedDescription =
      {
        "A simple wooden short bow with short range.",
        "Requires some skill to be used effectively."
      };

      AddBonus(ic, { ItemBonusType::SKL, -1 });
    }
    break;

    case RangedWeaponType::LONGBOW:
    {
      int numRolls = 1;
      int diceType = 6;

      ic->Data.Damage.SetMin(numRolls);
      ic->Data.Damage.SetMax(diceType);
      ic->Data.Range = 8;
      ic->Data.Durability.Reset(60 + 6 * (int)ic->Data.ItemQuality_);

      // ======================================================================70
      ic->Data.UnidentifiedDescription =
      {
        "A wooden bow for hunting animals and the like with medium range.",
        "Requires some skill to be used effectively."
      };

      AddBonus(ic, { ItemBonusType::SKL, -2 });
    }
    break;

    case RangedWeaponType::WAR_BOW:
    {
      int numRolls = 1;
      int diceType = 8;

      ic->Data.Damage.SetMin(numRolls);
      ic->Data.Damage.SetMax(diceType);
      ic->Data.Range = 10;
      ic->Data.Durability.Reset(80 + 8 * (int)ic->Data.ItemQuality_);

      // ======================================================================70
      ic->Data.UnidentifiedDescription =
      {
        "A long bow with massive pulling power.",
        "Distinctively designed for battle.",
        "Requires some skill to be used effectively."
      };

      AddBonus(ic, { ItemBonusType::SKL, -3 });
    }
    break;

    case RangedWeaponType::LIGHT_XBOW:
    {
      int numRolls = 2;
      int diceType = 4;

      ic->Data.Damage.SetMin(numRolls);
      ic->Data.Damage.SetMax(diceType);
      ic->Data.Range = 4;
      ic->Data.Durability.Reset(30 + 5 * (int)ic->Data.ItemQuality_);

      // ======================================================================70
      ic->Data.UnidentifiedDescription =
      {
        "A light crossbow has shorter range than its bow counterpart",
        "but has more punch and is easier to aim with.",
        "Requires some time to reload."
      };

      AddBonus(ic, { ItemBonusType::SPD, -1 });
      AddBonus(ic, { ItemBonusType::SKL,  1 });
    }
    break;

    case RangedWeaponType::XBOW:
    {
      int numRolls = 3;
      int diceType = 4;

      ic->Data.Damage.SetMin(numRolls);
      ic->Data.Damage.SetMax(diceType);
      ic->Data.Range = 6;
      ic->Data.Durability.Reset(45 + 8 * (int)ic->Data.ItemQuality_);

      // ======================================================================70
      ic->Data.UnidentifiedDescription =
      {
        "A crossbow has shorter range than its bow counterpart",
        "but has more punch and is easier to aim with.",
        "Requires some time to reload."
      };

      AddBonus(ic, { ItemBonusType::SPD, -2 });
      AddBonus(ic, { ItemBonusType::SKL,  2 });
    }
    break;

    case RangedWeaponType::HEAVY_XBOW:
    {
      int numRolls = 4;
      int diceType = 4;

      ic->Data.Damage.SetMin(numRolls);
      ic->Data.Damage.SetMax(diceType);
      ic->Data.Range = 8;
      ic->Data.Durability.Reset(70 + 12 * (int)ic->Data.ItemQuality_);

      // ======================================================================70
      ic->Data.UnidentifiedDescription =
      {
        "This heavy crossbow can deal some serious damage,",
        "but doesn't have that much range than its bow counterpart.",
        "Requires some time to reload."
      };

      AddBonus(ic, { ItemBonusType::SPD, -3 });
      AddBonus(ic, { ItemBonusType::SKL,  3 });
    }
    break;
  }

  for (auto& b : bonuses)
  {
    AddBonus(ic, b);
  }

  //ic->Data.IdentifiedDescription = ic->Data.UnidentifiedDescription;

  ic->Data.EqCategory = EquipmentCategory::WEAPON;
  ic->Data.ItemType_ = ItemType::RANGED_WEAPON;
  ic->Data.IsIdentified = (prefixOverride != ItemPrefix::RANDOM) ? true : false;

  ic->Data.UnidentifiedName = "?" + go->ObjectName + "?";
  ic->Data.IdentifiedName = go->ObjectName;

  // *** !!!
  // Identified description for weapon is
  // returned via private helper method in ItemComponent
  // *** !!!

  BUCQualityAdjust(ic->Data);
  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateRandomAccessory(int x, int y,
                                                      ItemPrefix prefixOverride,
                                                      bool atLeastOneBonus)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  EquipmentCategory category = EquipmentCategory::RING;
  int chance = RNG::Instance().RandomRange(0, 4);
  if (chance == 0)
  {
    category = EquipmentCategory::NECK;
  }

  if (category == EquipmentCategory::RING)
  {
    go->Image = '=';
    go->ObjectName = "Ring";
  }
  else if (category == EquipmentCategory::NECK)
  {
    go->Image = '"';
    go->ObjectName = "Amulet";
  }

  go->PosX = x;
  go->PosY = y;

  go->FgColor = "#FFFFFF";

  ItemComponent* ic = go->AddComponent<ItemComponent>();
  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;
  ic->Data.EqCategory = category;
  ic->Data.ItemType_ = ItemType::ACCESSORY;
  ic->Data.IsIdentified = (prefixOverride != ItemPrefix::RANDOM) ? true : false;
  ic->Data.UnidentifiedName = "?" + go->ObjectName + "?";
  ic->Data.IdentifiedName = go->ObjectName;

  // TODO: should rings and amulets quality affect bonuses?

  TryToAddBonuses(ic, atLeastOneBonus);

  if (ic->Data.Bonuses.empty())
  {
    ic->Data.IdentifiedName += " of the Bauble";
    ic->Data.Cost = 50 + (int)ic->Data.ItemQuality_ * 10;
  }

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateAccessory(int x, int y,
                                                EquipmentCategory category,
                                                const std::vector<ItemBonusStruct>& bonuses,
                                                ItemPrefix prefix,
                                                ItemQuality quality)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  if (category == EquipmentCategory::RING)
  {
    go->Image = '=';
    go->ObjectName = "Ring";
  }
  else if (category == EquipmentCategory::NECK)
  {
    go->Image = '"';
    go->ObjectName = "Amulet";
  }

  go->PosX = x;
  go->PosY = y;

  go->FgColor = "#FFFFFF";

  ItemComponent* ic = go->AddComponent<ItemComponent>();
  ic->Data.Prefix = (prefix == ItemPrefix::RANDOM) ? RollItemPrefix() : prefix;
  ic->Data.ItemQuality_ = (quality != ItemQuality::RANDOM) ? quality : RollItemQuality();
  ic->Data.EqCategory = category;
  ic->Data.ItemType_ = ItemType::ACCESSORY;
  ic->Data.IsIdentified = (prefix != ItemPrefix::RANDOM) ? true : false;
  ic->Data.UnidentifiedName = "?" + go->ObjectName + "?";
  ic->Data.IdentifiedName = go->ObjectName;

  std::vector<ItemBonusType> bonusesRolled;
  for (auto& b : bonuses)
  {
    bonusesRolled.push_back(b.Type);
    AddBonus(ic, b, true);
  }

  if (ic->Data.Bonuses.empty())
  {
    ic->Data.IdentifiedName += " of the Bauble";
    ic->Data.Cost = 50 + (int)ic->Data.ItemQuality_ * 10;
  }
  else
  {
    SetMagicItemName(ic, bonusesRolled);
  }

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateArrows(int x, int y, ArrowType type, ItemPrefix prefixOverride, int amount)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->PosX = x;
  go->PosY = y;

  go->Image = '^';
  go->FgColor = "#FFFFFF";

  go->ObjectName = GlobalConstants::ArrowNameByType.at(type);

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  int dl = Map::Instance().CurrentLevel->DungeonLevel;
  int randomAmount = RNG::Instance().RandomRange(1 + dl, 10 + dl);

  if (randomAmount <= 0)
  {
    randomAmount = 1;
  }

  ic->Data.ItemType_ = ItemType::ARROWS;
  ic->Data.AmmoType = type;
  ic->Data.EqCategory = EquipmentCategory::SHIELD;
  ic->Data.Amount = (amount != -1) ? amount : randomAmount;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = (prefixOverride != ItemPrefix::RANDOM) ? true : false;
  ic->Data.Cost = 1;

  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;

  ic->Data.UnidentifiedName = "?" + go->ObjectName + "?";
  ic->Data.IdentifiedName = go->ObjectName;

  std::vector<std::string> arrowsDesc =
  {
    "Bundle of arrows for a bow."
  };

  std::vector<std::string> boltsDesc =
  {
    "Bundle of crossbow bolts."
  };

  bool isArrows = (type == ArrowType::ARROWS);

  ic->Data.UnidentifiedDescription = isArrows ? arrowsDesc : boltsDesc;
  ic->Data.IdentifiedDescription = isArrows ? arrowsDesc : boltsDesc;

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateReturner(int x, int y, int charges, ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->PosX = x;
  go->PosY = y;

  go->Image = '*';

  int colorIndex = RNG::Instance().RandomRange(1, GlobalConstants::GemColorNameByType.size());

  GemType t = (GemType)colorIndex;

  std::string colorName = GlobalConstants::GemColorNameByType.at(t);

  std::string fgColor = GlobalConstants::GemColorByType.at(t).first;
  std::string bgColor = GlobalConstants::GemColorByType.at(t).second;

  go->FgColor = fgColor;
  go->BgColor = bgColor;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  int chargesNum = (charges == -1) ? RNG::Instance().RandomRange(1, 11) : charges;

  ic->Data.ItemType_ = ItemType::RETURNER;
  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;  
  ic->Data.IsStackable = false;
  ic->Data.IsIdentified = (prefixOverride == ItemPrefix::RANDOM) ? false : true;
  ic->Data.IsChargeable = true;
  ic->Data.Cost = 50;

  ic->Data.Amount = chargesNum;

  if (ic->Data.Prefix == ItemPrefix::BLESSED)
  {
    ic->Data.Amount = chargesNum * 2;
  }

  ic->Data.UnidentifiedDescription = { "Is this valuable?" };

  ic->Data.IdentifiedName = colorName + " Returner";
  ic->Data.UnidentifiedName = "?" + colorName + " Gem?";

  ic->Data.UseCallback = std::bind(&GameObjectsFactory::ReturnerUseHandler, this, ic);

  go->ObjectName = ic->Data.IdentifiedName;

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateRepairKit(int x, int y, int charges, ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->PosX = x;
  go->PosY = y;

  go->Image = '(';

  go->FgColor = GlobalConstants::WhiteColor;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  int chargesNum = (charges == -1) ? RNG::Instance().RandomRange(1, 51) : charges;

  ic->Data.ItemType_ = ItemType::REPAIR_KIT;
  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;
  ic->Data.IsStackable = false;
  ic->Data.IsIdentified = true;
  ic->Data.IsChargeable = true;
  ic->Data.Cost = 1;

  ic->Data.Amount = chargesNum;

  ic->Data.UnidentifiedDescription =
  {
    "A box filled with various materials and appliances",
    "that are used to repair weapons or armor."
  };

  ic->Data.IdentifiedDescription = ic->Data.UnidentifiedDescription;

  ic->Data.IdentifiedName = "Repair Kit";
  ic->Data.UnidentifiedName = "?Repair Kit?";

  ic->Data.UseCallback = std::bind(&GameObjectsFactory::RepairKitUseHandler, this, ic);

  go->ObjectName = ic->Data.IdentifiedName;

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateArmor(int x, int y, ArmorType type, ItemPrefix prefixOverride, ItemQuality quality)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  int dungeonLevel = Map::Instance().CurrentLevel->DungeonLevel * 0.5;
  if (dungeonLevel == 0)
  {
    dungeonLevel = 1;
  }

  go->PosX = x;
  go->PosY = y;

  go->ObjectName = GlobalConstants::ArmorNameByType.at(type);
  go->Image = '[';

  go->FgColor = GlobalConstants::WhiteColor;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.EqCategory = EquipmentCategory::TORSO;
  ic->Data.ItemType_ = ItemType::ARMOR;
  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;
  ic->Data.IsStackable = false;
  ic->Data.IsIdentified = (prefixOverride == ItemPrefix::RANDOM) ? false : true;
  ic->Data.ItemQuality_ = (quality != ItemQuality::RANDOM) ? quality : RollItemQuality();

  int baseDurability = GlobalConstants::ArmorDurabilityByType.at(type);  
  int cursedPenalty = 0;

  if (ic->Data.Prefix == ItemPrefix::CURSED)
  {
    cursedPenalty = -2;
    baseDurability /= 4;
  }
  else if (ic->Data.Prefix == ItemPrefix::BLESSED)
  {
    cursedPenalty = 2;
  }

  switch (type)
  {
    case ArmorType::PADDING:      
      ic->Data.UnidentifiedDescription =
      {
      // ======================================================================70
        "A thick coat with straw or horsehair filling",
        "to soften incoming blows.",
        "It won't last long, but any armor is better than nothing."
      };

      AddBonus(ic, { ItemBonusType::SPD, cursedPenalty });

      baseDurability += 2 * (int)ic->Data.ItemQuality_;

      break;

    case ArmorType::LEATHER:
      ic->Data.UnidentifiedDescription =
      {
      // ======================================================================70
        "Jacket made of tanned leather provides decent",
        "protection against cutting blows."
      };

      AddBonus(ic, { ItemBonusType::RES, cursedPenalty - 1 });
      AddBonus(ic, { ItemBonusType::SPD, cursedPenalty - 1 });

      baseDurability += 3 * (int)ic->Data.ItemQuality_;

      break;

    case ArmorType::MAIL:
      ic->Data.UnidentifiedDescription =
      {
      // ======================================================================70
        "A shirt made of metal rings",
        "is a popular outfit among common soldiers.",
        "It takes a while to adjust to its weight,",
        "but it offers good overall protection",
        "and is easy to repair."
      };

      AddBonus(ic, { ItemBonusType::RES, cursedPenalty - 3 });
      AddBonus(ic, { ItemBonusType::SPD, cursedPenalty - 2 });

      baseDurability += 4 * (int)ic->Data.ItemQuality_;

      break;

    case ArmorType::SCALE:
      ic->Data.UnidentifiedDescription =
      {
      // ======================================================================70
        "A body vest with overlapping scales worn over a small mail shirt.",
      };

      AddBonus(ic, { ItemBonusType::RES, cursedPenalty - 4 });
      AddBonus(ic, { ItemBonusType::SPD, cursedPenalty - 3 });

      baseDurability += 5 * (int)ic->Data.ItemQuality_;

      break;

    case ArmorType::PLATE:
      ic->Data.UnidentifiedDescription =
      {
      // ======================================================================70
        "A thick layer of padding, then a layer of a strong mail",
        "with metal plates riveted on top.",
        "The best protection you can find, usually worn by",
        "nobles and knights, this armor pretty much combines",
        "all others in itself."
      };

      AddBonus(ic, { ItemBonusType::RES, cursedPenalty - 6 });
      AddBonus(ic, { ItemBonusType::SPD, cursedPenalty - 4 });

      baseDurability += 6 * (int)ic->Data.ItemQuality_;

      break;
  }

  //ic->Data.IdentifiedDescription = ic->Data.UnidentifiedDescription;

  int randomDurAdd = RNG::Instance().RandomRange(0, baseDurability * 0.1f) + dungeonLevel;

  int durability = baseDurability + randomDurAdd;
  ic->Data.Durability.Reset(durability);

  ic->Data.UnidentifiedName = "?" + go->ObjectName + "?";
  ic->Data.IdentifiedName = go->ObjectName;

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateDoor(int x, int y,
                                           bool isOpen,
                                           const std::string& doorName,
                                           int hitPoints,
                                           const std::string& fgOverrideColor,
                                           const std::string& bgOverrideColor)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->PosX = x;
  go->PosY = y;

  if (hitPoints > 0)
  {
    go->Attrs.Indestructible = false;
    go->Attrs.HP.Reset(hitPoints);
  }

  DoorComponent* dc = go->AddComponent<DoorComponent>();
  dc->IsOpen = isOpen;
  dc->FgColorOverride = fgOverrideColor;
  dc->BgColorOverride = bgOverrideColor;
  dc->UpdateDoorState();

  // https://stackoverflow.com/questions/15264003/using-stdbind-with-member-function-use-object-pointer-or-not-for-this-argumen/15264126#15264126
  //
  // When using std::bind to bind a member function, the first argument is the object's this pointer.

  //dc->OwnerGameObject->InteractionCallback = std::bind(&GameObjectsFactory::DoorUseHandler, this, dc);
  dc->OwnerGameObject->InteractionCallback = std::bind(&GameObjectsFactory::DoorUseHandler, this, dc);
  dc->OwnerGameObject->ObjectName = doorName;

  return go;
}

GameObject* GameObjectsFactory::CreateStaticObject(int x, int y, const GameObjectInfo& objectInfo, int hitPoints, GameObjectType type)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->PosX = x;
  go->PosY = y;
  go->Image = objectInfo.Image;
  go->ObjectName = objectInfo.ObjectName;
  go->FogOfWarName = objectInfo.FogOfWarName;
  go->FgColor = objectInfo.FgColor;
  go->BgColor = objectInfo.BgColor;
  go->Blocking = objectInfo.IsBlocking;
  go->BlocksSight = objectInfo.BlocksSight;
  go->Type = type;

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

  copy->VisibilityRadius    = copyFrom->VisibilityRadius;
  copy->PosX                = copyFrom->PosX;
  copy->PosY                = copyFrom->PosY;
  copy->Special             = copyFrom->Special;
  copy->Blocking            = copyFrom->Blocking;
  copy->BlocksSight         = copyFrom->BlocksSight;
  copy->Revealed            = copyFrom->Revealed;
  copy->Visible             = copyFrom->Visible;
  copy->Occupied            = copyFrom->Occupied;
  copy->IsDestroyed         = copyFrom->IsDestroyed;
  copy->Image               = copyFrom->Image;
  copy->FgColor             = copyFrom->FgColor;
  copy->BgColor             = copyFrom->BgColor;
  copy->ObjectName          = copyFrom->ObjectName;
  copy->FogOfWarName        = copyFrom->FogOfWarName;
  copy->InteractionCallback = copyFrom->InteractionCallback;
  copy->Attrs               = copyFrom->Attrs;
  copy->HealthRegenTurns    = copyFrom->HealthRegenTurns;
  copy->Type                = copyFrom->Type;
  copy->IsLiving            = copyFrom->IsLiving;

  copy->_activeEffects           = copyFrom->_activeEffects;
  copy->_previousCell            = copyFrom->_previousCell;
  copy->_currentCell             = copyFrom->_currentCell;
  copy->_healthRegenTurnsCounter = copyFrom->_healthRegenTurnsCounter;
  copy->_manaRegenTurnsCounter   = copyFrom->_manaRegenTurnsCounter;

  return copy;
}

// ************************** PRIVATE METHODS ************************** //

void GameObjectsFactory::SetMagicItemName(ItemComponent* itemRef, const std::vector<ItemBonusType>& bonusesRolled)
{
  std::string prefix;
  std::string suffix;
  std::vector<std::string> rarePrefixes;

  switch (bonusesRolled.size())
  {
    case 1:
      prefix = GlobalConstants::ItemBonusPrefixes.at(bonusesRolled[0]);
      itemRef->Data.Rarity = ItemRarity::MAGIC;
      break;

    case 2:
      prefix = GlobalConstants::ItemBonusPrefixes.at(bonusesRolled[0]);
      suffix = GlobalConstants::ItemBonusSuffixes.at(bonusesRolled[1]);
      itemRef->Data.Rarity = ItemRarity::MAGIC;
      break;

    case 3:
    {
      // Randomize resulting name a bit in case of more than 2 bonuses

      std::vector<ItemBonusType> bonusesRolledCopy = bonusesRolled;

      int suffixInd1 = RNG::Instance().RandomRange(0, 3);
      auto prefix1 = GlobalConstants::ItemBonusPrefixes.at(bonusesRolledCopy[suffixInd1]);
      rarePrefixes.push_back(prefix1);
      bonusesRolledCopy.erase(bonusesRolledCopy.begin() + suffixInd1);

      int suffixInd2 = RNG::Instance().RandomRange(0, 2);
      auto prefix2 = GlobalConstants::ItemBonusPrefixes.at(bonusesRolledCopy[suffixInd2]);
      rarePrefixes.push_back(prefix2);
      bonusesRolledCopy.erase(bonusesRolledCopy.begin() + suffixInd2);

      suffix = GlobalConstants::ItemBonusSuffixes.at(bonusesRolledCopy[0]);

      itemRef->Data.Rarity = ItemRarity::RARE;
    }
    break;
  }

  std::string objName = itemRef->OwnerGameObject->ObjectName;

  std::string bucStatus = "Uncursed";
  if (itemRef->Data.Prefix == ItemPrefix::BLESSED)
  {
    bucStatus = "Blessed";
  }
  else if (itemRef->Data.Prefix == ItemPrefix::CURSED)
  {
    bucStatus = "Cursed";
  }

  std::string quality = GlobalConstants::QualityNameByQuality.at(itemRef->Data.ItemQuality_);
  if (!quality.empty())
  {
    bucStatus += " ";
  }

  std::string itemName = bucStatus + quality + " " + objName;
  if (bonusesRolled.size() == 1)
  {
    itemName = bucStatus + quality + " " + prefix + " " + objName;
  }
  else if (bonusesRolled.size() == 2)
  {
    itemName = bucStatus + quality + " " + prefix + " " + objName + " " + suffix;
  }
  else if (bonusesRolled.size() == 3)
  {
    itemName = bucStatus + quality + " " + rarePrefixes[0] + " " + rarePrefixes[1] + " " + objName + " " + suffix;
  }

  itemRef->Data.IdentifiedName = itemName;
}

void GameObjectsFactory::SetItemName(GameObject* go, ItemData& itemData)
{
  // Insertion to front goes in stack-like order:
  // objName + ItemQuality + BUC = BUC_ItemQuality_objName
  //
  switch (itemData.ItemQuality_)
  {    
    case ItemQuality::DAMAGED:
    case ItemQuality::FLAWED:
    case ItemQuality::FINE:
    case ItemQuality::EXCEPTIONAL:
      std::string qualityName = GlobalConstants::QualityNameByQuality.at(itemData.ItemQuality_);
      std::string strToInsert = qualityName + " ";
      itemData.IdentifiedName.insert(0, strToInsert);
      break;
  }

  switch (itemData.Prefix)
  {
    case ItemPrefix::BLESSED:
      itemData.IdentifiedName.insert(0, "Blessed ");      
      break;

    case ItemPrefix::UNCURSED:
      itemData.IdentifiedName.insert(0, "Uncursed ");
      break;

    case ItemPrefix::CURSED:
      itemData.IdentifiedName.insert(0, "Cursed ");      
      break;
  }

  switch (itemData.ItemType_)
  {
    case ItemType::HEALING_POTION:
      itemData.IdentifiedName.append(" of Healing");
      go->ObjectName.append(" +HP");
      break;

    case ItemType::MANA_POTION:
      itemData.IdentifiedName.append(" of Mana");
      go->ObjectName.append(" +MP");
      break;

    case ItemType::HUNGER_POTION:
      itemData.IdentifiedName.append(" of Satiation");
      go->ObjectName.append(" +SAT");
      break;

    case ItemType::EXP_POTION:
      itemData.IdentifiedName.append(" of Enlightenment");
      go->ObjectName.append(" +EXP");
      break;

    case ItemType::STR_POTION:
      itemData.IdentifiedName.append(" of Strength");
      go->ObjectName.append(" +STR");
      break;

    case ItemType::DEF_POTION:
      itemData.IdentifiedName.append(" of Defence");
      go->ObjectName.append(" +DEF");
      break;

    case ItemType::MAG_POTION:
      itemData.IdentifiedName.append(" of Magic");
      go->ObjectName.append(" +MAG");
      break;

    case ItemType::RES_POTION:
      itemData.IdentifiedName.append(" of Resistance");
      go->ObjectName.append(" +RES");
      break;

    case ItemType::SKL_POTION:
      itemData.IdentifiedName.append(" of Skill");
      go->ObjectName.append(" +SKL");
      break;

    case ItemType::SPD_POTION:
      itemData.IdentifiedName.append(" of Speed");
      go->ObjectName.append(" +SPD");
      break;

    case ItemType::REPAIR_KIT:
    {
      if (itemData.Prefix == ItemPrefix::BLESSED)
      {
        itemData.IdentifiedDescription.push_back("Because of its excellent condition,");
        itemData.IdentifiedDescription.push_back("repairing will be more effective.");
      }
      else if (itemData.Prefix == ItemPrefix::CURSED)
      {
        itemData.IdentifiedDescription.push_back("Because of its poor condition,");
        itemData.IdentifiedDescription.push_back("repairing will be less effective.");
      }
    }
    break;

    case ItemType::ARROWS:
    {
      if (itemData.Prefix == ItemPrefix::BLESSED)
      {
        itemData.IdentifiedDescription.push_back("These projectiles are blessed");
        itemData.IdentifiedDescription.push_back("and thus more likely to hit the enemy.");
      }
      else if (itemData.Prefix == ItemPrefix::CURSED)
      {
        itemData.IdentifiedDescription.push_back("These projectiles are cursed");
        itemData.IdentifiedDescription.push_back("and thus less likely to hit the enemy.");
      }
    }
    break;

    default:
    {
      if (itemData.Prefix == ItemPrefix::BLESSED
      && (itemData.ItemType_ != ItemType::WEAPON
       && itemData.ItemType_ != ItemType::ARMOR
       && itemData.ItemType_ != ItemType::RANGED_WEAPON))
      {
        itemData.IdentifiedDescription.push_back("This one is blessed and will perform better.");
      }
      else if (itemData.Prefix == ItemPrefix::CURSED
           && (itemData.ItemType_ != ItemType::WEAPON
            && itemData.ItemType_ != ItemType::ARMOR
            && itemData.ItemType_ != ItemType::RANGED_WEAPON))
      {
        itemData.IdentifiedDescription.push_back("This one is cursed and should probably be avoided.");
      }
    }
    break;
  }
}

ItemPrefix GameObjectsFactory::RollItemPrefix()
{
  int maxLevel = (int)MapType::THE_END;

  int dungeonLevel = Map::Instance().CurrentLevel->DungeonLevel;
  if (Map::Instance().CurrentLevel->MapType_ == MapType::TOWN)
  {
    dungeonLevel = maxLevel / 2;
  }

  float cursedScale = 2.0f;
  float blessedScale = 4.0f;

  int cursedRate = (int)((float)(maxLevel - dungeonLevel) / cursedScale);
  int blessedRate = (int)((float)(cursedRate + dungeonLevel) / blessedScale);

  std::map<ItemPrefix, int> weights =
  {
    { ItemPrefix::UNCURSED, dungeonLevel },
    { ItemPrefix::CURSED,   cursedRate   },
    { ItemPrefix::BLESSED,  blessedRate  }
  };

  auto res = Util::WeightedRandom(weights);
  ItemPrefix prefix = res.first;

  return prefix;
}

ItemQuality GameObjectsFactory::RollItemQuality()
{
  int maxLevel = (int)MapType::THE_END;

  int dungeonLevel = Map::Instance().CurrentLevel->DungeonLevel;
  if (Map::Instance().CurrentLevel->MapType_ == MapType::TOWN)
  {
    dungeonLevel = maxLevel / 2;
  }

  int rate = dungeonLevel / 2;

  float dmgdRateScale = 1.5f;
  float flawedRateScale = 1.125f;

  int fine = Util::Clamp(rate, 1, maxLevel);
  int exceptional = Util::Clamp(rate / 2, 1, maxLevel);
  int damaged = (int)((float)(maxLevel - dungeonLevel) * dmgdRateScale);
  int flawed = (int)((float)(maxLevel - dungeonLevel) * flawedRateScale);

  //printf("%i rate: %i f: %i e: %i maxlevel: %i diff: %i\n", dungeonLevel, rate, fine, exceptional, maxLevel, maxLevel - dungeonLevel);

  std::map<ItemQuality, int> weights =
  {
    { ItemQuality::DAMAGED,     damaged      },
    { ItemQuality::FLAWED,      flawed       },
    { ItemQuality::NORMAL,      dungeonLevel },
    { ItemQuality::FINE,        fine         },
    { ItemQuality::EXCEPTIONAL, exceptional  },
  };

  auto res = Util::WeightedRandom(weights);
  ItemQuality quality = res.first;

  return quality;
}

bool GameObjectsFactory::ProcessItemEquiption(ItemComponent* item)
{
  bool res = true;

  auto itemEquipped = _playerRef->EquipmentByCategory[item->Data.EqCategory][0];

  if (itemEquipped == nullptr)
  {
    // If nothing was equipped, equip item
    EquipItem(item);
  }
  else if (itemEquipped != item)
  {
    Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Information", { "Unequip first!" }, GlobalConstants::MessageBoxRedBorderColor);
    res = false;
  }
  else
  {
    if (itemEquipped->Data.Prefix == ItemPrefix::CURSED)
    {
      itemEquipped->Data.IsPrefixDiscovered = true;
      auto str = Util::StringFormat("You can't unequip %s - it's cursed!", itemEquipped->OwnerGameObject->ObjectName.data());
      Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Epic Fail!", { str }, GlobalConstants::MessageBoxRedBorderColor);
      res = false;
    }
    else
    {      
      // If it's the same item, just unequip it
      UnequipItem(itemEquipped);
    }
  }

  return res;
}

bool GameObjectsFactory::ProcessRingEquiption(ItemComponent* item)
{
  bool emptySlotFound = false;

  auto& rings = _playerRef->EquipmentByCategory[item->Data.EqCategory];

  // First, search if this ring is already equipped
  for (size_t i = 0; i < rings.size(); i++)
  {
    if (rings[i] == item)
    {
      if (rings[i]->Data.Prefix == ItemPrefix::CURSED)
      {
        rings[i]->Data.IsPrefixDiscovered = true;
        auto str = Util::StringFormat("You can't unequip %s - it's cursed!", rings[i]->OwnerGameObject->ObjectName.data());
        Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Epic Fail!", { str }, GlobalConstants::MessageBoxRedBorderColor);
        return false;
      }

      UnequipRing(rings[i], i);
      return true;
    }
  }

  // Second, if it's different item, try to find empty slot for it
  for (size_t i = 0; i < rings.size(); i++)
  {
    if (rings[i] == nullptr)
    {
      EquipRing(item, i);
      return true;
    }
  }

  // Finally, if no empty slots found, display a warning
  if (!emptySlotFound)
  {
    Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Information", { "Unequip first!" }, GlobalConstants::MessageBoxRedBorderColor);
  }

  return false;
}

void GameObjectsFactory::EquipItem(ItemComponent* item)
{
  item->Data.IsEquipped = true;
  _playerRef->EquipmentByCategory[item->Data.EqCategory][0] = item;

  std::string verb;

  if (item->Data.EqCategory == EquipmentCategory::WEAPON)
  {
    verb = "arm yourself with";
  }
  else
  {
    verb = "put on";
  }

  _playerRef->ApplyBonuses(item);

  std::string objName = item->Data.IsIdentified ? item->OwnerGameObject->ObjectName : item->Data.UnidentifiedName;

  auto message = Util::StringFormat("You %s %s", verb.data(), objName.data());
  Printer::Instance().AddMessage(message);
}

void GameObjectsFactory::UnequipItem(ItemComponent* item)
{
  item->Data.IsEquipped = false;
  _playerRef->EquipmentByCategory[item->Data.EqCategory][0] = nullptr;

  std::string verb;

  if (item->Data.EqCategory == EquipmentCategory::WEAPON)
  {
    verb = "put away";
  }
  else
  {
    verb = "take off";
  }

  _playerRef->UnapplyBonuses(item);

  std::string objName = item->Data.IsIdentified ? item->OwnerGameObject->ObjectName : item->Data.UnidentifiedName;

  auto message = Util::StringFormat("You %s %s", verb.data(), objName.data());
  Printer::Instance().AddMessage(message);
}

void GameObjectsFactory::EquipRing(ItemComponent* ring, int index)
{
  ring->Data.IsEquipped = true;
  _playerRef->EquipmentByCategory[ring->Data.EqCategory][index] = ring;

  _playerRef->ApplyBonuses(ring);

  std::string objName = ring->Data.IsIdentified ? ring->OwnerGameObject->ObjectName : ring->Data.UnidentifiedName;

  auto str = Util::StringFormat("You put on %s", objName.data());
  Printer::Instance().AddMessage(str);
}

void GameObjectsFactory::UnequipRing(ItemComponent* ring, int index)
{
  ring->Data.IsEquipped = false;
  _playerRef->EquipmentByCategory[ring->Data.EqCategory][index] = nullptr;

  _playerRef->UnapplyBonuses(ring);

  std::string objName = ring->Data.IsIdentified ? ring->OwnerGameObject->ObjectName : ring->Data.UnidentifiedName;

  auto str = Util::StringFormat("You take off %s", objName.data());
  Printer::Instance().AddMessage(str);
}

bool GameObjectsFactory::HealingPotionUseHandler(ItemComponent* item)
{
  int amount = 0;

  int statMax = _playerRef->Attrs.HP.Max().Get();
  int statCur = _playerRef->Attrs.HP.Min().Get();

  std::string message;

  float scale = 0.4f;

  if (item->Data.Prefix == ItemPrefix::BLESSED)
  {
    amount = statMax;
    message = (statCur == statMax)
              ? "Nothing happens"
              : "Your wounds are healed completely!";
  }
  else if (item->Data.Prefix == ItemPrefix::UNCURSED)
  {
    amount = statMax * scale;
    message = "You feel better";
    message = (statCur == statMax)
              ? "Nothing happens"
              : "You feel better";
  }
  else if (item->Data.Prefix == ItemPrefix::CURSED)
  {
    amount = statMax * (scale / 2.0f);

    int var = RNG::Instance().RandomRange(0, 3);
    if (var == 0)
    {
      message = (statCur == statMax)
                ? "Nothing happens"
                : "You feel a little better";
    }
    else if (var == 1)
    {
      amount = -amount;
      message = "You are damaged by a cursed potion!";
    }
    else if (var == 2)
    {
      message = "You feel unwell!";

      ItemBonusStruct e;
      e.Type = ItemBonusType::POISONED;
      e.Duration = 50;
      e.Period = 10;
      e.BonusValue = -1;
      e.Cumulative = true;

      _playerRef->AddEffect(e);
    }
  }

  Printer::Instance().AddMessage(message);

  _playerRef->Attrs.HP.AddMin(amount);

  Application::Instance().ChangeState(GameStates::MAIN_STATE);

  return true;
}

bool GameObjectsFactory::ManaPotionUseHandler(ItemComponent* item)
{
  int amount = 0;

  int statMax = _playerRef->Attrs.MP.Max().Get();

  if (item->Data.Prefix == ItemPrefix::BLESSED)
  {
    amount = statMax;
    Printer::Instance().AddMessage("Your spirit force was restored!");
  }
  else if (item->Data.Prefix == ItemPrefix::UNCURSED)
  {
    amount = statMax * 0.3f;
    Printer::Instance().AddMessage("Your spirit is reinforced");
  }
  else if (item->Data.Prefix == ItemPrefix::CURSED)
  {
    amount = -statMax * 0.3f;
    Printer::Instance().AddMessage("Your spirit force was drained!");
  }

  _playerRef->Attrs.MP.AddMin(amount);

  Application::Instance().ChangeState(GameStates::MAIN_STATE);

  return true;
}

bool GameObjectsFactory::HungerPotionUseHandler(ItemComponent* item)
{
  int amount = 0;

  int statMax = _playerRef->Attrs.Hunger;
  int& statCur = _playerRef->Attrs.Hunger;

  std::string message;

  if (item->Data.Prefix == ItemPrefix::BLESSED)
  {
    amount = statMax;
    message = (statCur == statMax)
              ? "Nothing happens"
              : "You feel satiated!";
  }
  else if (item->Data.Prefix == ItemPrefix::UNCURSED)
  {
    amount = statMax * 0.3f;
    message = (statCur == statMax)
              ? "Nothing happens"
              : "Your hunger has abated somewhat";
  }
  else if (item->Data.Prefix == ItemPrefix::CURSED)
  {
    amount = -statMax * 0.3f;
    message = "Your feel peckish";
  }

  Printer::Instance().AddMessage(message);

  statCur += amount;
  statCur = Util::Clamp(statCur, 0, statMax);

  Application::Instance().ChangeState(GameStates::MAIN_STATE);

  return true;
}

bool GameObjectsFactory::ExpPotionUseHandler(ItemComponent* item)
{
  int amount = 0;

  int statMax = 100;

  if (item->Data.Prefix == ItemPrefix::BLESSED)
  {
    amount = statMax;
    Printer::Instance().AddMessage("You feel enlighted!");
  }
  else if (item->Data.Prefix == ItemPrefix::UNCURSED)
  {
    amount = statMax * 0.3f;
    Printer::Instance().AddMessage("You feel more experienced");
  }
  else if (item->Data.Prefix == ItemPrefix::CURSED)
  {
    amount = -statMax * 0.3f;
    Printer::Instance().AddMessage("You lose some experience!");
  }

  Application::Instance().ChangeState(GameStates::MAIN_STATE);

  _playerRef->AwardExperience(amount);

  return true;
}

bool GameObjectsFactory::StatPotionUseHandler(ItemComponent* item)
{
  ItemPrefix buc = item->Data.Prefix;

  std::map<ItemType, std::string> useMessagesByType =
  {
    { ItemType::STR_POTION, "Your Strength has " },
    { ItemType::DEF_POTION, "Your Defence has " },
    { ItemType::MAG_POTION, "Your Magic has " },
    { ItemType::RES_POTION, "Your Resistance has " },
    { ItemType::SKL_POTION, "Your Skill has " },
    { ItemType::SPD_POTION, "Your Speed has " }
  };

  int valueToAdd = 0;

  auto message = useMessagesByType[item->Data.ItemType_];

  if (buc == ItemPrefix::UNCURSED)
  {
    valueToAdd = 1;
    message += "increased!";
  }
  else if (buc == ItemPrefix::BLESSED)
  {
    valueToAdd = 2;
    message += "increased significantly!";
  }
  else if (buc == ItemPrefix::CURSED)
  {
    valueToAdd = -1;
    message += "decreased!";
  }

  Printer::Instance().AddMessage(message);

  std::map<ItemType, Attribute&> playerStats =
  {
    { ItemType::STR_POTION, _playerRef->Attrs.Str },
    { ItemType::DEF_POTION, _playerRef->Attrs.Def },
    { ItemType::MAG_POTION, _playerRef->Attrs.Mag },
    { ItemType::RES_POTION, _playerRef->Attrs.Res },
    { ItemType::SKL_POTION, _playerRef->Attrs.Skl },
    { ItemType::SPD_POTION, _playerRef->Attrs.Spd }
  };

  auto itemType = item->Data.ItemType_;

  int newValue = playerStats.at(itemType).OriginalValue() + valueToAdd;

  if (newValue < 0)
  {
    newValue = 0;
    message = "Nothing happens";
  }

  playerStats.at(itemType).Set(newValue);

  Application::Instance().ChangeState(GameStates::MAIN_STATE);

  return true;
}

void GameObjectsFactory::BUCQualityAdjust(ItemData& itemData)
{
  switch (itemData.Prefix)
  {
    case ItemPrefix::CURSED:
    {
      itemData.Damage.AddMax(-1);
      int oldMin = itemData.Damage.Min().OriginalValue();
      if (oldMin - 1 != 0)
      {
        itemData.Damage.SetMin(oldMin - 1);
      }
    }
    break;

    case ItemPrefix::BLESSED:
    {
      itemData.Damage.AddMax(1);
      itemData.Damage.AddMin(1);
    }
    break;
  }  

  switch (itemData.ItemQuality_)
  {
    case ItemQuality::DAMAGED:
    {
      int dur = itemData.Durability.Max().Get();
      itemData.Durability.Reset(dur / 2);

      itemData.Damage.AddMax(-1);
      int oldMin = itemData.Damage.Min().OriginalValue();
      if (oldMin - 1 != 0)
      {
        itemData.Damage.SetMin(oldMin - 1);
      }
    }
    break;

    case ItemQuality::FLAWED:
    {
      float durF = (float)itemData.Durability.Max().Get() / 1.5f;
      itemData.Durability.Reset((int)durF);

      int oldMin = itemData.Damage.Min().OriginalValue();
      if (oldMin - 1 == 0)
      {
        itemData.Damage.AddMax(-1);
      }
      else
      {
        itemData.Damage.AddMin(-1);
      }
    }
    break;

    case ItemQuality::FINE:
    {
      float durF = (float)itemData.Durability.Max().Get() * 1.5f;
      itemData.Durability.Reset((int)durF);

      //int oldMin = itemData.Damage.Min().OriginalValue();
      //itemData.Damage.SetMin(oldMin + 1);
    }
    break;

    case ItemQuality::EXCEPTIONAL:
    {
      int dur = itemData.Durability.Max().Get();
      itemData.Durability.Reset(dur * 2);
      //itemData.Damage.AddMax(1);
      //itemData.Damage.AddMin(1);
    }
    break;
  }
}

size_t GameObjectsFactory::CalculateItemHash(ItemComponent* item)
{
  auto strToHash = std::to_string((int)item->Data.Prefix) + item->OwnerGameObject->ObjectName;
  std::hash<std::string> hasher;

  return hasher(strToHash);
}

int GameObjectsFactory::CalculateAverageDamage(int numRolls, int diceSides)
{
  int minDmg = (numRolls == 1) ? 0 : numRolls;
  int maxDmg = numRolls * diceSides;

  return (maxDmg - minDmg) / 2;
}

void GameObjectsFactory::GenerateLootIfPossible(int posX, int posY, GameObjectType monsterType)
{
  if (GlobalConstants::LootTable.count(monsterType) == 1)
  {
    auto weights = GlobalConstants::LootTable.at(monsterType);
    auto kvp = Util::WeightedRandom(weights);

    GenerateLoot(posX, posY, kvp, monsterType);
  }
}

void GameObjectsFactory::GenerateLoot(int posX, int posY, const std::pair<ItemType, int>& kvp, GameObjectType monsterType)
{
  switch (kvp.first)
  {
    case ItemType::NOTHING:
      break;

    case ItemType::FOOD:
    {
      auto foodMap = GlobalConstants::FoodLootTable.at(monsterType);
      auto kvp = Util::WeightedRandom(foodMap);

      auto go = CreateFood(posX, posY, kvp.first);
      Map::Instance().InsertGameObject(go);
    }
    break;

    default:
    {
      auto go = CreateGameObject(posX, posY, kvp.first);
      Map::Instance().InsertGameObject(go);
    }
    break;
  }
}

GameObject* GameObjectsFactory::CreateRandomGlass()
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->Image = '*';

  int colorIndex = RNG::Instance().RandomRange(1, GlobalConstants::GemColorNameByType.size());

  GemType t = (GemType)colorIndex;

  go->FgColor = GlobalConstants::GemColorByType.at(t).first;
  go->BgColor = GlobalConstants::GemColorByType.at(t).second;

  std::string colorDesc = GlobalConstants::GemColorNameByType.at(t);
  go->ObjectName = Util::StringFormat("%s Glass", colorDesc.data());

  ItemComponent* ic = go->AddComponent<ItemComponent>();
  ic->Data.ItemType_ = ItemType::GEM;
  ic->Data.IsStackable = false;
  ic->Data.IsIdentified = false;

  ic->Data.UnidentifiedDescription = { "Is this valuable?" };
  ic->Data.UnidentifiedName = Util::StringFormat("?%s Gem?", colorDesc.data());

  std::string lowerCase = colorDesc;  
  std::transform(lowerCase.begin(), lowerCase.end(), lowerCase.begin(), ::tolower);

  auto str = Util::StringFormat("This is a piece of %s worthless glass", lowerCase.data());
  ic->Data.IdentifiedDescription = { str };

  ic->Data.IdentifiedName = Util::StringFormat("%s worthless glass", colorDesc.data());
  ic->Data.Cost = 0;

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateGemHelper(GemType t, ItemQuality quality)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->Image = '*';

  go->FgColor = GlobalConstants::GemColorByType.at(t).first;
  go->BgColor = GlobalConstants::GemColorByType.at(t).second;

  go->ObjectName = GlobalConstants::GemNameByType.at(t);

  std::string colorDesc = GlobalConstants::GemColorNameByType.at(t);

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_ = ItemType::GEM;
  ic->Data.IsStackable = false;
  ic->Data.IsIdentified = false;

  ic->Data.UnidentifiedDescription = { "Is this valuable?" };
  ic->Data.UnidentifiedName = Util::StringFormat("?%s Gem?", colorDesc.data());

  auto str = Util::StringFormat("%s description goes here", GlobalConstants::GemNameByType.at(t).data());
  ic->Data.IdentifiedDescription = { str };

  ic->Data.IdentifiedName = GlobalConstants::GemNameByType.at(t);
  ic->Data.Cost = GlobalConstants::GemCostByType.at(t);

  ic->Data.ItemQuality_ = (quality != ItemQuality::RANDOM) ? quality : RollItemQuality();

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

void GameObjectsFactory::InitPotionColors()
{
  using PotionsMap = std::map<std::string, std::vector<std::string>>;

  _gamePotionsMap.clear();

  std::vector<ItemType> potionTypes =
  {
    ItemType::HEALING_POTION,
    ItemType::MANA_POTION,
    ItemType::HUNGER_POTION,
    ItemType::STR_POTION,
    ItemType::DEF_POTION,
    ItemType::MAG_POTION,
    ItemType::RES_POTION,
    ItemType::SKL_POTION,
    ItemType::SPD_POTION,
    ItemType::EXP_POTION
  };

  PotionsMap potions = GlobalConstants::PotionColorsByName;

  while (potionTypes.size() != 0)
  {
    int potionsTypeIndex = RNG::Instance().RandomRange(0, potionTypes.size());
    ItemType t = potionTypes[potionsTypeIndex];

    int potionsMapIndex = RNG::Instance().RandomRange(0, potions.size());
    auto it = potions.begin();
    std::advance(it, potionsMapIndex);

    PotionInfo pi;

    pi.PotionType = t;
    pi.FgBgColor = { it->second[0], it->second[1] };
    pi.PotionName = it->first;

    _gamePotionsMap[t] = pi;

    //auto str = Util::StringFormat("%s = %s", GlobalConstants::PotionNameByType.at(t).data(), it->first.data());
    //Logger::Instance().Print(str);

    potionTypes.erase(potionTypes.begin() + potionsTypeIndex);
    potions.erase(it);
  }
}

void GameObjectsFactory::InitScrolls()
{
  _gameScrollsMap.clear();

  auto scrollNames = GlobalConstants::ScrollUnidentifiedNames;
  auto validSpells = GlobalConstants::ScrollValidSpellTypes;

  while (scrollNames.size() != 0)
  {
    int scrollNameIndex = RNG::Instance().RandomRange(0, scrollNames.size());
    std::string scrollName = scrollNames[scrollNameIndex];

    int spellIndex = RNG::Instance().RandomRange(0, validSpells.size());
    SpellType spell = validSpells[spellIndex];

    ScrollInfo si;

    si.ScrollName = scrollName;
    si.SpellType_ = spell;

    _gameScrollsMap[spell] = si;

    //auto str = Util::StringFormat("%s = %s", GlobalConstants::SpellNameByType.at(si.SpellType_).data(), si.ScrollName.data());
    //Logger::Instance().Print(str);

    // printf("%s = %s\n", GlobalConstants::SpellNameByType.at(si.SpellType_).data(), si.ScrollName.data());

    scrollNames.erase(scrollNames.begin() + scrollNameIndex);
    validSpells.erase(validSpells.begin() + spellIndex);
  }
}

bool GameObjectsFactory::ReturnerUseHandler(ItemComponent* item)
{
  if (!item->Data.IsIdentified)
  {
    Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Information", { "Can't be used!" }, GlobalConstants::MessageBoxRedBorderColor);
    return false;
  }

  if (item->Data.Amount == 0)
  {
    Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Information", { "You invoke the returner, but nothing happens." }, GlobalConstants::MessageBoxDefaultBorderColor);
    return false;
  }

  return true;
}

bool GameObjectsFactory::RepairKitUseHandler(ItemComponent* item)
{
  if (!_playerRef->HasSkill(PlayerSkills::REPAIR))
  {
    Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Epic Fail!", { "You don't possess the necessary skill!" }, GlobalConstants::MessageBoxRedBorderColor);
    return false;
  }

  return true;
}

bool GameObjectsFactory::ScrollUseHandler(ItemComponent* item)
{
  SpellsProcessor::Instance().ProcessScroll(item);
  Application::Instance().ChangeState(GameStates::MAIN_STATE);
  return true;
}

void GameObjectsFactory::DoorUseHandler(DoorComponent* dc)
{
  dc->Interact();
}

void GameObjectsFactory::TryToAddBonuses(ItemComponent* itemRef, bool atLeastOne)
{
  std::map<ItemBonusType, int> bonusWeightByType =
  {
    { ItemBonusType::STR,             6 },
    { ItemBonusType::DEF,             6 },
    { ItemBonusType::MAG,             6 },
    { ItemBonusType::RES,             6 },
    { ItemBonusType::SKL,             6 },
    { ItemBonusType::SPD,             6 },
    { ItemBonusType::HP,             20 },
    { ItemBonusType::MP,             20 },
    { ItemBonusType::INDESTRUCTIBLE,  1 },
    { ItemBonusType::SELF_REPAIR,    10 },
    { ItemBonusType::VISIBILITY,     20 },
    { ItemBonusType::INVISIBILITY,    4 },
    { ItemBonusType::DAMAGE,         20 },
    { ItemBonusType::HUNGER,         10 },
    { ItemBonusType::IGNORE_DEFENCE, 10 },
    { ItemBonusType::KNOCKBACK,      15 },
    { ItemBonusType::MANA_SHIELD,     8 },
    { ItemBonusType::REGEN,           8 },
    { ItemBonusType::REFLECT,         8 },
    { ItemBonusType::LEECH,           8 },
    { ItemBonusType::DMG_ABSORB,     10 },
    { ItemBonusType::MAG_ABSORB,     10 },
    { ItemBonusType::THORNS,         10 },
    { ItemBonusType::TELEPATHY,       6 },
  };

  AdjustBonusWeightsMap(itemRef, bonusWeightByType);

  int curDungeonLvl = Map::Instance().CurrentLevel->DungeonLevel;

  // Increase chance of traders having magic items in town
  if (Map::Instance().CurrentLevel->MapType_ == MapType::TOWN)
  {
    curDungeonLvl = (int)MapType::THE_END / 2;
  }

  const float scale = 1.25f;
  float minSucc = 1.0f / ((float)MapType::THE_END * scale);
  float curSucc = minSucc * curDungeonLvl;
  int chance = (int)(curSucc * 100);

  std::map<ItemQuality, int> chanceModByQ =
  {
    { ItemQuality::DAMAGED,     -10 },
    { ItemQuality::FLAWED,       -5 },
    { ItemQuality::NORMAL,        0 },
    { ItemQuality::FINE,          5 },
    { ItemQuality::EXCEPTIONAL,  10 },
  };

  chance += chanceModByQ[itemRef->Data.ItemQuality_];

  chance = Util::Clamp(chance, 1, 100);

  std::vector<ItemBonusType> bonusesRolled;
  auto bonusesWeightCopy = bonusWeightByType;

  for (int i = 0; i < 3; i++)
  {
    if (Util::Rolld100(chance))
    {
      auto res = Util::WeightedRandom(bonusesWeightCopy);

      // No duplicate bonuses on a single item
      bonusesWeightCopy.erase(res.first);

      AddRandomBonus(itemRef, res.first);

      bonusesRolled.push_back(res.first);
    }
  }

  if (bonusesRolled.size() == 0 && atLeastOne)
  {
    auto res = Util::WeightedRandom(bonusWeightByType);
    AddRandomBonus(itemRef, res.first);
    bonusesRolled.push_back(res.first);
  }

  SetMagicItemName(itemRef, bonusesRolled);
}

void GameObjectsFactory::AddBonus(ItemComponent* itemRef, const ItemBonusStruct& bonusData, bool forceAdd)
{
  // If bonus doesn't modify anything,
  // (i.e. if during weapon / armor generation total value of modifier
  // became 0 due to being cursed), don't add it unless forced to
  // (if it's some kind of 'special' item bonus
  // like mana shield or knockback where bonus value is not used).
  if (!forceAdd && bonusData.BonusValue == 0)
  {
    return;
  }

  auto copy = bonusData;

  int moneyIncrease = GlobalConstants::MoneyCostIncreaseByBonusType.at(bonusData.Type);

  // If bonus is like mana shield that has no bonus value, count it only once
  if (bonusData.BonusValue == 0)
  {
    copy.MoneyCostIncrease = moneyIncrease;
  }
  // Negative bonus values aren't taken into account regarding total cost
  else if (bonusData.BonusValue > 0)
  {
    int total = bonusData.BonusValue * moneyIncrease;

    // MoneyCostIncrease is used to calculate total cost of the item
    // in ShoppingState by accumulating all these
    // values into one resulting variable.
    copy.MoneyCostIncrease = total;
  }

  copy.Id = itemRef->OwnerGameObject->ObjectId();
  copy.FromItem = true;

  itemRef->Data.Bonuses.push_back(copy);
}

void GameObjectsFactory::AddRandomBonus(ItemComponent* itemRef, ItemBonusType bonusType)
{
  // No negative bonuses assumed
  int moneyIncrease = GlobalConstants::MoneyCostIncreaseByBonusType.at(bonusType);

  ItemBonusStruct bs;
  bs.Type = bonusType;
  bs.MoneyCostIncrease = (itemRef->Data.Prefix == ItemPrefix::CURSED) ? moneyIncrease / 2 : moneyIncrease;

  std::map<ItemQuality, int> multByQ =
  {
    { ItemQuality::DAMAGED,      1 },
    { ItemQuality::FLAWED,       2 },
    { ItemQuality::NORMAL,       3 },
    { ItemQuality::FINE,         4 },
    { ItemQuality::EXCEPTIONAL,  5 },
  };

  // Probability of stat increase values
  std::map<int, int> statIncreaseWeightsMap =
  {
    { 1, 24 },
    { 2, 12 },
    { 3, 8  },
    { 4, 8  },
    { 5, 8  }
  };

  // TODO: finish other bonuses

  int value = 0;

  ItemQuality q = itemRef->Data.ItemQuality_;

  switch (bonusType)
  {
    case ItemBonusType::STR:
    case ItemBonusType::DEF:
    case ItemBonusType::MAG:
    case ItemBonusType::RES:
    case ItemBonusType::SKL:
    case ItemBonusType::SPD:
    {
      auto res = Util::WeightedRandom(statIncreaseWeightsMap);            
      value = res.first;
      bs.MoneyCostIncrease = res.first * moneyIncrease;
    }
    break;

    case ItemBonusType::HP:
    case ItemBonusType::MP:
    {
      int min = 1 + multByQ[q];
      int max = 10 + multByQ[q] * 2;
      value = RNG::Instance().RandomRange(min, max);
      bs.MoneyCostIncrease = value * moneyIncrease;
    }
    break;

    case ItemBonusType::DMG_ABSORB:
    case ItemBonusType::MAG_ABSORB:
    {
      int min = 1;
      int max = multByQ[q];
      value = RNG::Instance().RandomRange(min, max + 1);
      bs.MoneyCostIncrease = value * moneyIncrease;
    }
    break;

    case ItemBonusType::DAMAGE:
    {
      int min = multByQ[q];
      int max = multByQ[q] * 2;
      value = RNG::Instance().RandomRange(min, max + 1);
      bs.MoneyCostIncrease = value * moneyIncrease;
    }
    break;

    case ItemBonusType::VISIBILITY:
    {
      int min = multByQ[q];
      int max = multByQ[q] * 3;
      value = RNG::Instance().RandomRange(min, max + 1);
      bs.MoneyCostIncrease = value * moneyIncrease;
    }
    break;

    case ItemBonusType::THORNS:
    {
      int min = 20 + 5 * (multByQ[q] - 1);
      int max = 21 + 20 * multByQ[q];
      int percentage = RNG::Instance().RandomRange(min, max);
      value = percentage;
      bs.MoneyCostIncrease = value * moneyIncrease;
    }
    break;

    case ItemBonusType::LEECH:
    {
      int min = 20 + 5 * (multByQ[q] - 1);
      int max = 21 + 15 * multByQ[q];
      int percentage = RNG::Instance().RandomRange(min, max);
      value = percentage;
      bs.MoneyCostIncrease = value * moneyIncrease;
    }
    break;

    case ItemBonusType::SELF_REPAIR:
    case ItemBonusType::REGEN:
    {
      value = 1;

      int min = 20;
      int max = 40;

      // Number of turns before effect acts
      int minVal = min - 2 * multByQ[q];
      int maxVal = max - 2 * multByQ[q];
      bs.Period = RNG::Instance().RandomRange(minVal, maxVal + 1);
      bs.MoneyCostIncrease = (int)(((float)max / (float)bs.Period) * (float)moneyIncrease);
    }
    break;

    case ItemBonusType::KNOCKBACK:
    {
      value = RNG::Instance().RandomRange(1, 4);
      bs.MoneyCostIncrease = value * moneyIncrease;
    }
    break;

    case ItemBonusType::TELEPATHY:
    {
      // There is no range (at least for now)
      value = 1;
      bs.MoneyCostIncrease = value * moneyIncrease;
    }
    break;
  }

  // TODO: should there be cursed magic / rare items or fuck it?
  //
  // If item is cursed, there's 50% chance
  // that we'll get penalty instead of a bonus
  //bool fuckupChance = Util::Rolld100(50);

  //bs.IsCursed = fuckupChance;
  //bs.Value = (itemRef->Data.Prefix == ItemPrefix::CURSED && fuckupChance) ? -value : value;

  bs.BonusValue = value;
  bs.Id = itemRef->OwnerGameObject->ObjectId();
  bs.FromItem = true;

  itemRef->Data.Bonuses.push_back(bs);
}

void GameObjectsFactory::AdjustBonusWeightsMap(ItemComponent* itemRef, std::map<ItemBonusType, int>& bonusWeightByType)
{
  // Certain items shouldn't have certain bonuses
  // that don't make sense (kinda)
  if (itemRef->Data.EqCategory == EquipmentCategory::BOOTS
   || itemRef->Data.EqCategory == EquipmentCategory::HEAD
   || itemRef->Data.EqCategory == EquipmentCategory::LEGS
   || itemRef->Data.EqCategory == EquipmentCategory::TORSO)
  {
    bonusWeightByType.erase(ItemBonusType::DAMAGE);
    bonusWeightByType.erase(ItemBonusType::IGNORE_DEFENCE);
    bonusWeightByType.erase(ItemBonusType::KNOCKBACK);
    bonusWeightByType.erase(ItemBonusType::LEECH);
    bonusWeightByType.erase(ItemBonusType::INVISIBILITY);
    bonusWeightByType.erase(ItemBonusType::TELEPATHY);
  }
  else if (itemRef->Data.EqCategory == EquipmentCategory::WEAPON)
  {
    bonusWeightByType.erase(ItemBonusType::REFLECT);
    bonusWeightByType.erase(ItemBonusType::REGEN);
    bonusWeightByType.erase(ItemBonusType::MANA_SHIELD);
    bonusWeightByType.erase(ItemBonusType::DMG_ABSORB);
    bonusWeightByType.erase(ItemBonusType::MAG_ABSORB);
    bonusWeightByType.erase(ItemBonusType::THORNS);
    bonusWeightByType.erase(ItemBonusType::HUNGER);
    bonusWeightByType.erase(ItemBonusType::INVISIBILITY);
    bonusWeightByType.erase(ItemBonusType::TELEPATHY);
  }
  else if (itemRef->Data.EqCategory == EquipmentCategory::NECK
        || itemRef->Data.EqCategory == EquipmentCategory::RING)
  {
    bonusWeightByType.erase(ItemBonusType::INDESTRUCTIBLE);
    bonusWeightByType.erase(ItemBonusType::SELF_REPAIR);
    bonusWeightByType.erase(ItemBonusType::KNOCKBACK);
    bonusWeightByType.erase(ItemBonusType::DAMAGE);
    bonusWeightByType.erase(ItemBonusType::IGNORE_DEFENCE);
    bonusWeightByType.erase(ItemBonusType::LEECH);
  }
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

  ContainerComponent* cc = go->AddComponent<ContainerComponent>();
  cc->CanBeOpened = false;
  cc->MaxCapacity = maxLevel + 1;

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
      auto item = CreateRandomItem(0, 0);

      // If there is already such stackable item in inventory,
      // delete the game object we just created.
      if (item != nullptr && cc->AddToInventory(item))
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
