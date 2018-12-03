#include "game-objects-factory.h"

#include "constants.h"
#include "rng.h"
#include "item-component.h"
#include "shrine-component.h"
#include "ai-component.h"
#include "ai-monster-basic.h"
#include "ai-npc.h"
#include "stairs-component.h"
#include "go-timer-destroyer.h"
#include "map.h"
#include "application.h"

void GameObjectsFactory::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

GameObject* GameObjectsFactory::CreateGameObject(int x, int y, ItemType objType)
{  
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

    default:
      break;
  }

  go->PosX = x;
  go->PosY = y;

  return go;
}

GameObject* GameObjectsFactory::CreateMonster(int x, int y, MonsterType monsterType)
{
  GameObject* go = nullptr;

  switch (monsterType)
  {
    case MonsterType::RAT:
      go = CreateRat(x, y);
      break;

    case MonsterType::BAT:
      go = CreateBat(x, y);
      break;

    case MonsterType::SPIDER:
      go = CreateSpider(x, y);
      break;
  }

  go->Type = monsterType;

  go->PosX = x;
  go->PosY = y;

  return go;
}

GameObject* GameObjectsFactory::CreateMoney(int amount)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->ObjectName = "Gold Coins";
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
  ic->Data.IdentifiedName = "Gold Coins";

  ic->Data.ItemTypeHash = CalculateHash(ic);

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
  GameObject* go = new GameObject(Map::Instance().CurrentLevel, x, y, '@', "#FFFFFF");

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
  go->ObjectName = "Rat";
  go->Attrs.Indestructible = false;
  go->HealthRegenTurns = 30;

  // Sets Occupied flag for _currentCell
  go->Move(0, 0);

  AIComponent* ai = go->AddComponent<AIComponent>();
  AIMonsterBasic* aimb = ai->AddModel<AIMonsterBasic>();

  aimb->AgroRadius = 8;

  // Set attributes
  if (randomize)
  {
    int pl = _playerRef->Attrs.Lvl.CurrentValue;
    int dl = Map::Instance().CurrentLevel->DungeonLevel;
    int difficulty = std::max(pl, dl); //pl + dl;

    int randomStr = RNG::Instance().RandomRange(1 * difficulty, 2 * difficulty);
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
  }
  else
  {
    // FIXME: for debugging purposes
    go->ObjectName = "Battle Rat";

    go->Attrs.Str.Set(2 * _playerRef->Attrs.Lvl.CurrentValue);
    go->Attrs.Def.Set(1 * _playerRef->Attrs.Lvl.CurrentValue);
    go->Attrs.Spd.Set(10);

    go->Attrs.HP.Set(10);
  }

  ai->ChangeModel<AIMonsterBasic>();

  return go;
}

GameObject* GameObjectsFactory::CreateBat(int x, int y, bool randomize)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel, x, y, 'b', GlobalConstants::MonsterColor);
  go->ObjectName = "Flying Bat";
  go->Attrs.Indestructible = false;
  go->HealthRegenTurns = 20;

  // Sets Occupied flag for _currentCell
  go->Move(0, 0);

  AIComponent* ai = go->AddComponent<AIComponent>();
  AIMonsterBasic* aimb = ai->AddModel<AIMonsterBasic>();

  aimb->AgroRadius = 16;

  // Set attributes
  if (randomize)
  {
    int pl = _playerRef->Attrs.Lvl.CurrentValue;
    int dl = Map::Instance().CurrentLevel->DungeonLevel;
    int difficulty = std::max(pl, dl); //pl + dl;

    int randomStr = RNG::Instance().RandomRange(1 * difficulty, 2 * difficulty);
    int randomDef = RNG::Instance().RandomRange(0, 1 * difficulty);
    int randomSkl = RNG::Instance().RandomRange(1 * difficulty, 2 * difficulty);
    int randomHp = RNG::Instance().RandomRange(1 * difficulty, 3 * difficulty);
    int randomSpd = RNG::Instance().RandomRange(1 * difficulty, 3 * difficulty);

    go->Attrs.Lvl.Set(difficulty);

    go->Attrs.Str.Set(randomStr);
    go->Attrs.Def.Set(randomDef);
    go->Attrs.HP.Set(randomHp);
    go->Attrs.Spd.Set(randomSpd);
    go->Attrs.Skl.Set(randomSkl);
  }

  ai->ChangeModel<AIMonsterBasic>();

  return go;
}

GameObject* GameObjectsFactory::CreateSpider(int x, int y, bool randomize)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel, x, y, 's', GlobalConstants::MonsterColor);
  go->ObjectName = "Spider";
  go->Attrs.Indestructible = false;
  go->HealthRegenTurns = 30;

  // Sets Occupied flag for _currentCell
  go->Move(0, 0);

  AIComponent* ai = go->AddComponent<AIComponent>();
  AIMonsterBasic* aimb = ai->AddModel<AIMonsterBasic>();

  aimb->AgroRadius = 12;

  // Set attributes
  if (randomize)
  {
    int pl = _playerRef->Attrs.Lvl.CurrentValue;
    int dl = Map::Instance().CurrentLevel->DungeonLevel;
    int difficulty = std::max(pl, dl); //pl + dl;

    int randomStr = RNG::Instance().RandomRange(1 * difficulty, 2 * difficulty);
    int randomDef = RNG::Instance().RandomRange(1 * difficulty, 2 * difficulty);
    int randomSkl = RNG::Instance().RandomRange(1 * difficulty, 2 * difficulty);
    int randomHp = RNG::Instance().RandomRange(2 * difficulty, 6 * difficulty);
    int randomSpd = RNG::Instance().RandomRange(0, 2 * difficulty);

    go->Attrs.Lvl.Set(difficulty);

    go->Attrs.Str.Set(randomStr);
    go->Attrs.Def.Set(randomDef);
    go->Attrs.HP.Set(randomHp);
    go->Attrs.Spd.Set(randomSpd);
    go->Attrs.Skl.Set(randomSkl);
  }

  ai->ChangeModel<AIMonsterBasic>();

  return go;
}

GameObject* GameObjectsFactory::CreateRemains(GameObject* from)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel, from->PosX, from->PosY, '%', from->FgColor);

  TimerDestroyerComponent* td = go->AddComponent<TimerDestroyerComponent>();
  td->Time = 200; //from->Attrs.HP.OriginalValue * 2;

  auto str = Util::StringFormat("%s's remains", from->ObjectName.data());
  go->ObjectName = str;

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

  // TODO: cursed items

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
    item->Data.UseCallback(item);
    res = true;
  }
  else
  {
    switch (item->Data.ItemType_)
    {
      case ItemType::COINS:
        Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Information", { "You don't 'use' money like that." }, GlobalConstants::MessageBoxRedBorderColor);
        break;

      default:
        auto go = item->OwnerGameObject;
        auto msg = Util::StringFormat("You can't use %s!", go->ObjectName.data());
        Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Information", { msg }, GlobalConstants::MessageBoxRedBorderColor);
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
  int hungerMax = _playerRef->Attrs.HungerRate.CurrentValue;
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

  ic->Data.ItemTypeHash = CalculateHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateHealingPotion(ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->FgColor = "#FFFFFF";
  go->BgColor = "#FF0000";
  go->Image = '!';
  go->ObjectName = "Red Potion";

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_ = ItemType::HEALING_POTION;
  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;
  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;
  ic->Data.Cost = 100;

  ic->Data.IdentifiedDescription = { "Restores some of your health." };
  ic->Data.IdentifiedName = "Red Potion";

  ic->Data.UseCallback = std::bind(&GameObjectsFactory::HealingPotionUseHandler, this, ic);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateManaPotion(ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->FgColor = "#FFFFFF";
  go->BgColor = "#0000FF";
  go->Image = '!';
  go->ObjectName = "Blue Potion";

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_ = ItemType::MANA_POTION;
  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;
  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;
  ic->Data.Cost = 50;

  ic->Data.IdentifiedDescription = { "Helps you regain spiritual powers." };
  ic->Data.IdentifiedName = "Blue Potion";

  ic->Data.UseCallback = std::bind(&GameObjectsFactory::ManaPotionUseHandler, this, ic);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateHungerPotion(ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->FgColor = "#FFFFFF";
  go->BgColor = "#004400";
  go->Image = '!';
  go->ObjectName = "Slimy Potion";

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_ = ItemType::HUNGER_POTION;
  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;
  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;
  ic->Data.Cost = 50;

  ic->Data.IdentifiedDescription = { "Liquid food. Drink it if there's nothing else to eat." };
  ic->Data.IdentifiedName = "Slimy Potion";

  ic->Data.UseCallback = std::bind(&GameObjectsFactory::HungerPotionUseHandler, this, ic);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateExpPotion(ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->FgColor = "#FFFFFF";
  go->BgColor = "#888888";
  go->Image = '!';
  go->ObjectName = "Clear Potion";

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_ = ItemType::EXP_POTION;
  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;
  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;
  ic->Data.Cost = 250;

  ic->Data.IdentifiedDescription = { "They say drinking this will bring you to the next level.", "Whatever that means..." };
  ic->Data.IdentifiedName = "Clear Potion";

  ic->Data.UseCallback = std::bind(&GameObjectsFactory::ExpPotionUseHandler, this, ic);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateStatPotion(std::string statName, ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->FgColor = "#FFFFFF";
  go->BgColor = "#888888";
  go->Image = '!';
  go->ObjectName = "Radiant Potion";

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_ = GlobalConstants::PotionTypeByStatName.at(statName);
  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;
  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;
  ic->Data.Cost = 500;

  auto str = Util::StringFormat("This will affect your %s", statName);
  ic->Data.IdentifiedDescription = { str };

  ic->Data.IdentifiedName = "Radiant Potion";

  ic->Data.UseCallback = std::bind(&GameObjectsFactory::StatPotionUseHandler, this, ic);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateRandomPotion()
{
  GameObject* go = nullptr;

  auto weights = Util::WeightedRandom(GlobalConstants::PotionsWeightTable);
  go = CreateGameObject(0, 0, weights.first);

  ItemComponent* ic = go->GetComponent<ItemComponent>();

  ic->Data.IsIdentified = false;
  ic->Data.UnidentifiedDescription = { "You don't know what will happen if you drink it." };

  int index = RNG::Instance().RandomRange(0, GlobalConstants::PotionColors.size());
  auto it = GlobalConstants::PotionColors.begin();
  std::advance(it, index);
  auto kvp = *it;

  go->FgColor = kvp.second[0];
  go->BgColor = kvp.second[1];
  go->ObjectName = kvp.first;

  ic->Data.UnidentifiedName = "?" + kvp.first + "?";
  ic->Data.IdentifiedName = kvp.first;

  ic->Data.Prefix = RollItemPrefix();

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateRandomWeapon()
{
  GameObject* go = nullptr;

  int index = RNG::Instance().RandomRange(0, GlobalConstants::WeaponNameByType.size());
  auto it = GlobalConstants::WeaponNameByType.begin();
  std::advance(it, index);
  auto kvp = *it;

  go = CreateWeapon(kvp.first);

  return go;
}

GameObject* GameObjectsFactory::CreateRandomItem(int x, int y)
{
  GameObject* go = nullptr;

  std::vector<ItemType> possibleItems =
  {
    ItemType::COINS,
    ItemType::WEAPON,
    ItemType::POTION,
    ItemType::FOOD
  };

  std::map<FoodType, int> foodMap =
  {
    { FoodType::APPLE, 1 },
    { FoodType::BREAD, 1 },
    { FoodType::CHEESE, 1 },
    { FoodType::MEAT, 1 },
    { FoodType::RATIONS, 1 },
    { FoodType::IRON_RATIONS, 1 }
  };

  int index = RNG::Instance().RandomRange(0, possibleItems.size());

  ItemType res = possibleItems[index];

  switch (res)
  {
    case ItemType::COINS:
      go = CreateMoney();
      break;

    case ItemType::WEAPON:
      go = CreateRandomWeapon();
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
  }

  go->PosX = x;
  go->PosY = y;

  return go;
}

void GameObjectsFactory::CreateStairs(MapLevelBase* levelWhereCreate, int x, int y, chtype image, MapType leadsTo)
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

void GameObjectsFactory::FoodUseHandler(ItemComponent* item)
{
  auto objName = (item->Data.IsIdentified) ? item->Data.IdentifiedName : item->Data.UnidentifiedName;

  auto str = Util::StringFormat("You eat %s...", objName.data());
  Printer::Instance().AddMessage(str);

  if (item->Data.Prefix == ItemPrefix::CURSED)
  {
    Printer::Instance().AddMessage("You feel even more hungry!");

    _playerRef->Attrs.Hunger += item->Data.Cost;
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

  _playerRef->Attrs.Hunger = Util::Clamp(_playerRef->Attrs.Hunger, 0, _playerRef->Attrs.HungerRate.CurrentValue);

  Application::Instance().ChangeState(GameStates::MAIN_STATE);
}

GameObject* GameObjectsFactory::CreateNote(std::string objName, std::vector<std::string> text)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->FgColor = "#000000";
  go->BgColor = "#FFFFFF";
  go->Image = '?';
  go->ObjectName = objName;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_ = ItemType::DUMMY;
  ic->Data.IsStackable = false;
  ic->Data.IsIdentified = true;

  ic->Data.IdentifiedDescription = text;
  ic->Data.IdentifiedName = objName;

  ic->Data.ItemTypeHash = CalculateHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateWeapon(WeaponType type, bool overridePrefix)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  int dungeonLevel = Map::Instance().CurrentLevel->DungeonLevel * 0.5;
  if (dungeonLevel == 0)
  {
    dungeonLevel = 1;
  }

  go->ObjectName = GlobalConstants::WeaponNameByType.at(type);
  go->Image = ')';
  go->FgColor = "#FFFFFF";

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.EqCategory = EquipmentCategory::WEAPON;
  ic->Data.ItemType_ = ItemType::WEAPON;

  ic->Data.Prefix = overridePrefix ? ItemPrefix::UNCURSED : RollItemPrefix();
  ic->Data.IsIdentified = overridePrefix ? true : false;

  int avgDamage = 0;
  int baseDurability = 0;

  switch (type)
  {
    case WeaponType::DAGGER:
    {
      int diceRolls = 1;
      int diceSides = 4;

      avgDamage = CalculateAverageDamage(diceRolls, diceSides);

      baseDurability = 15;

      ic->Data.Damage.CurrentValue = diceRolls;
      ic->Data.Damage.OriginalValue = diceSides;

      ic->Data.StatBonuses[StatsEnum::SKL] = 1;
      ic->Data.StatBonuses[StatsEnum::SPD] = 3;
    }
    break;

    case WeaponType::SHORT_SWORD:
    {
      int diceRolls = 1;
      int diceSides = 6;

      avgDamage = CalculateAverageDamage(diceRolls, diceSides);

      baseDurability = 20;

      ic->Data.Damage.CurrentValue = diceRolls;
      ic->Data.Damage.OriginalValue = diceSides;

      ic->Data.StatBonuses[StatsEnum::STR] = 1;
      ic->Data.StatBonuses[StatsEnum::SPD] = 1;
    }
    break;

    case WeaponType::ARMING_SWORD:
    {
      int diceRolls = 1;
      int diceSides = 8;

      avgDamage = CalculateAverageDamage(diceRolls, diceSides);

      baseDurability = 25;

      ic->Data.Damage.CurrentValue = diceRolls;
      ic->Data.Damage.OriginalValue = diceSides;

      ic->Data.StatBonuses[StatsEnum::STR] = 2;
      ic->Data.StatBonuses[StatsEnum::DEF] = 1;      
    }
    break;

    case WeaponType::LONG_SWORD:
    {
      int diceRolls = 2;
      int diceSides = 6;

      avgDamage = CalculateAverageDamage(diceRolls, diceSides);

      baseDurability = 35;

      ic->Data.Damage.CurrentValue = diceRolls;
      ic->Data.Damage.OriginalValue = diceSides;

      ic->Data.StatBonuses[StatsEnum::STR] = 2;
      ic->Data.StatBonuses[StatsEnum::DEF] = 1;
      ic->Data.StatBonuses[StatsEnum::SPD] = -1;
    }
    break;

    case WeaponType::GREAT_SWORD:
    {
      int diceRolls = 3;
      int diceSides = 10;

      avgDamage = CalculateAverageDamage(diceRolls, diceSides);

      baseDurability = 50;

      ic->Data.Damage.CurrentValue = diceRolls;
      ic->Data.Damage.OriginalValue = diceSides;

      ic->Data.StatBonuses[StatsEnum::STR] = 4;
      ic->Data.StatBonuses[StatsEnum::SKL] = -2;
      ic->Data.StatBonuses[StatsEnum::SPD] = -4;
    }
    break;

    case WeaponType::STAFF:
    {
      int diceRolls = 1;
      int diceSides = 6;

      avgDamage = CalculateAverageDamage(diceRolls, diceSides);

      baseDurability = 10;

      ic->Data.Damage.CurrentValue = diceRolls;
      ic->Data.Damage.OriginalValue = diceSides;

      ic->Data.StatBonuses[StatsEnum::STR] = 1;
      ic->Data.StatBonuses[StatsEnum::DEF] = 2;
      ic->Data.StatBonuses[StatsEnum::SPD] = -1;
    }
    break;
  }

  int durability = baseDurability + (baseDurability / 10) + dungeonLevel;
  ic->Data.Durability.Set(durability);

  ic->Data.UnidentifiedName = "?" + go->ObjectName + "?";
  ic->Data.IdentifiedName = go->ObjectName;

  auto str = Util::StringFormat("You think it'll do %d damage on average.", avgDamage);
  ic->Data.UnidentifiedDescription = { str, "You can't tell anything else." };

  // *** !!!
  // Identified description for weapon is
  // returned via private helper method in ItemComponent
  // *** !!!

  AdjustWeaponBonuses(ic->Data);
  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateContainer(std::string name, chtype image, int x, int y)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->ObjectName = name;
  go->PosX = x;
  go->PosY = y;
  go->Image = image;
  go->FgColor = "#FFFFFF";
  go->BgColor = GlobalConstants::RoomFloorColor;
  go->Blocking = true;

  ContainerComponent* cc = go->AddComponent<ContainerComponent>();

  cc->MaxCapacity = GlobalConstants::InventoryMaxSize;

  go->InteractionCallback = std::bind(&ContainerComponent::Interact, cc);

  return go;
}

// ************************** PRIVATE METHODS ************************** //

void GameObjectsFactory::SetItemName(GameObject* go, ItemData& itemData)
{
  switch (itemData.Prefix)
  {
    case ItemPrefix::BLESSED:
      itemData.IdentifiedName.insert(0, "Blessed ");
      itemData.IdentifiedDescription.push_back("This one is blessed and will perform better.");
      break;

    case ItemPrefix::UNCURSED:
      itemData.IdentifiedName.insert(0, "Uncursed ");
      break;

    case ItemPrefix::CURSED:
      itemData.IdentifiedName.insert(0, "Cursed ");
      itemData.IdentifiedDescription.push_back("This one is cursed and should be avoided.");
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
      break;

    case ItemType::EXP_POTION:
      itemData.IdentifiedName.append(" of Enlightenment");
      break;

    case ItemType::STR_POTION:
      itemData.IdentifiedName.append(" of Strength");
      break;

    case ItemType::DEF_POTION:
      itemData.IdentifiedName.append(" of Defence");
      break;

    case ItemType::MAG_POTION:
      itemData.IdentifiedName.append(" of Magic");
      break;

    case ItemType::RES_POTION:
      itemData.IdentifiedName.append(" of Resistance");
      break;

    case ItemType::SKL_POTION:
      itemData.IdentifiedName.append(" of Skill");
      break;

    case ItemType::SPD_POTION:
      itemData.IdentifiedName.append(" of Speed");
      break;
  }
}

ItemPrefix GameObjectsFactory::RollItemPrefix()
{
  std::map<ItemPrefix, int> weights =
  {
    { ItemPrefix::UNCURSED, 5 },
    { ItemPrefix::CURSED, 5 },
    { ItemPrefix::BLESSED, 1 }
  };

  auto res = Util::WeightedRandom(weights);
  ItemPrefix prefix = res.first;

  return prefix;
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
  for (int i = 0; i < rings.size(); i++)
  {
    if (rings[i] == item)
    {
      UnequipRing(rings[i], i);
      return true;
    }
  }

  // Second, if it's different item, try to find empty slot for it
  for (int i = 0; i < rings.size(); i++)
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

  _playerRef->SetStatsModifiers(item->Data);

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

  _playerRef->UnsetStatsModifiers(item->Data);

  std::string objName = item->Data.IsIdentified ? item->OwnerGameObject->ObjectName : item->Data.UnidentifiedName;

  auto message = Util::StringFormat("You %s %s", verb.data(), objName.data());
  Printer::Instance().AddMessage(message);
}

void GameObjectsFactory::EquipRing(ItemComponent* ring, int index)
{
  ring->Data.IsEquipped = true;
  _playerRef->EquipmentByCategory[ring->Data.EqCategory][index] = ring;

  std::string objName = ring->Data.IsIdentified ? ring->OwnerGameObject->ObjectName : ring->Data.UnidentifiedName;

  auto str = Util::StringFormat("You put on %s", objName.data());
  Printer::Instance().AddMessage(str);
}

void GameObjectsFactory::UnequipRing(ItemComponent* ring, int index)
{
  ring->Data.IsEquipped = false;
  _playerRef->EquipmentByCategory[ring->Data.EqCategory][index] = nullptr;

  std::string objName = ring->Data.IsIdentified ? ring->OwnerGameObject->ObjectName : ring->Data.UnidentifiedName;

  auto str = Util::StringFormat("You take off %s", objName.data());
  Printer::Instance().AddMessage(str);
}

void GameObjectsFactory::HealingPotionUseHandler(ItemComponent* item)
{
  int amount = 0;

  int statMax = _playerRef->Attrs.HP.OriginalValue;
  int& statCur = _playerRef->Attrs.HP.CurrentValue;

  if (item->Data.Prefix == ItemPrefix::BLESSED)
  {
    amount = statMax;
    Printer::Instance().AddMessage("Your wounds are healed completely!");
  }
  else if (item->Data.Prefix == ItemPrefix::UNCURSED)
  {
    amount = statMax * 0.3f;
    Printer::Instance().AddMessage("You feel better");
  }
  else if (item->Data.Prefix == ItemPrefix::CURSED)
  {
    amount = -statMax * 0.3f;
    Printer::Instance().AddMessage("You are damaged by a cursed potion!");
  }

  statCur += amount;
  statCur = Util::Clamp(statCur, 0, statMax);

  Application::Instance().ChangeState(GameStates::MAIN_STATE);
}

void GameObjectsFactory::ManaPotionUseHandler(ItemComponent* item)
{
  int amount = 0;

  int statMax = _playerRef->Attrs.MP.OriginalValue;
  int& statCur = _playerRef->Attrs.MP.CurrentValue;

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

  statCur += amount;
  statCur = Util::Clamp(statCur, 0, statMax);

  Application::Instance().ChangeState(GameStates::MAIN_STATE);
}

void GameObjectsFactory::HungerPotionUseHandler(ItemComponent* item)
{
  int amount = 0;

  int statMax = _playerRef->Attrs.Hunger;
  int& statCur = _playerRef->Attrs.Hunger;

  if (item->Data.Prefix == ItemPrefix::BLESSED)
  {
    amount = statMax;
    Printer::Instance().AddMessage("You feel satiated!");
  }
  else if (item->Data.Prefix == ItemPrefix::UNCURSED)
  {
    amount = statMax * 0.3f;
    Printer::Instance().AddMessage("Your hunger has abated somewhat");
  }
  else if (item->Data.Prefix == ItemPrefix::CURSED)
  {
    amount = -statMax * 0.3f;
    Printer::Instance().AddMessage("Your feel peckish");
  }

  statCur += amount;
  statCur = Util::Clamp(statCur, 0, statMax);

  Application::Instance().ChangeState(GameStates::MAIN_STATE);
}

void GameObjectsFactory::ExpPotionUseHandler(ItemComponent* item)
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
}

void GameObjectsFactory::StatPotionUseHandler(ItemComponent* item)
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

  int newValue = playerStats.at(itemType).OriginalValue + valueToAdd;

  if (newValue < 0)
  {
    newValue = 0;
  }

  playerStats.at(itemType).Set(newValue);
}

void GameObjectsFactory::AdjustWeaponBonuses(ItemData& itemData)
{
  for (auto& kvp : itemData.StatBonuses)
  {
    if (kvp.second != 0)
    {
      switch (itemData.Prefix)
      {
        case ItemPrefix::CURSED:
          kvp.second--;
          itemData.Durability.Set(itemData.Durability.OriginalValue / 2);
          itemData.Damage.CurrentValue--;
          itemData.Damage.OriginalValue--;
          itemData.Damage.CurrentValue = Util::Clamp(itemData.Damage.CurrentValue, 1, itemData.Damage.CurrentValue);
          itemData.Damage.OriginalValue = Util::Clamp(itemData.Damage.OriginalValue, 1, itemData.Damage.OriginalValue);
          break;

        case ItemPrefix::BLESSED:
          kvp.second++;
          itemData.Damage.CurrentValue *= 2;
          itemData.Damage.OriginalValue *= 2;
          break;
      }      
    }
  }
}

size_t GameObjectsFactory::CalculateHash(ItemComponent* item)
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

void GameObjectsFactory::GenerateLootIfPossible(int posX, int posY, MonsterType monsterType)
{
  if (GlobalConstants::LootTable.count(monsterType) == 1)
  {
    auto weights = GlobalConstants::LootTable.at(monsterType);
    auto kvp = Util::WeightedRandom(weights);

    GenerateLoot(posX, posY, kvp, monsterType);
  }
}

void GameObjectsFactory::GenerateLoot(int posX, int posY, std::pair<ItemType, int> kvp, MonsterType monsterType)
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
      if (go != nullptr)
      {
        Map::Instance().InsertGameObject(go);
      }
    }
    break;
  }
}
