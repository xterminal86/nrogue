#include "items-factory.h"

#include "application.h"
#include "map.h"
#include "game-objects-factory.h"
#include "printer.h"
#include "logger.h"

#include "item-component.h"
#include "item-use-handlers.h"

using namespace std::placeholders;

void ItemsFactory::InitSpecific()
{
  InitPotionColors();
  InitScrolls();

  _playerRef = &Application::Instance().PlayerInstance;
}

void ItemsFactory::InitPotionColors()
{
  using PotionsMap = std::map<std::string, std::vector<std::string>>;

  _gamePotionsMap.clear();

  // TODO: potion of cure weakness and restore ability,
  // allow repeatable colors

  std::vector<ItemType> potionTypes =
  {
    ItemType::HEALING_POTION,
    ItemType::MANA_POTION,
    ItemType::HUNGER_POTION,
    ItemType::NP_POTION,
    ItemType::STR_POTION,
    ItemType::DEF_POTION,
    ItemType::MAG_POTION,
    ItemType::RES_POTION,
    ItemType::SKL_POTION,
    ItemType::SPD_POTION,
    ItemType::EXP_POTION
  };

  PotionsMap potions = Colors::PotionColorsByName;

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

void ItemsFactory::InitScrolls()
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

    // DebugLog("%s = %s\n", GlobalConstants::SpellNameByType.at(si.SpellType_).data(), si.ScrollName.data());

    scrollNames.erase(scrollNames.begin() + scrollNameIndex);
    validSpells.erase(validSpells.begin() + spellIndex);
  }
}

void ItemsFactory::SetPotionImage(GameObject* go)
{
#ifdef USE_SDL
    go->Image = GlobalConstants::CP437IndexByType[NameCP437::EXCL_MARK_INV];
#else
    go->Image = '!';
#endif
}

GameObject* ItemsFactory::CreateMoney(int amount)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->ObjectName = Strings::MoneyName;
  go->Image = '$';
  go->FgColor = Colors::CoinsColor;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.IdentifiedDescription = { "You can buy things with these." };

  int scale = Map::Instance().CurrentLevel->DungeonLevel;

  int money = (amount == 0) ? RNG::Instance().RandomRange(1, 11) * scale : amount;

  ic->Data.Cost           = money;
  ic->Data.Amount         = money;
  ic->Data.IsStackable    = true;
  ic->Data.ItemType_      = ItemType::COINS;
  ic->Data.IsIdentified   = true;
  ic->Data.IdentifiedName = Strings::MoneyName;

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  go->StackObjectId = go->ObjectId();

  return go;
}

GameObject* ItemsFactory::CreatePotion(ItemType type, ItemPrefix prefixOverride)
{
  GameObject* go = nullptr;

  if (GlobalConstants::PotionsWeightTable.count(type) != 0)
  {
    switch (type)
    {
      case ItemType::HEALING_POTION:
        go = CreateHealingPotion(prefixOverride);
        break;

      case ItemType::MANA_POTION:
        go = CreateManaPotion(prefixOverride);
        break;

      case ItemType::NP_POTION:
        go = CreateNeutralizePoisonPotion(prefixOverride);
        break;

      case ItemType::HUNGER_POTION:
        go = CreateHungerPotion(prefixOverride);
        break;

      case ItemType::STR_POTION:
      case ItemType::DEF_POTION:
      case ItemType::MAG_POTION:
      case ItemType::RES_POTION:
      case ItemType::SKL_POTION:
      case ItemType::SPD_POTION:
        go = CreateStatPotion(GlobalConstants::StatNameByPotionType.at(type), prefixOverride);
        break;

      case ItemType::EXP_POTION:
        go = CreateExpPotion(prefixOverride);
        break;

      default:
        DebugLog("CreatePotion(): potion type %i is not handled!", type);
        break;
    }
  }
  else
  {
    #ifdef DEBUG_BUILD
    DebugLog("Potion type %i not found!", (int)type);
    #endif
  }

  return go;
}

GameObject* ItemsFactory::CreateHealingPotion(ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  ItemType t = ItemType::HEALING_POTION;

  std::string fgColor = _gamePotionsMap[t].FgBgColor.first;
  std::string bgColor = _gamePotionsMap[t].FgBgColor.second;
  std::string name = _gamePotionsMap[t].PotionName;

  go->FgColor = fgColor;
  go->BgColor = bgColor;

  SetPotionImage(go);

  go->ObjectName = name;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_ = t;
  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;
  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;
  ic->Data.Cost = 25;

  go->StackObjectId = go->ObjectId();

  ic->Data.IdentifiedDescription = { "Restores some of your health." };
  ic->Data.IdentifiedName = name;
  ic->Data.UnidentifiedName = "?" + name + "?";

  ic->Data.UseCallback = std::bind(&ItemUseHandlers::HealingPotionUseHandler,
                                   _1,
                                   _2);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* ItemsFactory::CreateNeutralizePoisonPotion(ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  ItemType t = ItemType::NP_POTION;

  std::string fgColor = _gamePotionsMap[t].FgBgColor.first;
  std::string bgColor = _gamePotionsMap[t].FgBgColor.second;
  std::string name    = _gamePotionsMap[t].PotionName;

  go->FgColor = fgColor;
  go->BgColor = bgColor;

  SetPotionImage(go);

  go->ObjectName = name;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_ = t;
  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;
  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;
  ic->Data.Cost = 5;

  go->StackObjectId = go->ObjectId();

  ic->Data.IdentifiedDescription = { "Removes poison from the body" };
  ic->Data.IdentifiedName        = name;
  ic->Data.UnidentifiedName      = "?" + name + "?";

  ic->Data.UseCallback = std::bind(&ItemUseHandlers::NeutralizePoisonPotionUseHandler,
                                   _1,
                                   _2);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* ItemsFactory::CreateManaPotion(ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  ItemType t = ItemType::MANA_POTION;

  std::string fgColor = _gamePotionsMap[t].FgBgColor.first;
  std::string bgColor = _gamePotionsMap[t].FgBgColor.second;
  std::string name = _gamePotionsMap[t].PotionName;

  go->FgColor = fgColor;
  go->BgColor = bgColor;

  SetPotionImage(go);

  go->ObjectName = name;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_ = t;
  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;
  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;
  ic->Data.Cost = 25;

  go->StackObjectId = go->ObjectId();

  ic->Data.IdentifiedDescription = { "Helps you regain spiritual powers." };
  ic->Data.IdentifiedName = name;
  ic->Data.UnidentifiedName = "?" + name + "?";

  ic->Data.UseCallback = std::bind(&ItemUseHandlers::ManaPotionUseHandler,
                                   _1,
                                   _2);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* ItemsFactory::CreateHungerPotion(ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  ItemType t = ItemType::HUNGER_POTION;

  std::string fgColor = _gamePotionsMap[t].FgBgColor.first;
  std::string bgColor = _gamePotionsMap[t].FgBgColor.second;
  std::string name = _gamePotionsMap[t].PotionName;

  go->FgColor = fgColor;
  go->BgColor = bgColor;

  SetPotionImage(go);

  go->ObjectName = name;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_ = t;
  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;
  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;
  ic->Data.Cost = 15;

  go->StackObjectId = go->ObjectId();

  ic->Data.IdentifiedDescription = { "Liquid food. Drink it if there's nothing else to eat." };
  ic->Data.IdentifiedName = name;
  ic->Data.UnidentifiedName = "?" + name + "?";

  ic->Data.UseCallback = std::bind(&ItemUseHandlers::HungerPotionUseHandler,
                                   _1,
                                   _2);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* ItemsFactory::CreateExpPotion(ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  ItemType t = ItemType::EXP_POTION;

  std::string fgColor = _gamePotionsMap[t].FgBgColor.first;
  std::string bgColor = _gamePotionsMap[t].FgBgColor.second;
  std::string name = _gamePotionsMap[t].PotionName;

  go->FgColor = fgColor;
  go->BgColor = bgColor;

  SetPotionImage(go);

  go->ObjectName = name;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_ = t;
  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;
  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;
  ic->Data.Cost = 250;

  go->StackObjectId = go->ObjectId();

  ic->Data.IdentifiedDescription =
  {
    "They say drinking this will bring you to the next level.",
    "Whatever it means..."
  };

  ic->Data.IdentifiedName = name;
  ic->Data.UnidentifiedName = "?" + name + "?";

  ic->Data.UseCallback = std::bind(&ItemUseHandlers::ExpPotionUseHandler,
                                   _1,
                                   _2);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* ItemsFactory::CreateStatPotion(const std::string& statName, ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  ItemType t = GlobalConstants::PotionTypeByStatName.at(statName);

  std::string fgColor = _gamePotionsMap[t].FgBgColor.first;
  std::string bgColor = _gamePotionsMap[t].FgBgColor.second;
  std::string name = _gamePotionsMap[t].PotionName;

  go->FgColor = fgColor;
  go->BgColor = bgColor;

  SetPotionImage(go);

  go->ObjectName = name;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_ = t;
  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;
  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;
  ic->Data.Cost = 500;

  go->StackObjectId = go->ObjectId();

  auto str = Util::StringFormat("This will affect your %s", statName.data());
  ic->Data.IdentifiedDescription = { str };

  ic->Data.IdentifiedName = name;
  ic->Data.UnidentifiedName = "?" + name + "?";

  ic->Data.UseCallback = std::bind(&ItemUseHandlers::StatPotionUseHandler,
                                   _1,
                                   _2);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* ItemsFactory::CreateRandomPotion()
{
  GameObject* go = nullptr;

  auto weights = Util::WeightedRandom(GlobalConstants::PotionsWeightTable);
  go = GameObjectsFactory::Instance().CreateGameObject(0, 0, weights.first);

  ItemComponent* ic = go->GetComponent<ItemComponent>();

  ic->Data.IsIdentified = false;
  ic->Data.UnidentifiedDescription = { "You don't know what will happen if you drink it." };

  return go;
}

GameObject* ItemsFactory::CreateFood(int x, int y, FoodType type, ItemPrefix prefixOverride, bool isIdentified)
{
  std::string name;
  int addsHunger = 0;

  name = GlobalConstants::FoodHungerPercentageByName.at(type).first;
  int percentage = GlobalConstants::FoodHungerPercentageByName.at(type).second;
  int hungerMax = _playerRef->Attrs.HungerRate.Get();
  int hungerToAdd = ((float)hungerMax * ((float)percentage / 100.0f));

  addsHunger = hungerToAdd;

  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->FgColor = Colors::WhiteColor;
  go->BgColor = Colors::BlackColor;
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
  go->StackObjectId = go->ObjectId();

  std::string unidName = (type == FoodType::RATIONS || type == FoodType::IRON_RATIONS) ? "Rations" : name;
  ic->Data.UnidentifiedName = "?" + unidName + "?";

  ic->Data.UnidentifiedDescription = { "Looks edible but eat at your own risk." };
  ic->Data.IdentifiedDescription = { "Looks edible." };
  ic->Data.IdentifiedName = name;

  ic->Data.UseCallback = std::bind(&ItemUseHandlers::FoodUseHandler,
                                   _1,
                                   _2);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* ItemsFactory::CreateNote(const std::string& objName, const std::vector<std::string>& text)
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

GameObject* ItemsFactory::CreateDummyItem(const std::string& objName,
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

GameObject* ItemsFactory::CreateScroll(int x, int y, SpellType type, ItemPrefix prefixOverride)
{
  SpellInfo* si = SpellsDatabase::Instance().GetSpellInfoFromDatabase(type);

  if (std::find(GlobalConstants::ScrollValidSpellTypes.begin(),
                GlobalConstants::ScrollValidSpellTypes.end(),
                type) == GlobalConstants::ScrollValidSpellTypes.end())
  {
    auto msg = Util::StringFormat("[WARNING] Trying to create a scroll with invalid spell (%s)!\n", si->SpellName.data());
    Printer::Instance().AddMessage(msg);
    Logger::Instance().Print(msg, true);
    DebugLog("%s\n", msg.data());
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

  ic->Data.SpellHeld.SpellType_ = type;

  // NOTE: scrolls cost = base * 2, spellbooks = base * 10
  //
  // Too expensive?
  ic->Data.Cost = si->SpellBaseCost; //si->SpellBaseCost * 2;

  ic->Data.UnidentifiedName = "\"" + _gameScrollsMap[type].ScrollName + "\"";
  ic->Data.UnidentifiedDescription = { "Who knows what will happen if you read these words aloud..." };

  ic->Data.IdentifiedDescription = { "TODO:" };
  ic->Data.IdentifiedName = "Scroll of " + si->SpellName;

  SetItemName(go, ic->Data);

  ic->Data.UseCallback = std::bind(&ItemUseHandlers::ScrollUseHandler,
                                   _1,
                                   _2);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* ItemsFactory::CreateRandomScroll(ItemPrefix prefix)
{
  int index = RNG::Instance().RandomRange(0, GlobalConstants::ScrollValidSpellTypes.size());
  SpellType type = GlobalConstants::ScrollValidSpellTypes.at(index);
  return CreateScroll(0, 0, type, prefix);
}

GameObject* ItemsFactory::CreateMeleeWeapon(int x, int y, WeaponType type, ItemPrefix prefix, ItemQuality quality, const std::vector<ItemBonusStruct>& bonuses)
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
  ic->Data.ItemType_  = ItemType::WEAPON;

  ic->Data.Prefix       = (prefix != ItemPrefix::RANDOM) ? prefix : RollItemPrefix();
  ic->Data.IsIdentified = (prefix != ItemPrefix::RANDOM) ? true : false;
  ic->Data.ItemQuality_ = (quality != ItemQuality::RANDOM) ? quality : RollItemQuality();

  int avgDamage = 0;
  int baseDurability = 0;

  int diceRolls = 0;
  int diceSides = 0;

  ic->Data.WeaponType_ = type;

  // NOTE: innate bonuses are hardcoded and not affected by cursed BUC status,
  // so we don't force-add them like in armor creation case.
  switch (type)
  {
    case WeaponType::DAGGER:
    {
      diceRolls = 1;
      diceSides = 3;

      baseDurability = 30 + 1 * (int)ic->Data.ItemQuality_;

      ic->Data.Damage.SetMin(diceRolls);
      ic->Data.Damage.SetMax(diceSides);

      AddBonusToItem(ic, { ItemBonusType::SKL, 1 });
      AddBonusToItem(ic, { ItemBonusType::SPD, 1 });
    }
    break;

    case WeaponType::SHORT_SWORD:
    {
      diceRolls = 1;
      diceSides = 6;

      baseDurability = 40 + 2 * (int)ic->Data.ItemQuality_;

      ic->Data.Damage.SetMin(diceRolls);
      ic->Data.Damage.SetMax(diceSides);

      AddBonusToItem(ic, { ItemBonusType::STR, 1 });
    }
    break;

    case WeaponType::ARMING_SWORD:
    {
      diceRolls = 1;
      diceSides = 8;

      baseDurability = 50 + 3 * (int)ic->Data.ItemQuality_;

      ic->Data.Damage.SetMin(diceRolls);
      ic->Data.Damage.SetMax(diceSides);

      AddBonusToItem(ic, { ItemBonusType::STR, 1 });
      AddBonusToItem(ic, { ItemBonusType::DEF, 1 });
    }
    break;

    case WeaponType::LONG_SWORD:
    {
      diceRolls = 2;
      diceSides = 6;

      baseDurability = 65 + 3 * (int)ic->Data.ItemQuality_;

      ic->Data.Damage.SetMin(diceRolls);
      ic->Data.Damage.SetMax(diceSides);

      AddBonusToItem(ic, { ItemBonusType::STR,  2 });
      AddBonusToItem(ic, { ItemBonusType::DEF,  1 });
      AddBonusToItem(ic, { ItemBonusType::SPD, -1 });
    }
    break;

    case WeaponType::GREAT_SWORD:
    {
      diceRolls = 3;
      diceSides = 10;

      baseDurability = 80 + 4 * (int)ic->Data.ItemQuality_;

      ic->Data.Damage.SetMin(diceRolls);
      ic->Data.Damage.SetMax(diceSides);

      AddBonusToItem(ic, { ItemBonusType::STR,  4 });
      AddBonusToItem(ic, { ItemBonusType::SKL, -2 });
      AddBonusToItem(ic, { ItemBonusType::SPD, -4 });
    }
    break;

    case WeaponType::STAFF:
    {
      diceRolls = 1;
      diceSides = 6;

      baseDurability = 30 + 2 * (int)ic->Data.ItemQuality_;

      ic->Data.Damage.SetMin(diceRolls);
      ic->Data.Damage.SetMax(diceSides);

      AddBonusToItem(ic, { ItemBonusType::DEF,  1 });
      AddBonusToItem(ic, { ItemBonusType::SPD, -1 });
    }
    break;

    case WeaponType::PICKAXE:
    {
      diceRolls = 1;
      diceSides = 6;

      int rndDur = RNG::Instance().RandomRange(20, 31);
      int spread = RNG::Instance().RandomRange(1, 5);

      baseDurability = rndDur + spread * (int)ic->Data.ItemQuality_;

      ic->Data.Damage.SetMin(diceRolls);
      ic->Data.Damage.SetMax(diceSides);
    }
    break;
  }

  for (auto& b : bonuses)
  {
    AddBonusToItem(ic, b);
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

GameObject* ItemsFactory::ChooseRandomMeleeWeapon(ItemPrefix prefixOverride)
{
  int index = RNG::Instance().RandomRange(0, GlobalConstants::WeaponNameByType.size());
  auto it = GlobalConstants::WeaponNameByType.begin();
  std::advance(it, index);
  return CreateMeleeWeapon(0, 0, it->first, prefixOverride);
}

GameObject* ItemsFactory::ChooseRandomRangedWeapon(ItemPrefix prefixOverride)
{
  int index = RNG::Instance().RandomRange(0, GlobalConstants::RangedWeaponNameByType.size());
  auto it = GlobalConstants::RangedWeaponNameByType.begin();
  std::advance(it, index);
  return CreateRangedWeapon(0, 0, it->first, prefixOverride);
}

GameObject* ItemsFactory::CreateRandomMeleeWeapon(WeaponType type, ItemPrefix prefixOverride)
{
  GameObject* go = nullptr;

  if (type != WeaponType::RANDOM)
  {
    go = CreateMeleeWeapon(0, 0, type, prefixOverride);
  }
  else
  {
    go = ChooseRandomMeleeWeapon(prefixOverride);
  }

  ItemComponent* ic = go->GetComponent<ItemComponent>();
  TryToAddBonusesToItem(ic);

  return go;
}

GameObject* ItemsFactory::CreateRandomRangedWeapon(RangedWeaponType type, ItemPrefix prefixOverride)
{
  GameObject* go = nullptr;

  if (type != RangedWeaponType::RANDOM)
  {
    go = CreateRangedWeapon(0, 0, type, prefixOverride);
  }
  else
  {
    go = ChooseRandomRangedWeapon(prefixOverride);
  }

  ItemComponent* ic = go->GetComponent<ItemComponent>();
  TryToAddBonusesToItem(ic);

  return go;
}

GameObject* ItemsFactory::CreateRandomWeapon(ItemPrefix prefixOverride)
{
  GameObject* go = nullptr;

  int isMelee = RNG::Instance().RandomRange(0, 2);
  if (isMelee == 0)
  {
    go = ChooseRandomMeleeWeapon(prefixOverride);
  }
  else
  {
    go = ChooseRandomRangedWeapon(prefixOverride);
  }

  ItemComponent* ic = go->GetComponent<ItemComponent>();
  TryToAddBonusesToItem(ic);

  return go;
}

GameObject* ItemsFactory::CreateGem(int x, int y, GemType type, int actualGemChance)
{
  GameObject* go = nullptr;

  if (type == GemType::RANDOM)
  {
    int rndStartingIndex = 0;

    //
    // Will worthless glass participate in random roll
    //
    if (actualGemChance != -1)
    {
      if (Util::Rolld100(actualGemChance))
      {
        rndStartingIndex = 1;
      }
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

GameObject* ItemsFactory::CreateWand(int x,
                                     int y,
                                     WandMaterials material,
                                     SpellType spellType,
                                     ItemPrefix prefixOverride,
                                     ItemQuality quality)
{
  SpellInfo si = *SpellsDatabase::Instance().GetSpellInfoFromDatabase(spellType);

  if (GlobalConstants::WandSpellCapacityCostByType.count(spellType) == 0)
  {
    auto str = Util::StringFormat("Wands don't support spell '%s'!", si.SpellName.data());
    Logger::Instance().Print(str);
    DebugLog("%s\n", str.data());
    return nullptr;
  }

  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  auto wandColorPair = Colors::WandColorsByMaterial.at(material);
  std::string wandMaterialName = GlobalConstants::WandMaterialNamesByMaterial.at(material);
  std::string spellName = si.SpellName;
  std::string spellShortName = si.SpellShortName;

  go->PosX = x;
  go->PosY = y;

  go->Image = 'i';

  go->FgColor = wandColorPair.first;
  go->BgColor = wandColorPair.second;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM) ? RollItemPrefix() : prefixOverride;
  ic->Data.ItemQuality_ = (quality == ItemQuality::RANDOM) ? RollItemQuality() : quality;
  ic->Data.WandMaterial = material;
  ic->Data.SpellHeld = si;

  Util::RecalculateWandStats(ic);

  ic->Data.IsChargeable = true;
  ic->Data.EqCategory = EquipmentCategory::WEAPON;
  ic->Data.ItemType_ = ItemType::WAND;
  ic->Data.Durability.Reset(1);
  ic->Data.IsIdentified = (prefixOverride != ItemPrefix::RANDOM) ? true : false;

  ic->Data.UnidentifiedName = "?" + wandMaterialName + " Wand?";
  ic->Data.IdentifiedName = wandMaterialName + " Wand of " + spellName;

  ic->Data.UnidentifiedDescription = { "You don't know what it can do" };

  auto str = Util::StringFormat("%s Wand (%s)", wandMaterialName.data(), spellShortName.data());
  go->ObjectName = str;

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* ItemsFactory::CreateRandomWand(ItemPrefix prefixOverride)
{
  GameObject* go = nullptr;

  std::map<WandMaterials, int> materialsDistribution =
  {
    { WandMaterials::YEW_1,    80 },
    { WandMaterials::IVORY_2,  60 },
    { WandMaterials::EBONY_3,  45 },
    { WandMaterials::ONYX_4,   30 },
    { WandMaterials::GLASS_5,  20 },
    { WandMaterials::COPPER_6, 15 },
    { WandMaterials::GOLDEN_7, 10 },
  };

  auto materialPair = Util::WeightedRandom(materialsDistribution);

  std::map<SpellType, int> spellsDistribution =
  {
    { SpellType::LIGHT,         100 },
    { SpellType::STRIKE,         80 },
    { SpellType::FROST,          70 },
    { SpellType::TELEPORT,       60 },
    { SpellType::FIREBALL,       50 },
    { SpellType::LASER,          25 },
    { SpellType::LIGHTNING,      25 },
    { SpellType::MAGIC_MISSILE,  50 }
  };

  auto spellPair = Util::WeightedRandom(spellsDistribution);

  go = CreateWand(0, 0, materialPair.first, spellPair.first, prefixOverride);

  return go;
}

GameObject* ItemsFactory::CreateReturner(int x, int y, int charges, ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->PosX = x;
  go->PosY = y;

  go->Image = '*';

  int colorIndex = RNG::Instance().RandomRange(1, Colors::GemColorNameByType.size());

  GemType t = (GemType)colorIndex;

  std::string colorName = Colors::GemColorNameByType.at(t);

  std::string fgColor = Colors::GemColorByType.at(t).first;
  std::string bgColor = Colors::GemColorByType.at(t).second;

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

  ic->Data.UseCallback = std::bind(&ItemUseHandlers::ReturnerUseHandler,
                                   _1,
                                   _2);

  go->ObjectName = ic->Data.IdentifiedName;

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* ItemsFactory::CreateRepairKit(int x, int y, int charges, ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->PosX = x;
  go->PosY = y;

  go->Image = '(';

  go->FgColor = Colors::WhiteColor;

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

  ic->Data.UseCallback = std::bind(&ItemUseHandlers::RepairKitUseHandler,
                                   _1,
                                   _2);

  go->ObjectName = ic->Data.IdentifiedName;

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* ItemsFactory::CreateArmor(int x, int y, ArmorType type, ItemPrefix prefixOverride, ItemQuality quality)
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

  go->FgColor = Colors::WhiteColor;

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

      AddBonusToItem(ic, { ItemBonusType::SPD, cursedPenalty }, true);

      baseDurability += 2 * (int)ic->Data.ItemQuality_;

      break;

    case ArmorType::LEATHER:
      ic->Data.UnidentifiedDescription =
      {
      // ======================================================================70
        "Jacket made of tanned leather provides decent",
        "protection against cutting blows."
      };

      AddBonusToItem(ic, { ItemBonusType::RES, cursedPenalty - 1 }, true);
      AddBonusToItem(ic, { ItemBonusType::SPD, cursedPenalty - 1 }, true);

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

      AddBonusToItem(ic, { ItemBonusType::RES, cursedPenalty - 3 }, true);
      AddBonusToItem(ic, { ItemBonusType::SPD, cursedPenalty - 2 }, true);

      baseDurability += 4 * (int)ic->Data.ItemQuality_;

      break;

    case ArmorType::SCALE:
      ic->Data.UnidentifiedDescription =
      {
      // ======================================================================70
        "A body vest with overlapping scales worn over a small mail shirt.",
      };

      AddBonusToItem(ic, { ItemBonusType::RES, cursedPenalty - 4 }, true);
      AddBonusToItem(ic, { ItemBonusType::SPD, cursedPenalty - 3 }, true);

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

      AddBonusToItem(ic, { ItemBonusType::RES, cursedPenalty - 6 }, true);
      AddBonusToItem(ic, { ItemBonusType::SPD, cursedPenalty - 4 }, true);

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

GameObject* ItemsFactory::CreateRandomArmor(ArmorType type, ItemPrefix prefixOverride)
{
  GameObject* go = nullptr;

  if (type != ArmorType::RANDOM)
  {
    go = CreateArmor(0, 0, type, prefixOverride);
  }
  else
  {
    int index = RNG::Instance().RandomRange(0, GlobalConstants::ArmorNameByType.size());
    auto it = GlobalConstants::ArmorNameByType.begin();
    std::advance(it, index);
    go = CreateArmor(0, 0, it->first, prefixOverride);
  }

  ItemComponent* ic = go->GetComponent<ItemComponent>();
  TryToAddBonusesToItem(ic);

  return go;
}

GameObject* ItemsFactory::CreateArrows(int x, int y, ArrowType type, ItemPrefix prefixOverride, int amount)
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

  go->StackObjectId = go->ObjectId();

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

GameObject* ItemsFactory::CreateRangedWeapon(int x,
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

      AddBonusToItem(ic, { ItemBonusType::SKL, -1 });
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

      AddBonusToItem(ic, { ItemBonusType::SKL, -2 });
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

      AddBonusToItem(ic, { ItemBonusType::SKL, -3 });
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

      AddBonusToItem(ic, { ItemBonusType::SPD, -1 });
      AddBonusToItem(ic, { ItemBonusType::SKL,  1 });
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

      AddBonusToItem(ic, { ItemBonusType::SPD, -2 });
      AddBonusToItem(ic, { ItemBonusType::SKL,  2 });
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

      AddBonusToItem(ic, { ItemBonusType::SPD, -3 });
      AddBonusToItem(ic, { ItemBonusType::SKL,  3 });
    }
    break;
  }

  for (auto& b : bonuses)
  {
    AddBonusToItem(ic, b);
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

GameObject* ItemsFactory::CreateRandomAccessory(int x, int y,
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
  ic->Data.UnidentifiedDescription =
  {
    "Could be magical or just a trinket."
  };

  // TODO: should rings and amulets quality affect bonuses?

  TryToAddBonusesToItem(ic, atLeastOneBonus);

  if (ic->Data.Bonuses.empty())
  {
    ic->Data.IdentifiedName += " of Adornment";
    ic->Data.Cost = 50 + (int)ic->Data.ItemQuality_ * 10;
  }

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

///
/// Utility method, used for debug creation of items.
///
GameObject* ItemsFactory::CreateAccessory(int x, int y,
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
  ic->Data.UnidentifiedDescription =
  {
    "Could be magical or just a trinket."
  };

  std::vector<ItemBonusType> bonusesRolled;
  for (auto& b : bonuses)
  {
    bonusesRolled.push_back(b.Type);
    AddBonusToItem(ic, b, true);
  }

  if (ic->Data.Bonuses.empty())
  {
    ic->Data.IdentifiedName += " of Adornment";
    ic->Data.Cost = 50 + (int)ic->Data.ItemQuality_ * 10;
  }
  else
  {
    SetMagicItemName(ic, bonusesRolled);
  }

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* ItemsFactory::CreateRandomItem(int x,
                                           int y,
                                           const std::vector<ItemType>& itemsToExclude)
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

  for (auto& item : itemsToExclude)
  {
    auto findRes = std::find(possibleItems.begin(), possibleItems.end(), item);

    if (findRes != possibleItems.end())
    {
      possibleItems.erase(findRes);
    }
  }

  std::map<FoodType, int> foodMap =
  {
    { FoodType::APPLE,        1 },
    { FoodType::CHEESE,       1 },
    { FoodType::BREAD,        1 },
    { FoodType::FISH,         1 },
    { FoodType::PIE,          1 },
    { FoodType::MEAT,         1 },
    { FoodType::TIN,          1 },
    { FoodType::RATIONS,      1 },
    { FoodType::IRON_RATIONS, 1 }
  };

  std::map<GemType, int> gemsMap =
  {
    { GemType::WORTHLESS_GLASS, 250 },
    { GemType::BLACK_OBSIDIAN,  150 },
    { GemType::GREEN_JADE,      100 },
    { GemType::PURPLE_FLUORITE,  75 },
    { GemType::PURPLE_AMETHYST,  50 },
    { GemType::RED_GARNET,       43 },
    { GemType::WHITE_OPAL,       37 },
    { GemType::BLACK_JETSTONE,   35 },
    { GemType::ORANGE_AMBER,     30 },
    { GemType::BLUE_AQUAMARINE,  20 },
    { GemType::YELLOW_CITRINE,   20 },
    { GemType::GREEN_EMERALD,    12 },
    { GemType::BLUE_SAPPHIRE,    10 },
    { GemType::ORANGE_JACINTH,    9 },
    { GemType::RED_RUBY,          8 },
    { GemType::WHITE_DIAMOND,     7 },
  };

  std::map<ItemType, int> returnerMap =
  {
    { ItemType::RETURNER, 1 },
    { ItemType::NOTHING,  4 }
  };

  // TODO: power of randomly created item
  // should scale with dungeon level.
  int index = RNG::Instance().RandomRange(0, possibleItems.size());

  ItemType res = possibleItems[index];

  //
  // TODO: add cases for all item types after they are decided
  //
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

    default:
      DebugLog("CreateRandomItem(): item type %i is not handled!", res);
      break;
  }

  if (go != nullptr)
  {
    go->PosX = x;
    go->PosY = y;
  }

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateNeedleShortSword()
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->ObjectName = "Short Sword";
  go->Image = '(';
  go->FgColor = Colors::ItemUniqueColor;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.EqCategory = EquipmentCategory::WEAPON;
  ic->Data.ItemType_ = ItemType::WEAPON;
  ic->Data.Rarity = ItemRarity::UNIQUE;

  ic->Data.Prefix = ItemPrefix::BLESSED;
  ic->Data.IsIdentified = false;

  int diceRolls = 1;
  int diceSides = 6;

  ic->Data.WeaponType_ = WeaponType::SHORT_SWORD;

  int avgDamage = CalculateAverageDamage(diceRolls, diceSides);

  ic->Data.Damage.SetMin(diceRolls);
  ic->Data.Damage.SetMax(diceSides);

  AddRandomValueBonusToItem(ic, ItemBonusType::IGNORE_DEFENCE);
  AddRandomValueBonusToItem(ic, ItemBonusType::IGNORE_ARMOR);

  AddBonusToItem(ic, { ItemBonusType::SKL, RNG::Instance().RandomRange(0, 3) });
  AddBonusToItem(ic, { ItemBonusType::SPD, RNG::Instance().RandomRange(2, 5) });

  ic->Data.Durability.Reset(RNG::Instance().RandomRange(30, 40));

  ic->Data.UnidentifiedName = "?" + go->ObjectName + "?";
  ic->Data.IdentifiedName = "The Needle";

  auto str = Util::StringFormat("You think it'll do %d damage on average.", avgDamage);
  ic->Data.UnidentifiedDescription = { str, "You can't tell anything else." };

  ic->Data.IdentifiedDescription =
  {
   //=========1=========2=========3=========4=========5=========6=========7=========8
    "A small and elegant looking sword,",
    "it feels very light and easy to handle.",
    "Surprisingly, there are no signs",
    "of the blade ever being used."
  };

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

GameObject* ItemsFactory::CreateBlockBreakerPickaxe()
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->ObjectName = "Pickaxe";
  go->Image = '(';
  go->FgColor = Colors::ItemUniqueColor;

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

  AddRandomValueBonusToItem(ic, ItemBonusType::SELF_REPAIR);

  AddBonusToItem(ic, { ItemBonusType::SKL, 1 });
  AddBonusToItem(ic, { ItemBonusType::SPD, 1 });

  ic->Data.Durability.Reset(30);

  ic->Data.UnidentifiedName = "?" + go->ObjectName + "?";
  ic->Data.IdentifiedName = "Block Breaker";

  auto str = Util::StringFormat("You think it'll do %d damage on average.", avgDamage);
  ic->Data.UnidentifiedDescription = { str, "You can't tell anything else." };

  ic->Data.IdentifiedDescription =
  {
   //=========1=========2=========3=========4=========5=========6=========7=========8
    "This is quite an old but sturdy looking pickaxe,",
    "yet you can't shake the uneasy feeling about it.",
    "There are traces of blood on its head."
  };

  ic->Data.ItemTypeHash = CalculateItemHash(ic);

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateRandomGlass()
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->Image = '*';

  int colorIndex = RNG::Instance().RandomRange(1, Colors::GemColorNameByType.size());

  GemType t = (GemType)colorIndex;

  go->FgColor = Colors::GemColorByType.at(t).first;
  go->BgColor = Colors::GemColorByType.at(t).second;

  std::string colorDesc = Colors::GemColorNameByType.at(t);
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

GameObject* ItemsFactory::CreateGemHelper(GemType t, ItemQuality quality)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->Image = '*';

  go->FgColor = Colors::GemColorByType.at(t).first;
  go->BgColor = Colors::GemColorByType.at(t).second;

  go->ObjectName = GlobalConstants::GemNameByType.at(t);

  std::string colorDesc = Colors::GemColorNameByType.at(t);

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

void ItemsFactory::AdjustBonusWeightsMapForItem(ItemComponent* itemRef, std::map<ItemBonusType, int>& bonusWeightByType)
{
  // Certain items shouldn't have certain bonuses
  // that don't make sense (kinda)
  if (itemRef->Data.EqCategory == EquipmentCategory::BOOTS
   || itemRef->Data.EqCategory == EquipmentCategory::HEAD
   || itemRef->Data.EqCategory == EquipmentCategory::TORSO)
  {
    bonusWeightByType.erase(ItemBonusType::DAMAGE);
    bonusWeightByType.erase(ItemBonusType::IGNORE_DEFENCE);
    bonusWeightByType.erase(ItemBonusType::IGNORE_ARMOR);
    bonusWeightByType.erase(ItemBonusType::KNOCKBACK);
    bonusWeightByType.erase(ItemBonusType::LEECH);
    bonusWeightByType.erase(ItemBonusType::INVISIBILITY);
    bonusWeightByType.erase(ItemBonusType::TELEPATHY);
    bonusWeightByType.erase(ItemBonusType::LEVITATION);
    bonusWeightByType.erase(ItemBonusType::FREE_ACTION);
    bonusWeightByType.erase(ItemBonusType::POISON_IMMUNE);
  }
  else if (itemRef->Data.EqCategory == EquipmentCategory::WEAPON)
  {
    bonusWeightByType.erase(ItemBonusType::REFLECT);
    bonusWeightByType.erase(ItemBonusType::REGEN);
    bonusWeightByType.erase(ItemBonusType::MANA_SHIELD);
    bonusWeightByType.erase(ItemBonusType::DMG_ABSORB);
    bonusWeightByType.erase(ItemBonusType::MAG_ABSORB);
    bonusWeightByType.erase(ItemBonusType::THORNS);
    bonusWeightByType.erase(ItemBonusType::REMOVE_HUNGER);
    bonusWeightByType.erase(ItemBonusType::FREE_ACTION);
    bonusWeightByType.erase(ItemBonusType::POISON_IMMUNE);
    bonusWeightByType.erase(ItemBonusType::INVISIBILITY);
    bonusWeightByType.erase(ItemBonusType::TELEPATHY);
    bonusWeightByType.erase(ItemBonusType::LEVITATION);
  }
  else if (itemRef->Data.EqCategory == EquipmentCategory::NECK
        || itemRef->Data.EqCategory == EquipmentCategory::RING)
  {
    bonusWeightByType.erase(ItemBonusType::INDESTRUCTIBLE);
    bonusWeightByType.erase(ItemBonusType::SELF_REPAIR);
    bonusWeightByType.erase(ItemBonusType::KNOCKBACK);
    bonusWeightByType.erase(ItemBonusType::DAMAGE);
    bonusWeightByType.erase(ItemBonusType::IGNORE_DEFENCE);
    bonusWeightByType.erase(ItemBonusType::IGNORE_ARMOR);
    bonusWeightByType.erase(ItemBonusType::LEECH);
  }
}

void ItemsFactory::TryToAddBonusesToItem(ItemComponent* itemRef, bool atLeastOne)
{
  std::map<ItemBonusType, int> bonusWeightByType =
  {
    { ItemBonusType::STR,             25 },
    { ItemBonusType::DEF,             25 },
    { ItemBonusType::MAG,             25 },
    { ItemBonusType::RES,             25 },
    { ItemBonusType::SKL,             25 },
    { ItemBonusType::SPD,             25 },
    { ItemBonusType::HP,             100 },
    { ItemBonusType::MP,             100 },
    { ItemBonusType::INDESTRUCTIBLE,   7 },
    { ItemBonusType::SELF_REPAIR,     20 },
    { ItemBonusType::VISIBILITY,      50 },
    { ItemBonusType::INVISIBILITY,     5 },
    { ItemBonusType::DAMAGE,          75 },
    { ItemBonusType::REMOVE_HUNGER,   40 },
    { ItemBonusType::POISON_IMMUNE,   15 },
    { ItemBonusType::FREE_ACTION,     15 },
    { ItemBonusType::IGNORE_DEFENCE,  10 },
    { ItemBonusType::IGNORE_ARMOR,    10 },
    { ItemBonusType::KNOCKBACK,       20 },
    { ItemBonusType::MANA_SHIELD,      7 },
    { ItemBonusType::REGEN,            7 },
    { ItemBonusType::REFLECT,          7 },
    { ItemBonusType::LEECH,            7 },
    { ItemBonusType::DMG_ABSORB,      15 },
    { ItemBonusType::MAG_ABSORB,      15 },
    { ItemBonusType::THORNS,          15 },
    { ItemBonusType::TELEPATHY,       35 },
    { ItemBonusType::LEVITATION,      17 }
  };

  AdjustBonusWeightsMapForItem(itemRef, bonusWeightByType);

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
    { ItemQuality::DAMAGED,    -10 },
    { ItemQuality::FLAWED,      -5 },
    { ItemQuality::NORMAL,       0 },
    { ItemQuality::FINE,         5 },
    { ItemQuality::EXCEPTIONAL, 10 },
  };

  chance += chanceModByQ[itemRef->Data.ItemQuality_];

  chance = Util::Clamp(chance, 1, 100);

  std::vector<ItemBonusType> bonusesRolled;
  auto bonusesWeightCopy = bonusWeightByType;

  for (int i = 0; i < 3; i++)
  {
    if (bonusesWeightCopy.empty())
    {
      break;
    }

    if (Util::Rolld100(chance))
    {
      auto res = Util::WeightedRandom(bonusesWeightCopy);

      // No duplicate bonuses on a single item
      bonusesWeightCopy.erase(res.first);

      AddRandomValueBonusToItem(itemRef, res.first);

      bonusesRolled.push_back(res.first);
    }
  }

  if (bonusesRolled.size() == 0 && atLeastOne)
  {
    auto res = Util::WeightedRandom(bonusWeightByType);
    AddRandomValueBonusToItem(itemRef, res.first);
    bonusesRolled.push_back(res.first);
  }

  SetMagicItemName(itemRef, bonusesRolled);
}

void ItemsFactory::AddRandomValueBonusToItem(ItemComponent* itemRef, ItemBonusType bonusType)
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
    { 1, 100 },
    { 2,  75 },
    { 3,  50 },
    { 4,  25 },
    { 5,  12 }
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
      int maxVal = max - 4 * multByQ[q];
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

    //
    // There is no range for telepathy (at least for now)
    //
    case ItemBonusType::TELEPATHY:
    case ItemBonusType::LEVITATION:
    case ItemBonusType::INVISIBILITY:
    case ItemBonusType::IGNORE_DEFENCE:
    case ItemBonusType::IGNORE_ARMOR:
    case ItemBonusType::REMOVE_HUNGER:
    case ItemBonusType::POISON_IMMUNE:
    case ItemBonusType::FREE_ACTION:
    case ItemBonusType::INDESTRUCTIBLE:
    case ItemBonusType::REFLECT:
    case ItemBonusType::MANA_SHIELD:
    {
      value = 1;
      bs.MoneyCostIncrease = value * moneyIncrease;
    }
    break;

    default:
      DebugLog("[WAR] ItemsFactory::AddRandomValueBonusToItem() bonus %i not handled on item %s",
               (int)bonusType,
               itemRef->Data.IdentifiedName.data());
      break;
  }

  bs.BonusValue = value;
  bs.Id = itemRef->OwnerGameObject->ObjectId();
  bs.FromItem = true;

  itemRef->Data.Bonuses.push_back(bs);
}

void ItemsFactory::AddBonusToItem(ItemComponent* itemRef, const ItemBonusStruct& bonusData, bool forceAdd)
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

  // If bonus is like mana shield, that has no bonus value, count it only once
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

void ItemsFactory::SetItemName(GameObject* go, ItemData& itemData)
{
  //
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

    case ItemType::NP_POTION:
      itemData.IdentifiedName.append(" of Cleansing");
      go->ObjectName.append(" -Psd");
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

    //
    // Special info is filled inside ItemComponent::GetInspectionInfo()
    // for specific type of item.
    //
    default:
    {
      if (itemData.Prefix == ItemPrefix::BLESSED
      && (itemData.ItemType_ != ItemType::WEAPON
       && itemData.ItemType_ != ItemType::WAND
       && itemData.ItemType_ != ItemType::ARMOR
       && itemData.ItemType_ != ItemType::RANGED_WEAPON))
      {
        itemData.IdentifiedDescription.push_back("This one is blessed and will perform better.");
      }
      else if (itemData.Prefix == ItemPrefix::CURSED
           && (itemData.ItemType_ != ItemType::WEAPON
            && itemData.ItemType_ != ItemType::WAND
            && itemData.ItemType_ != ItemType::ARMOR
            && itemData.ItemType_ != ItemType::RANGED_WEAPON))
      {
        itemData.IdentifiedDescription.push_back("This one is cursed and should probably be avoided.");
      }
    }
    break;
  }
}

void ItemsFactory::SetMagicItemName(ItemComponent* itemRef, const std::vector<ItemBonusType>& bonusesRolled)
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

void ItemsFactory::BUCQualityAdjust(ItemData& itemData)
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

      int oldMin = itemData.Damage.Min().OriginalValue();
      itemData.Damage.SetMin(oldMin + 1);
    }
    break;

    case ItemQuality::EXCEPTIONAL:
    {
      int dur = itemData.Durability.Max().Get();
      itemData.Durability.Reset(dur * 2);

      itemData.Damage.AddMax(1);
      itemData.Damage.AddMin(1);
    }
    break;
  }

  // Just in case, since '0 dice rolls' makes no sense
  if (itemData.Damage.Min().OriginalValue() == 0)
  {
    itemData.Damage.Min().Set(1);
  }

  // Just to be sure
  if (itemData.Damage.Max().OriginalValue() == 0)
  {
    itemData.Damage.Max().Set(1);
  }
}

int ItemsFactory::CalculateAverageDamage(int numRolls, int diceSides)
{
  int minDmg = (numRolls == 1) ? 0 : numRolls;
  int maxDmg = numRolls * diceSides;

  return (maxDmg - minDmg) / 2;
}

ItemPrefix ItemsFactory::RollItemPrefix()
{
  std::map<ItemPrefix, int> weights =
  {
    { ItemPrefix::UNCURSED, 4 },
    { ItemPrefix::CURSED,   4 },
    { ItemPrefix::BLESSED,  1 }
  };

  auto res = Util::WeightedRandom(weights);
  ItemPrefix prefix = res.first;

  return prefix;
}

ItemQuality ItemsFactory::RollItemQuality()
{
  std::map<ItemQuality, int> weights =
  {
    { ItemQuality::DAMAGED,     8 },
    { ItemQuality::FLAWED,      6 },
    { ItemQuality::NORMAL,      4 },
    { ItemQuality::FINE,        3 },
    { ItemQuality::EXCEPTIONAL, 2 },
  };

  auto res = Util::WeightedRandom(weights);
  ItemQuality quality = res.first;

  return quality;
}

size_t ItemsFactory::CalculateItemHash(ItemComponent* item)
{
  auto strToHash = std::to_string((int)item->Data.Prefix) + item->OwnerGameObject->ObjectName;
  std::hash<std::string> hasher;

  return hasher(strToHash);
}
