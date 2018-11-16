#include "game-objects-factory.h"

#include "constants.h"
#include "rng.h"
#include "item-component.h"
#include "shrine-component.h"
#include "ai-monster-basic.h"
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
  }

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

  Component* c = go->AddComponent<ItemComponent>();
  ItemComponent* ic = static_cast<ItemComponent*>(c);

  ic->Data.IdentifiedDescription = { "You can buy things with these." };

  int pl = _playerRef->Attrs.Lvl.CurrentValue;

  int money = (amount == 0) ? RNG::Instance().RandomRange(1 * pl, 10 * pl) : amount;
  ic->Data.Cost = money;
  ic->Data.Amount = money;
  ic->Data.IsStackable = true;
  ic->Data.TypeOfItem = ItemType::COINS;
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

  auto c = go->AddComponent<ShrineComponent>();
  ShrineComponent* sc = static_cast<ShrineComponent*>(c);

  sc->Type = type;
  sc->Counter = timeout;
  sc->Timeout = timeout;

  go->ObjectName = GlobalConstants::ShrineNameByType.at(type);
  go->FogOfWarName = "?Shrine?";
  go->InteractionCallback = std::bind(&ShrineComponent::Interact, sc);

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

  auto c = go->AddComponent<AIMonsterBasic>();
  AIMonsterBasic* ai = static_cast<AIMonsterBasic*>(c);

  ai->AgroRadius = 8;

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

    go->Attrs.Str.Set(randomStr);
    go->Attrs.Def.Set(randomDef);
    go->Attrs.HP.Set(randomHp);
    go->Attrs.Spd.Set(randomSpd);
    go->Attrs.Skl.Set(randomSkl);
  }
  else
  {
    // For debugging purposes
    go->ObjectName = "Battle Rat";

    go->Attrs.Str.Set(2 * _playerRef->Attrs.Lvl.CurrentValue);
    go->Attrs.Def.Set(1 * _playerRef->Attrs.Lvl.CurrentValue);
    go->Attrs.Spd.Set(10);

    go->Attrs.HP.Set(10);
  }

  return go;
}

GameObject* GameObjectsFactory::CreateRemains(GameObject* from)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel, from->PosX, from->PosY, '%', from->FgColor);

  auto td = go->AddComponent<TimerDestroyerComponent>();
  ((TimerDestroyerComponent*)td)->Time = 200; //from->Attrs.HP.OriginalValue * 2;

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
    switch (item->Data.TypeOfItem)
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
GameObject* GameObjectsFactory::CreateFood(FoodType type, ItemPrefix prefixOverride)
{
  std::string name;
  int addsHunger = 0;

  if (GlobalConstants::FoodItemHungerByName.count(type) == 1)
  {
    name = GlobalConstants::FoodItemHungerByName.at(type).first;
    addsHunger = GlobalConstants::FoodItemHungerByName.at(type).second;
  }
  else
  {
    int index = RNG::Instance().RandomRange(0, GlobalConstants::FoodItemHungerByName.size());
    name = GlobalConstants::FoodItemHungerByName.at((FoodType)index).first;
    addsHunger = GlobalConstants::FoodItemHungerByName.at((FoodType)index).second;
  }

  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->FgColor = "#FFFFFF";
  go->BgColor = "#000000";
  go->Image = '%';
  go->ObjectName = name;

  Component* c = go->AddComponent<ItemComponent>();
  ItemComponent* ic = static_cast<ItemComponent*>(c);

  ic->Data.TypeOfItem = ItemType::FOOD;
  ic->Data.Prefix = prefixOverride;
  ic->Data.Amount = 1;

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

  Component* c = go->AddComponent<ItemComponent>();
  ItemComponent* ic = static_cast<ItemComponent*>(c);

  ic->Data.TypeOfItem = ItemType::HEALING_POTION;
  ic->Data.Prefix = prefixOverride;
  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;

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

  Component* c = go->AddComponent<ItemComponent>();
  ItemComponent* ic = static_cast<ItemComponent*>(c);

  ic->Data.TypeOfItem = ItemType::MANA_POTION;
  ic->Data.Prefix = prefixOverride;
  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;

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

  Component* c = go->AddComponent<ItemComponent>();
  ItemComponent* ic = static_cast<ItemComponent*>(c);

  ic->Data.TypeOfItem = ItemType::HUNGER_POTION;
  ic->Data.Prefix = prefixOverride;
  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;

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

  Component* c = go->AddComponent<ItemComponent>();
  ItemComponent* ic = static_cast<ItemComponent*>(c);

  ic->Data.TypeOfItem = ItemType::EXP_POTION;
  ic->Data.Prefix = prefixOverride;
  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;

  ic->Data.IdentifiedDescription = { "They say drinking this will bring you to the next level.", "Whatever that means..." };
  ic->Data.IdentifiedName = "Clear Potion";

  ic->Data.UseCallback = std::bind(&GameObjectsFactory::ExpPotionUseHandler, this, ic);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateHash(ic);

  return go;
}

GameObject* GameObjectsFactory::CreateRandomPotion()
{
  GameObject* go = nullptr;

  int roll = RNG::Instance().RandomRange(0, 100);

  // TODO: more potions

  if (roll < 3)
  {
    // random stat potion
    // FIXME: fallback to healing potion for now
    go = CreateHealingPotion();
  }
  else if (roll < 6)
  {
    go = CreateExpPotion();
  }
  else
  {
    int which = RNG::Instance().RandomRange(0, 3);
    if (which == 0)
    {
      go = CreateHealingPotion();
    }
    else if (which == 1)
    {
      go = CreateManaPotion();
    }
    else
    {
      go = CreateHungerPotion();
    }
  }

  Component* c = go->GetComponent<ItemComponent>();
  ItemComponent* ic = static_cast<ItemComponent*>(c);

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

void GameObjectsFactory::CreateStairs(MapLevelBase* levelWhereCreate, int x, int y, chtype image, MapType leadsTo)
{
  auto tile = levelWhereCreate->MapArray[x][y].get();

  auto c = tile->AddComponent<StairsComponent>();
  StairsComponent* stairs = static_cast<StairsComponent*>(c);
  stairs->LeadsTo = leadsTo;

  //tile->Blocking = false;
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
    Printer::Instance().AddMessage("Disgusting!");

    _playerRef->Attrs.Hunger += item->Data.Cost;
  }
  else if (item->Data.Prefix == ItemPrefix::BLESSED)
  {
    Printer::Instance().AddMessage("It's delicious!");

    _playerRef->Attrs.Hunger -= item->Data.Cost * 2;
  }
  else
  {
    Printer::Instance().AddMessage("It tasted OK");

    _playerRef->Attrs.Hunger -= item->Data.Cost;
  }

  _playerRef->Attrs.Hunger = Util::Clamp(_playerRef->Attrs.Hunger, 0, _playerRef->Attrs.HungerRate.CurrentValue);

  Application::Instance().ChangeState(Application::GameStates::MAIN_STATE);
}

GameObject* GameObjectsFactory::CreateNote(std::string objName, std::vector<std::string> text)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->FgColor = "#000000";
  go->BgColor = "#FFFFFF";
  go->Image = '?';
  go->ObjectName = objName;

  Component* c = go->AddComponent<ItemComponent>();
  ItemComponent* ic = static_cast<ItemComponent*>(c);

  ic->Data.TypeOfItem = ItemType::DUMMY;
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

  int dungeonLevel = Map::Instance().CurrentLevel->DungeonLevel;
  if (dungeonLevel == 0)
  {
    dungeonLevel = 1;
  }

  go->ObjectName = GlobalConstants::WeaponNameByType.at(type);
  go->Image = ')';
  go->FgColor = "#FFFFFF";

  auto c = go->AddComponent<ItemComponent>();
  ItemComponent* ic = static_cast<ItemComponent*>(c);

  ic->Data.EqCategory = EquipmentCategory::WEAPON;
  ic->Data.TypeOfItem = ItemType::WEAPON;

  ic->Data.Prefix = overridePrefix ? ItemPrefix::UNCURSED : RollItemPrefix();
  ic->Data.IsIdentified = overridePrefix ? true : false;

  int avgDamage = 0;
  int baseDurability = 0;

  switch (type)
  {
    case WeaponType::DAGGER:
    {
      int diceRolls = 1 * dungeonLevel;
      int diceSides = 4 * dungeonLevel;

      avgDamage = ((diceRolls * diceSides) - diceRolls) / 2;

      baseDurability = 15;

      ic->Data.Damage.CurrentValue = diceRolls;
      ic->Data.Damage.OriginalValue = diceSides;

      ic->Data.StatBonuses[StatsEnum::SKL] = 1;
      ic->Data.StatBonuses[StatsEnum::SPD] = 3;
    }
    break;

    case WeaponType::SHORT_SWORD:
    {
      int diceRolls = 1 * dungeonLevel;
      int diceSides = 6 * dungeonLevel;

      avgDamage = ((diceRolls * diceSides) - diceRolls) / 2;

      baseDurability = 20;

      ic->Data.Damage.CurrentValue = diceRolls;
      ic->Data.Damage.OriginalValue = diceSides;

      ic->Data.StatBonuses[StatsEnum::STR] = 1;
      ic->Data.StatBonuses[StatsEnum::SPD] = 1;
    }
    break;

    case WeaponType::ARMING_SWORD:
    {
      int diceRolls = 2 * dungeonLevel;
      int diceSides = 6 * dungeonLevel;

      avgDamage = ((diceRolls * diceSides) - diceRolls) / 2;

      baseDurability = 25;

      ic->Data.Damage.CurrentValue = diceRolls;
      ic->Data.Damage.OriginalValue = diceSides;

      ic->Data.StatBonuses[StatsEnum::STR] = 2;
      ic->Data.StatBonuses[StatsEnum::DEF] = 1;      
    }
    break;

    case WeaponType::STAFF:
    {
      int diceRolls = 1 * dungeonLevel;
      int diceSides = 6 * dungeonLevel;

      avgDamage = ((diceRolls * diceSides) - diceRolls) / 2;

      baseDurability = 10;

      ic->Data.Damage.CurrentValue = diceRolls;
      ic->Data.Damage.OriginalValue = diceSides;

      ic->Data.StatBonuses[StatsEnum::STR] = 1;
      ic->Data.StatBonuses[StatsEnum::DEF] = 2;
      ic->Data.StatBonuses[StatsEnum::SPD] = 1;
    }
    break;
  }

  int durability = baseDurability + (baseDurability / 10) + dungeonLevel;
  ic->Data.Durability.Set(durability);

  ic->Data.UnidentifiedName = "?" + go->ObjectName + "?";
  ic->Data.IdentifiedName = go->ObjectName;

  auto str = Util::StringFormat("It seems to inflict %d damage on average", avgDamage);
  ic->Data.UnidentifiedDescription = { str, "You can't tell anything else." };

  // *** !!!
  // Identified description for weapon is
  // returned via private helper method in ItemComponent
  // *** !!!

  AdjustItemBonuses(ic->Data);
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

  auto c = go->AddComponent<ContainerComponent>();
  ContainerComponent* cc = static_cast<ContainerComponent*>(c);

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

  switch (itemData.TypeOfItem)
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
  }
}

ItemPrefix GameObjectsFactory::RollItemPrefix()
{
  int roll = RNG::Instance().RandomRange(0, 101);

  if (roll >= 0 && roll <= 10)
  {
    return ItemPrefix::BLESSED;
  }
  else if (roll > 10 && roll <= 80)
  {
    return ItemPrefix::UNCURSED;
  }
  else if (roll > 80)
  {
    return ItemPrefix::CURSED;
  }

  return ItemPrefix::UNCURSED;
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

  SetStatsModifiers(item->Data);

  auto message = Util::StringFormat("You %s %s", verb.data(), item->OwnerGameObject->ObjectName.data());
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

  UnsetStatsModifiers(item->Data);

  auto message = Util::StringFormat("You %s %s", verb.data(), item->OwnerGameObject->ObjectName.data());
  Printer::Instance().AddMessage(message);
}

void GameObjectsFactory::EquipRing(ItemComponent* ring, int index)
{
  ring->Data.IsEquipped = true;
  _playerRef->EquipmentByCategory[ring->Data.EqCategory][index] = ring;

  auto str = Util::StringFormat("You put on %s", ring->OwnerGameObject->ObjectName.data());
  Printer::Instance().AddMessage(str);
}

void GameObjectsFactory::UnequipRing(ItemComponent* ring, int index)
{
  ring->Data.IsEquipped = false;
  _playerRef->EquipmentByCategory[ring->Data.EqCategory][index] = nullptr;

  auto str = Util::StringFormat("You take off %s", ring->OwnerGameObject->ObjectName.data());
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

  Application::Instance().ChangeState(Application::GameStates::MAIN_STATE);
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

  Application::Instance().ChangeState(Application::GameStates::MAIN_STATE);
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

  Application::Instance().ChangeState(Application::GameStates::MAIN_STATE);
}

void GameObjectsFactory::ExpPotionUseHandler(ItemComponent* item)
{
  int amount = 0;

  int statMax = 100;
  int& statCur = _playerRef->Attrs.Exp.CurrentValue;

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

  Application::Instance().ChangeState(Application::GameStates::MAIN_STATE);

  _playerRef->AwardExperience(amount);
}

void GameObjectsFactory::AdjustItemBonuses(ItemData& itemData)
{
  for (auto& kvp : itemData.StatBonuses)
  {
    if (kvp.second != 0)
    {
      switch (itemData.Prefix)
      {
        case ItemPrefix::CURSED:
          kvp.second--;
          itemData.Damage.CurrentValue--;
          itemData.Damage.OriginalValue--;
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

void GameObjectsFactory::SetStatsModifiers(ItemData& itemData)
{
  _playerRef->Attrs.Str.Modifier += itemData.StatBonuses[StatsEnum::STR];
  _playerRef->Attrs.Def.Modifier += itemData.StatBonuses[StatsEnum::DEF];
  _playerRef->Attrs.Mag.Modifier += itemData.StatBonuses[StatsEnum::MAG];
  _playerRef->Attrs.Res.Modifier += itemData.StatBonuses[StatsEnum::RES];
  _playerRef->Attrs.Skl.Modifier += itemData.StatBonuses[StatsEnum::SKL];
  _playerRef->Attrs.Spd.Modifier += itemData.StatBonuses[StatsEnum::SPD];
}

void GameObjectsFactory::UnsetStatsModifiers(ItemData& itemData)
{
  _playerRef->Attrs.Str.Modifier -= itemData.StatBonuses[StatsEnum::STR];
  _playerRef->Attrs.Def.Modifier -= itemData.StatBonuses[StatsEnum::DEF];
  _playerRef->Attrs.Mag.Modifier -= itemData.StatBonuses[StatsEnum::MAG];
  _playerRef->Attrs.Res.Modifier -= itemData.StatBonuses[StatsEnum::RES];
  _playerRef->Attrs.Skl.Modifier -= itemData.StatBonuses[StatsEnum::SKL];
  _playerRef->Attrs.Spd.Modifier -= itemData.StatBonuses[StatsEnum::SPD];
}

size_t GameObjectsFactory::CalculateHash(ItemComponent* item)
{
  auto strToHash = std::to_string((int)item->Data.Prefix) + item->OwnerGameObject->ObjectName;
  std::hash<std::string> hasher;

  return hasher(strToHash);
}
