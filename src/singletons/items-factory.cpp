#include "items-factory.h"

#include "application.h"
#include "map.h"
#include "game-objects-factory.h"
#include "printer.h"

#ifdef DEBUG_BUILD
#include "logger.h"
#endif

#include "item-component.h"
#include "item-use-handlers.h"

using namespace std::placeholders;

void ItemsFactory::InitSpecific()
{
  _playerRef = &Application::Instance().PlayerInstance;

  _rng.seed(RNG::Instance().Seed);

  InitPotionColors();
  InitScrolls();
}

// =============================================================================

void ItemsFactory::InitPotionColors()
{
  _gamePotionsMap.clear();

  //
  // Is modified, cannot move inside class and make const.
  //
  std::vector<PotionType> potionTypes =
  {
    PotionType::HEALING_POTION,
    PotionType::MANA_POTION,
    PotionType::JUICE_POTION,
    PotionType::NP_POTION,
    PotionType::STR_POTION,
    PotionType::DEF_POTION,
    PotionType::MAG_POTION,
    PotionType::RES_POTION,
    PotionType::SKL_POTION,
    PotionType::SPD_POTION,
    PotionType::EXP_POTION,
    PotionType::CW_POTION,
    PotionType::RA_POTION
  };

  std::shuffle(potionTypes.begin(), potionTypes.end(), _rng);

  size_t mapInd = 0;

  for (size_t i = 0; i < potionTypes.size(); i++)
  {
    auto it = Colors::PotionColorsByName.begin();
    std::advance(it, mapInd);

    PotionInfo pi;

    pi.PotionType_ = potionTypes[i];
    pi.FgBgColor   = { it->second[0], it->second[1] };
    pi.PotionName  = it->first;

    _gamePotionsMap[potionTypes[i]] = pi;

    mapInd++;

    mapInd %= Colors::PotionColorsByName.size();
  }
}

// =============================================================================

void ItemsFactory::InitScrolls()
{
  _gameScrollsMap.clear();

  auto scrollNames = GlobalConstants::ScrollUnidentifiedNames;
  auto validSpells = GlobalConstants::ScrollValidSpellTypes;

  while (scrollNames.size() != 0)
  {
    int scrollNameIndex = Util::RandomRange(0, scrollNames.size(), _rng);
    std::string scrollName = scrollNames[scrollNameIndex];

    int spellIndex = Util::RandomRange(0, validSpells.size(), _rng);
    SpellType spell = validSpells[spellIndex];

    ScrollInfo si;

    si.ScrollName = scrollName;
    si.SpellType_ = spell;

    _gameScrollsMap[spell] = si;

    //auto str = Util::StringFormat(
    //               "%s = %s",
    //               GlobalConstants::SpellNameByType.at(si.SpellType_).data(),
    //               si.ScrollName.data()
    //);
    //Logger::Instance().Print(str);

    // DebugLog("%s = %s\n",
    //          GlobalConstants::SpellNameByType.at(si.SpellType_).data(),
    //          si.ScrollName.data());

    scrollNames.erase(scrollNames.begin() + scrollNameIndex);
    validSpells.erase(validSpells.begin() + spellIndex);
  }
}

// =============================================================================

void ItemsFactory::SetPotionImage(GameObject* go)
{
#ifdef USE_SDL
    go->Image = GlobalConstants::CP437IndexByType[NameCP437::EXCL_MARK_INV];
#else
    go->Image = '!';
#endif
}

// =============================================================================

GameObject* ItemsFactory::CreateMoney(int amount)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->ObjectName = Strings::MoneyName;
  go->Image = '$';
  go->FgColor = Colors::CoinsColor;
  go->BgColor = Colors::None;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.IdentifiedDescription = { "You can buy things with these." };

  int scale = Map::Instance().CurrentLevel->DungeonLevel;

  int money = (amount == 0)
              ? RNG::Instance().RandomRange(1, 11) * scale
              : amount;

  ic->Data.Cost           = money;
  ic->Data.Amount         = money;
  ic->Data.IsStackable    = true;
  ic->Data.ItemType_      = ItemType::COINS;
  ic->Data.IsIdentified   = true;
  ic->Data.IdentifiedName = Strings::MoneyName;

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  go->StackObjectId = go->ObjectId();

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreatePotion(PotionType type,
                                       ItemPrefix prefixOverride)
{
  GameObject* go = nullptr;

  if (GlobalConstants::PotionsWeightTable.count(type) != 0)
  {
    switch (type)
    {
      case PotionType::HEALING_POTION:
        go = CreateHealingPotion(prefixOverride);
        break;

      case PotionType::MANA_POTION:
        go = CreateManaPotion(prefixOverride);
        break;

      case PotionType::NP_POTION:
        go = CreateNeutralizePoisonPotion(prefixOverride);
        break;

      case PotionType::JUICE_POTION:
        go = CreateJuicePotion(prefixOverride);
        break;

      case PotionType::STR_POTION:
      case PotionType::DEF_POTION:
      case PotionType::MAG_POTION:
      case PotionType::RES_POTION:
      case PotionType::SKL_POTION:
      case PotionType::SPD_POTION:
        go = CreateStatPotion(GlobalConstants::StatNameByPotionType.at(type),
                              prefixOverride);
        break;

      case PotionType::EXP_POTION:
        go = CreateExpPotion(prefixOverride);
        break;

      case PotionType::CW_POTION:
        go = CreateCWPotion(prefixOverride);
        break;

      case PotionType::RA_POTION:
        go = CreateRAPotion(prefixOverride);
        break;

      default:
        DebugLog("CreatePotion(): potion type %i is not handled!", type);
        break;
    }
  }
  else
  {
    DebugLog("Potion type %i not found!", (int)type);
  }

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateHealingPotion(ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  PotionType pt = PotionType::HEALING_POTION;

  uint32_t fgColor = _gamePotionsMap[pt].FgBgColor.first;
  uint32_t bgColor = _gamePotionsMap[pt].FgBgColor.second;

  std::string name = _gamePotionsMap[pt].PotionName;

  go->FgColor = fgColor;
  go->BgColor = bgColor;

  SetPotionImage(go);

  go->ObjectName = name;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_ = ItemType::POTION;
  ic->Data.PotionType_ = pt;

  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM)
                    ? RollItemPrefix()
                    : prefixOverride;

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

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

GameObject*
ItemsFactory::CreateNeutralizePoisonPotion(ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  PotionType pt = PotionType::NP_POTION;

  uint32_t fgColor = _gamePotionsMap[pt].FgBgColor.first;
  uint32_t bgColor = _gamePotionsMap[pt].FgBgColor.second;

  std::string name = _gamePotionsMap[pt].PotionName;

  go->FgColor = fgColor;
  go->BgColor = bgColor;

  SetPotionImage(go);

  go->ObjectName = name;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_   = ItemType::POTION;
  ic->Data.PotionType_ = pt;

  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM)
                    ? RollItemPrefix()
                    : prefixOverride;

  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;
  ic->Data.Cost = 5;

  go->StackObjectId = go->ObjectId();

  ic->Data.IdentifiedDescription = { "Removes poison from the body" };
  ic->Data.IdentifiedName        = name;
  ic->Data.UnidentifiedName      = "?" + name + "?";

  ic->Data.UseCallback =
      std::bind(&ItemUseHandlers::NeutralizePoisonPotionUseHandler,
                _1,
                _2);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateManaPotion(ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  PotionType pt = PotionType::MANA_POTION;

  uint32_t fgColor = _gamePotionsMap[pt].FgBgColor.first;
  uint32_t bgColor = _gamePotionsMap[pt].FgBgColor.second;

  std::string name = _gamePotionsMap[pt].PotionName;

  go->FgColor = fgColor;
  go->BgColor = bgColor;

  SetPotionImage(go);

  go->ObjectName = name;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_   = ItemType::POTION;
  ic->Data.PotionType_ = pt;

  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM)
                    ? RollItemPrefix()
                    : prefixOverride;

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

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateJuicePotion(ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  PotionType pt = PotionType::JUICE_POTION;

  uint32_t fgColor = _gamePotionsMap[pt].FgBgColor.first;
  uint32_t bgColor = _gamePotionsMap[pt].FgBgColor.second;

  std::string name = _gamePotionsMap[pt].PotionName;

  go->FgColor = fgColor;
  go->BgColor = bgColor;

  SetPotionImage(go);

  go->ObjectName = name;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_   = ItemType::POTION;
  ic->Data.PotionType_ = pt;

  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM)
                    ? RollItemPrefix()
                    : prefixOverride;

  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;
  ic->Data.Cost = 15;

  go->StackObjectId = go->ObjectId();

  ic->Data.IdentifiedDescription = { "Looks like a fruit juice of some sort" };
  ic->Data.IdentifiedName = name;
  ic->Data.UnidentifiedName = "?" + name + "?";

  ic->Data.UseCallback = std::bind(&ItemUseHandlers::JuicePotionUseHandler,
                                   _1,
                                   _2);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateExpPotion(ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  PotionType pt = PotionType::EXP_POTION;

  uint32_t fgColor = _gamePotionsMap[pt].FgBgColor.first;
  uint32_t bgColor = _gamePotionsMap[pt].FgBgColor.second;

  std::string name = _gamePotionsMap[pt].PotionName;

  go->FgColor = fgColor;
  go->BgColor = bgColor;

  SetPotionImage(go);

  go->ObjectName = name;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_   = ItemType::POTION;
  ic->Data.PotionType_ = pt;

  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM)
                    ? RollItemPrefix()
                    : prefixOverride;

  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;
  ic->Data.Cost = 250;

  go->StackObjectId = go->ObjectId();

  ic->Data.IdentifiedDescription =
  {
    "They say drinking this will bring you to the next level.",
    "Whatever that means..."
  };

  ic->Data.IdentifiedName = name;
  ic->Data.UnidentifiedName = "?" + name + "?";

  ic->Data.UseCallback = std::bind(&ItemUseHandlers::ExpPotionUseHandler,
                                   _1,
                                   _2);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateStatPotion(const std::string& statName,
                                           ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  auto m = Util::FlipMap(GlobalConstants::StatNameByPotionType);

  PotionType pt = m.at(statName);

  uint32_t fgColor = _gamePotionsMap[pt].FgBgColor.first;
  uint32_t bgColor = _gamePotionsMap[pt].FgBgColor.second;

  std::string name = _gamePotionsMap[pt].PotionName;

  go->FgColor = fgColor;
  go->BgColor = bgColor;

  SetPotionImage(go);

  go->ObjectName = name;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_   = ItemType::POTION;
  ic->Data.PotionType_ = pt;

  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM)
                    ? RollItemPrefix()
                    : prefixOverride;

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

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateCWPotion(ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  PotionType pt = PotionType::CW_POTION;

  uint32_t fgColor = _gamePotionsMap[pt].FgBgColor.first;
  uint32_t bgColor = _gamePotionsMap[pt].FgBgColor.second;

  std::string name = _gamePotionsMap[pt].PotionName;

  go->FgColor = fgColor;
  go->BgColor = bgColor;

  SetPotionImage(go);

  go->ObjectName = name;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_   = ItemType::POTION;
  ic->Data.PotionType_ = pt;

  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM)
                    ? RollItemPrefix()
                    : prefixOverride;

  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;
  ic->Data.Cost = 150;

  go->StackObjectId = go->ObjectId();

  ic->Data.IdentifiedDescription =
  {
    "This is frequently used by healers",
    "to help patients regain their strength",
    "after prolonged illness"
  };

  ic->Data.IdentifiedName = name;
  ic->Data.UnidentifiedName = "?" + name + "?";

  ic->Data.UseCallback = std::bind(&ItemUseHandlers::CWPotionUseHandler,
                                   _1,
                                   _2);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateRAPotion(ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  PotionType pt = PotionType::RA_POTION;

  uint32_t fgColor = _gamePotionsMap[pt].FgBgColor.first;
  uint32_t bgColor = _gamePotionsMap[pt].FgBgColor.second;

  std::string name = _gamePotionsMap[pt].PotionName;

  go->FgColor = fgColor;
  go->BgColor = bgColor;

  SetPotionImage(go);

  go->ObjectName = name;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_   = ItemType::POTION;
  ic->Data.PotionType_ = pt;

  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM)
                    ? RollItemPrefix()
                    : prefixOverride;

  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;
  ic->Data.IsIdentified = true;
  ic->Data.Cost = 500;

  go->StackObjectId = go->ObjectId();

  ic->Data.IdentifiedDescription =
  {
    "This is used by people who believe",
    "that they have been cursed"
  };

  ic->Data.IdentifiedName = name;
  ic->Data.UnidentifiedName = "?" + name + "?";

  ic->Data.UseCallback = std::bind(&ItemUseHandlers::RAPotionUseHandler,
                                   _1,
                                   _2);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateRandomPotion()
{
  GameObject* go = nullptr;

  auto weights = Util::WeightedRandom(GlobalConstants::PotionsWeightTable);

  switch (weights.first)
  {
    case PotionType::HEALING_POTION:
      go = CreateHealingPotion();
      break;

    case PotionType::MANA_POTION:
      go = CreateManaPotion();
      break;

    case PotionType::NP_POTION:
      go = CreateNeutralizePoisonPotion();
      break;

    case PotionType::JUICE_POTION:
      go = CreateJuicePotion();
      break;

    case PotionType::EXP_POTION:
      go = CreateExpPotion();
      break;

    case PotionType::CW_POTION:
      go = CreateCWPotion();
      break;

    case PotionType::RA_POTION:
      go = CreateRAPotion();
      break;

    case PotionType::STR_POTION:
    case PotionType::DEF_POTION:
    case PotionType::MAG_POTION:
    case PotionType::RES_POTION:
    case PotionType::SKL_POTION:
    case PotionType::SPD_POTION:
    {
      const std::string& name =
          GlobalConstants::StatNameByPotionType.at(weights.first);
      go = CreateStatPotion(name);
    }
    break;
  }

  ItemComponent* ic = go->GetComponent<ItemComponent>();

  ic->Data.IsIdentified = false;
  ic->Data.UnidentifiedDescription =
  {
    "You don't know what will happen if you drink it."
  };

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateFood(int x,
                                     int y,
                                     FoodType type,
                                     ItemPrefix prefixOverride,
                                     bool isIdentified)
{
  std::string name;
  int addsHunger = 0;

  name = GlobalConstants::FoodHungerPercentageByName.at(type).first;
  int percentage = GlobalConstants::FoodHungerPercentageByName.at(type).second;
  int hungerMax = _playerRef->Attrs.HungerRate.Get();
  int hungerToAdd = ((double)hungerMax * ((double)percentage / 100.0));

  addsHunger = hungerToAdd;

  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->FgColor = Colors::WhiteColor;
  go->BgColor = Colors::None;
  go->Image = '%';
  go->ObjectName = name;
  go->PosX = x;
  go->PosY = y;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.ItemType_ = ItemType::FOOD;
  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM)
                    ? RollItemPrefix()
                    : prefixOverride;
  ic->Data.Amount = 1;
  ic->Data.IsIdentified = isIdentified;

  // Use Cost field to store amount of hunger replenished
  ic->Data.Cost = addsHunger;

  ic->Data.IsStackable = true;
  go->StackObjectId = go->ObjectId();

  std::string unidName = (type == FoodType::RATIONS
                       || type == FoodType::IRON_RATIONS)
                         ? "Rations"
                         : name;

  ic->Data.UnidentifiedName = "?" + unidName + "?";

  ic->Data.UnidentifiedDescription =
  {
    "Looks edible but eat at your own risk."
  };

  ic->Data.IdentifiedDescription = { "Looks edible." };
  ic->Data.IdentifiedName = name;

  ic->Data.UseCallback = std::bind(&ItemUseHandlers::FoodUseHandler,
                                   _1,
                                   _2);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateNote(const std::string& objName,
                                     const std::vector<std::string>& text)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->FgColor = Colors::BlackColor;
  go->BgColor = Colors::WhiteColor;
  go->Image = '?';
  go->ObjectName = objName;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.IsBurnable = true;
  ic->Data.ItemType_ = ItemType::DUMMY;
  ic->Data.IsStackable = false;
  ic->Data.IsIdentified = true;

  ic->Data.IdentifiedDescription = text;
  ic->Data.IdentifiedName = objName;

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

GameObject*
ItemsFactory::CreateDummyItem(const std::string& objName,
                              char image,
                              const uint32_t& fgColor,
                              const uint32_t& bgColor,
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

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateScroll(int x,
                                       int y,
                                       SpellType type,
                                       ItemPrefix prefixOverride)
{
  SpellInfo* si = SpellsDatabase::Instance().GetSpellInfoFromDatabase(type);

  if (std::find(GlobalConstants::ScrollValidSpellTypes.begin(),
                GlobalConstants::ScrollValidSpellTypes.end(),
                type) == GlobalConstants::ScrollValidSpellTypes.end())
  {
    auto msg = Util::StringFormat("[WARNING] Trying to create a scroll "
                                  "with invalid spell (%s)!\n",
                                  si->SpellName.data());
    Printer::Instance().AddMessage(msg);
    LogPrint(msg, true);
    DebugLog("%s\n", msg.data());
    return nullptr;
  }

  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->PosX = x;
  go->PosY = y;
  go->FgColor = Colors::BlackColor;
  go->BgColor = Colors::WhiteColor;
  go->Image = '?';
  go->ObjectName = "\"" + si->SpellName + "\"";

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.IsBurnable = true;

  ic->Data.Prefix = (prefixOverride != ItemPrefix::RANDOM)
                    ? prefixOverride
                    : RollItemPrefix();

  ic->Data.IsIdentified = (prefixOverride != ItemPrefix::RANDOM) ? true : false;

  ic->Data.ItemType_ = ItemType::SCROLL;
  ic->Data.IsStackable = false;

  ic->Data.SpellHeld.SpellType_ = type;

  // NOTE: scrolls cost = base * 2, spellbooks = base * 10
  //
  // Too expensive?
  ic->Data.Cost = si->SpellBaseCost; //si->SpellBaseCost * 2;

  ic->Data.UnidentifiedName = "\"" + _gameScrollsMap[type].ScrollName + "\"";
  ic->Data.UnidentifiedDescription =
  {
    "Who knows what will happen if you read these words aloud..."
  };

  ic->Data.IdentifiedDescription = { "TODO:" };
  ic->Data.IdentifiedName        = "Scroll of " + si->SpellName;

  SetItemName(go, ic->Data);

  ic->Data.UseCallback = std::bind(&ItemUseHandlers::ScrollUseHandler,
                                   _1,
                                   _2);

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateRandomScroll(ItemPrefix prefix)
{
  size_t totalSize = GlobalConstants::ScrollValidSpellTypes.size();
  int index = RNG::Instance().RandomRange(0, totalSize);
  SpellType type = GlobalConstants::ScrollValidSpellTypes.at(index);
  return CreateScroll(0, 0, type, prefix);
}

// =============================================================================

GameObject*
ItemsFactory::CreateMeleeWeapon(int x,
                                int y,
                                WeaponType type,
                                ItemPrefix prefix,
                                ItemQuality quality,
                                const std::vector<ItemBonusStruct>& bonuses)
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
  go->FgColor = Colors::WhiteColor;
  go->BgColor = Colors::None;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.EqCategory = EquipmentCategory::WEAPON;
  ic->Data.ItemType_  = ItemType::WEAPON;

  ic->Data.Prefix = (prefix != ItemPrefix::RANDOM) ? prefix : RollItemPrefix();
  ic->Data.IsIdentified = (prefix != ItemPrefix::RANDOM) ? true : false;

  ic->Data.ItemQuality_ = (quality != ItemQuality::RANDOM)
                          ? quality
                          : RollItemQuality();

  int avgDamage = 0;
  int baseDurability = 0;

  int diceRolls = 0;
  int diceSides = 0;

  ic->Data.WeaponType_ = type;

  //
  // NOTE: innate bonuses are hardcoded and not affected by cursed BUC status,
  // so we don't force-add them like in armor creation case.
  //
  switch (type)
  {
    case WeaponType::DAGGER:
    {
      diceRolls = 1;
      diceSides = 3;

      baseDurability = 30 + 1 * (int)ic->Data.ItemQuality_;

      ic->Data.Damage.SetMin(diceRolls);
      ic->Data.Damage.SetMax(diceSides);

      ic->Data.GeneratedAfter = MapType::MINES_3;

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

      ic->Data.GeneratedAfter = MapType::CAVES_1;

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

      ic->Data.GeneratedAfter = MapType::LOST_CITY;

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

      ic->Data.GeneratedAfter = MapType::LOST_CITY;

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

      ic->Data.GeneratedAfter = MapType::LOST_CITY;

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

  int randomDurAdd = RNG::Instance().RandomRange(0, baseDurability * 0.1) +
                     dungeonLevel;

  int durability = baseDurability + randomDurAdd;
  ic->Data.Durability.Reset(durability);

  ic->Data.UnidentifiedName = "?" + go->ObjectName + "?";
  ic->Data.IdentifiedName = go->ObjectName;

  auto str = Util::StringFormat(Strings::ItemDefaultDescWeaponDmg.data(),
                                avgDamage);
  ic->Data.UnidentifiedDescription = { str, Strings::ItemDefaultDescWeaponEnd };

  // *** !!!
  // Identified description for weapon is
  // returned via private helper method in ItemComponent
  // *** !!!

  BUCQualityAdjust(ic->Data);
  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

GameObject* ItemsFactory::ChooseRandomMeleeWeapon(ItemPrefix prefixOverride,
                                                  ItemQuality qualityOverride)
{
  size_t totalSize = GlobalConstants::WeaponNameByType.size();
  int index = RNG::Instance().RandomRange(0, totalSize);
  auto it = GlobalConstants::WeaponNameByType.begin();
  std::advance(it, index);
  return CreateMeleeWeapon(0, 0, it->first, prefixOverride, qualityOverride);
}

// =============================================================================

GameObject* ItemsFactory::ChooseRandomRangedWeapon(ItemPrefix prefixOverride,
                                                   ItemQuality qualityOverride)
{
  size_t totalSize = GlobalConstants::RangedWeaponNameByType.size();
  int index = RNG::Instance().RandomRange(0, totalSize);
  auto it = GlobalConstants::RangedWeaponNameByType.begin();
  std::advance(it, index);
  return CreateRangedWeapon(0, 0, it->first, prefixOverride, qualityOverride);
}

// =============================================================================

GameObject* ItemsFactory::CreateRandomMeleeWeapon(WeaponType type,
                                                  ItemPrefix prefixOverride,
                                                  ItemQuality qualityOverride)
{
  GameObject* go = nullptr;

  if (type != WeaponType::RANDOM)
  {
    go = CreateMeleeWeapon(0, 0, type, prefixOverride, qualityOverride);
  }
  else
  {
    go = ChooseRandomMeleeWeapon(prefixOverride, qualityOverride);
  }

  ItemComponent* ic = go->GetComponent<ItemComponent>();
  TryToAddBonusesToItem(ic);

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateRandomRangedWeapon(RangedWeaponType type,
                                                   ItemPrefix prefixOverride)
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

// =============================================================================

GameObject* ItemsFactory::CreateRandomWeapon(ItemPrefix prefixOverride,
                                             ItemQuality qualityOverride)
{
  GameObject* go = nullptr;

  int isMelee = RNG::Instance().RandomRange(0, 2);
  if (isMelee == 0)
  {
    go = ChooseRandomMeleeWeapon(prefixOverride, qualityOverride);
  }
  else
  {
    go = ChooseRandomRangedWeapon(prefixOverride, qualityOverride);
  }

  ItemComponent* ic = go->GetComponent<ItemComponent>();
  TryToAddBonusesToItem(ic);

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateGem(int x, int y,
                                    GemType type,
                                    int actualGemChance,
                                    ItemQuality qualityOverride)
{
  GameObject* go = nullptr;

  if (type == GemType::RANDOM)
  {
    int rndStartingIndex = 0;

    //
    // Will worthless glass participate in random roll.
    //
    if (actualGemChance != -1)
    {
      if (Util::Rolld100(actualGemChance))
      {
        rndStartingIndex = 1;
      }
    }

    size_t totalSize = GlobalConstants::GemNameByType.size();

    int index = RNG::Instance().RandomRange(rndStartingIndex, totalSize);

    GemType t = (GemType)index;
    if (t == GemType::WORTHLESS_GLASS)
    {
      go = CreateRandomGlass(qualityOverride);
    }
    else
    {
      go = CreateGemHelper(t, qualityOverride);
    }
  }
  else
  {
    if (type == GemType::WORTHLESS_GLASS)
    {
      go = CreateRandomGlass(qualityOverride);
    }
    else
    {
      go = CreateGemHelper(type, qualityOverride);
    }
  }

  go->PosX = x;
  go->PosY = y;

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateWand(int x,
                                     int y,
                                     WandMaterials material,
                                     SpellType spellType,
                                     ItemPrefix prefixOverride,
                                     ItemQuality quality)
{
  SpellInfo si =
      *SpellsDatabase::Instance().GetSpellInfoFromDatabase(spellType);

  if (GlobalConstants::WandSpellCapacityCostByType.count(spellType) == 0)
  {
    auto str = Util::StringFormat("Wands don't support spell '%s'!",
                                  si.SpellName.data());
    LogPrint(str);
    DebugLog("%s\n", str.data());
    return nullptr;
  }

  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  auto wandColorPair = Colors::WandColorsByMaterial.at(material);
  std::string wandMaterialName =
      GlobalConstants::WandMaterialNamesByMaterial.at(material);
  std::string spellName = si.SpellName;
  std::string spellShortName = si.SpellShortName;

  go->PosX = x;
  go->PosY = y;

  go->Image = 'i';

  go->FgColor = wandColorPair.first;
  go->BgColor = wandColorPair.second;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM)
                    ? RollItemPrefix()
                    : prefixOverride;

  ic->Data.ItemQuality_ = (quality == ItemQuality::RANDOM)
                          ? RollItemQuality()
                          : quality;

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

  auto str = Util::StringFormat("%s Wand (%s)",
                                wandMaterialName.data(),
                                spellShortName.data());
  go->ObjectName = str;

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateRandomWand(ItemPrefix prefixOverride)
{
  GameObject* go = nullptr;

  auto materialPair = Util::WeightedRandom(_wandMaterialsDistribution);
  auto spellPair    = Util::WeightedRandom(_spellsDistribution);

  go = CreateWand(0, 0, materialPair.first, spellPair.first, prefixOverride);

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateReturner(int x,
                                         int y,
                                         int charges,
                                         ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->PosX = x;
  go->PosY = y;

  go->Image = '*';

  size_t totalSize = Colors::GemColorNameByType.size();

  int colorIndex = RNG::Instance().RandomRange(1, totalSize);

  GemType t = (GemType)colorIndex;

  std::string colorName = Colors::GemColorNameByType.at(t);

  uint32_t fgColor = Colors::GemColorByType.at(t).first;
  uint32_t bgColor = Colors::GemColorByType.at(t).second;

  go->FgColor = fgColor;
  go->BgColor = bgColor;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  int chargesNum = (charges == -1)
                  ? RNG::Instance().RandomRange(1, 6)
                  : charges;

  ic->Data.ItemType_ = ItemType::RETURNER;

  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM)
                    ? RollItemPrefix()
                    : prefixOverride;

  ic->Data.IsStackable = false;
  ic->Data.IsIdentified = (prefixOverride == ItemPrefix::RANDOM) ? false : true;
  ic->Data.IsChargeable = true;
  ic->Data.Cost = 50;

  ic->Data.Amount = chargesNum;

  if (ic->Data.Prefix == ItemPrefix::BLESSED)
  {
    ic->Data.Amount = chargesNum * 2;
  }

  ic->Data.UnidentifiedDescription = { Strings::ItemDefaultDescGem };

  ic->Data.IdentifiedName = colorName + " Returner";
  ic->Data.UnidentifiedName = "?" + colorName + " Gem?";

  ic->Data.UseCallback = std::bind(&ItemUseHandlers::ReturnerUseHandler,
                                   _1,
                                   _2);

  go->ObjectName = ic->Data.IdentifiedName;

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateRepairKit(int x,
                                          int y,
                                          int charges,
                                          ItemPrefix prefixOverride)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->PosX = x;
  go->PosY = y;

  go->Image = '(';

  go->FgColor = Colors::WhiteColor;
  go->BgColor = Colors::None;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  int chargesNum = (charges == -1)
                   ? RNG::Instance().RandomRange(1, 51)
                   : charges;

  ic->Data.ItemType_ = ItemType::REPAIR_KIT;

  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM)
                    ? RollItemPrefix()
                    : prefixOverride;

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

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateArmor(int x,
                                      int y,
                                      ArmorType type,
                                      ItemPrefix prefixOverride,
                                      ItemQuality quality)
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
  go->BgColor = Colors::None;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.EqCategory = EquipmentCategory::TORSO;
  ic->Data.ItemType_ = ItemType::ARMOR;

  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM)
                    ? RollItemPrefix()
                    : prefixOverride;

  ic->Data.IsStackable = false;
  ic->Data.IsIdentified = (prefixOverride == ItemPrefix::RANDOM) ? false : true;

  ic->Data.ItemQuality_ = (quality != ItemQuality::RANDOM)
                          ? quality
                          : RollItemQuality();

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
      // ----------------------------------------------------------------------70
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
      // ----------------------------------------------------------------------70
        "Jacket made of tanned leather provides decent",
        "protection against cutting blows."
      };

      ic->Data.GeneratedAfter = MapType::MINES_3;

      AddBonusToItem(ic, { ItemBonusType::RES, cursedPenalty - 1 }, true);
      AddBonusToItem(ic, { ItemBonusType::SPD, cursedPenalty - 1 }, true);

      baseDurability += 3 * (int)ic->Data.ItemQuality_;

      break;

    case ArmorType::MAIL:
      ic->Data.UnidentifiedDescription =
      {
      // ----------------------------------------------------------------------70
        "A shirt made of metal rings",
        "is a popular outfit among common soldiers.",
        "It takes a while to adjust to its weight,",
        "but it offers good overall protection",
        "and is easy to repair."
      };

      ic->Data.GeneratedAfter = MapType::CAVES_3;

      AddBonusToItem(ic, { ItemBonusType::RES, cursedPenalty - 3 }, true);
      AddBonusToItem(ic, { ItemBonusType::SPD, cursedPenalty - 2 }, true);

      baseDurability += 4 * (int)ic->Data.ItemQuality_;

      break;

    case ArmorType::SCALE:
      ic->Data.UnidentifiedDescription =
      {
      // ----------------------------------------------------------------------70
        "A body vest with overlapping scales worn over a small mail shirt.",
      };

      ic->Data.GeneratedAfter = MapType::LOST_CITY;

      AddBonusToItem(ic, { ItemBonusType::RES, cursedPenalty - 4 }, true);
      AddBonusToItem(ic, { ItemBonusType::SPD, cursedPenalty - 3 }, true);

      baseDurability += 5 * (int)ic->Data.ItemQuality_;

      break;

    case ArmorType::PLATE:
      ic->Data.UnidentifiedDescription =
      {
      // ----------------------------------------------------------------------70
        "A thick layer of padding, then a layer of a strong mail",
        "with metal plates riveted on top.",
        "The best protection you can find, usually worn by",
        "nobles and knights, this armor pretty much combines",
        "all others in itself."
      };

      ic->Data.GeneratedAfter = MapType::DEEP_DARK_3;

      AddBonusToItem(ic, { ItemBonusType::RES, cursedPenalty - 6 }, true);
      AddBonusToItem(ic, { ItemBonusType::SPD, cursedPenalty - 4 }, true);

      baseDurability += 6 * (int)ic->Data.ItemQuality_;

      break;
  }

  //ic->Data.IdentifiedDescription = ic->Data.UnidentifiedDescription;

  int randomDurAdd = RNG::Instance().RandomRange(0, baseDurability * 0.1) +
                     dungeonLevel;

  int durability = baseDurability + randomDurAdd;
  ic->Data.Durability.Reset(durability);

  ic->Data.UnidentifiedName = "?" + go->ObjectName + "?";
  ic->Data.IdentifiedName = go->ObjectName;

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateRandomArmor(ArmorType type,
                                            ItemPrefix prefixOverride,
                                            ItemQuality qualityOverride)
{
  GameObject* go = nullptr;

  if (type != ArmorType::RANDOM)
  {
    go = CreateArmor(0, 0, type, prefixOverride, qualityOverride);
  }
  else
  {
    size_t totalSize = GlobalConstants::ArmorNameByType.size();
    int index = RNG::Instance().RandomRange(0, totalSize);
    auto it = GlobalConstants::ArmorNameByType.begin();
    std::advance(it, index);
    go = CreateArmor(0, 0, it->first, prefixOverride, qualityOverride);
  }

  ItemComponent* ic = go->GetComponent<ItemComponent>();
  TryToAddBonusesToItem(ic);

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateArrows(int x,
                                       int y,
                                       ArrowType type,
                                       ItemPrefix prefixOverride,
                                       int amount)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->PosX = x;
  go->PosY = y;

  go->Image = '^';
  go->FgColor = Colors::WhiteColor;
  go->BgColor = Colors::None;

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

  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM)
                    ? RollItemPrefix()
                    : prefixOverride;

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

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

GameObject*
ItemsFactory::CreateRangedWeapon(int x,
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
  go->FgColor = Colors::WhiteColor;
  go->BgColor = Colors::None;

  go->ObjectName = GlobalConstants::RangedWeaponNameByType.at(type);

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.RangedWeaponType_ = type;

  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM)
                    ? RollItemPrefix()
                    : prefixOverride;

  ic->Data.ItemQuality_ = (quality != ItemQuality::RANDOM)
                          ? quality
                          : RollItemQuality();

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

      ic->Data.UnidentifiedDescription =
      {
      // ----------------------------------------------------------------------70
        "A simple wooden short bow with short range.",
        "Requires some skill to be used effectively."
      };

      ic->Data.GeneratedAfter = MapType::MINES_3;

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

      ic->Data.UnidentifiedDescription =
      {
      // ----------------------------------------------------------------------70
        "A wooden bow for hunting animals and the like with medium range.",
        "Requires some skill to be used effectively."
      };

      ic->Data.GeneratedAfter = MapType::CAVES_3;

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

      ic->Data.GeneratedAfter = MapType::LOST_CITY;

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

      ic->Data.UnidentifiedDescription =
      {
      // ----------------------------------------------------------------------70
        "A light crossbow has shorter range than its bow counterpart",
        "but has more punch and is easier to aim with.",
        "Requires some time to reload."
      };

      ic->Data.GeneratedAfter = MapType::CAVES_1;

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

      ic->Data.UnidentifiedDescription =
      {
      // ----------------------------------------------------------------------70
        "A crossbow has shorter range than its bow counterpart",
        "but has more punch and is easier to aim with.",
        "Requires some time to reload."
      };

      ic->Data.GeneratedAfter = MapType::LOST_CITY;

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

      ic->Data.UnidentifiedDescription =
      {
      // ----------------------------------------------------------------------70
        "This heavy crossbow can deal some serious damage,",
        "but doesn't have that much range than its bow counterpart.",
        "Requires some time to reload."
      };

      ic->Data.GeneratedAfter = MapType::LOST_CITY;

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

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

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

  go->FgColor = Colors::WhiteColor;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.Prefix = (prefixOverride == ItemPrefix::RANDOM)
                    ? RollItemPrefix()
                    : prefixOverride;

  ic->Data.EqCategory = category;
  ic->Data.ItemType_ = ItemType::ACCESSORY;
  ic->Data.IsIdentified = (prefixOverride != ItemPrefix::RANDOM) ? true : false;
  ic->Data.UnidentifiedName = "?" + go->ObjectName + "?";
  ic->Data.IdentifiedName = go->ObjectName;
  ic->Data.UnidentifiedDescription =
  {
    Strings::ItemDefaultDescAccessory
  };

  // TODO: should rings and amulets quality affect bonus / curse strength?

  TryToAddBonusesToItem(ic, atLeastOneBonus);

  if (ic->Data.Bonuses.empty())
  {
    ic->Data.IdentifiedName += " of Adornment";
    ic->Data.Cost = 50 + (int)ic->Data.ItemQuality_ * 10;
  }

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

//
// Utility method, used for debug creation of items.
//
GameObject*
ItemsFactory::CreateAccessory(int x, int y,
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

  go->FgColor = Colors::WhiteColor;
  go->BgColor = Colors::None;

  ItemComponent* ic = go->AddComponent<ItemComponent>();
  ic->Data.Prefix = (prefix == ItemPrefix::RANDOM) ? RollItemPrefix() : prefix;

  ic->Data.ItemQuality_ = (quality != ItemQuality::RANDOM)
                          ? quality
                          : RollItemQuality();

  ic->Data.EqCategory = category;
  ic->Data.ItemType_ = ItemType::ACCESSORY;
  ic->Data.IsIdentified = (prefix != ItemPrefix::RANDOM) ? true : false;
  ic->Data.UnidentifiedName = "?" + go->ObjectName + "?";
  ic->Data.IdentifiedName = go->ObjectName;
  ic->Data.UnidentifiedDescription =
  {
    Strings::ItemDefaultDescAccessory
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

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

GameObject*
ItemsFactory::CreateRandomItem(int x,
                               int y,
                               const std::vector<ItemType>& itemsToExclude)
{
  GameObject* go = nullptr;

  std::vector<ItemType> possibleItems =
  {
    ItemType::COINS,
    ItemType::POTION,
    ItemType::FOOD,
    ItemType::SCROLL,
    ItemType::GEM,
    ItemType::RETURNER,
    ItemType::WEAPON,
    ItemType::ARMOR,
    ItemType::ACCESSORY,
    ItemType::WAND,
    ItemType::SPELLBOOK,
    ItemType::REPAIR_KIT,
    ItemType::ARROWS
  };

  for (auto& item : itemsToExclude)
  {
    auto findRes = std::find(possibleItems.begin(), possibleItems.end(), item);

    if (findRes != possibleItems.end())
    {
      possibleItems.erase(findRes);
    }
  }

  //
  // TODO: power of randomly created item
  // should scale with dungeon level.
  //
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
      auto pair = Util::WeightedRandom(_foodMap);
      go = CreateFood(0, 0, pair.first);
    }
    break;

    case ItemType::GEM:
    {
      auto pair = Util::WeightedRandom(_gemsMap);
      go = CreateGem(0, 0, pair.first);
    }
    break;

    case ItemType::RETURNER:
    {
      auto pair = Util::WeightedRandom(_returnerMap);
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
  go->BgColor = Colors::None;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.EqCategory = EquipmentCategory::WEAPON;
  ic->Data.ItemType_  = ItemType::WEAPON;
  ic->Data.Rarity     = ItemRarity::UNIQUE;

  ic->Data.Prefix       = ItemPrefix::BLESSED;
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
  ic->Data.IdentifiedName = HIDE("The Needle");

  auto str = Util::StringFormat(Strings::ItemDefaultDescWeaponDmg.data(),
                                avgDamage);
  ic->Data.UnidentifiedDescription = { str, Strings::ItemDefaultDescWeaponEnd };

  ic->Data.IdentifiedDescription =
  {
          //---------1---------2---------3---------4---------5---------6---------7---------8
    { HIDE("A small and elegant looking sword,")      },
    { HIDE("it feels very light and easy to handle.") },
    { HIDE("Surprisingly, there are no signs")        },
    { HIDE("of the blade ever being used.")           }
  };

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateBlockBreakerPickaxe()
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->ObjectName = "Pickaxe";
  go->Image = '(';
  go->FgColor = Colors::ItemUniqueColor;
  go->BgColor = Colors::None;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.EqCategory = EquipmentCategory::WEAPON;
  ic->Data.ItemType_  = ItemType::WEAPON;
  ic->Data.Rarity     = ItemRarity::UNIQUE;

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
  ic->Data.IdentifiedName = HIDE("Block Breaker");

  auto str = Util::StringFormat(Strings::ItemDefaultDescWeaponDmg.data(),
                                avgDamage);
  ic->Data.UnidentifiedDescription = { str, Strings::ItemDefaultDescWeaponEnd };

  ic->Data.IdentifiedDescription =
  {
   //---------1---------2---------3---------4---------5---------6---------7---------8
    { HIDE("This is quite an old but sturdy looking pickaxe,") },
    { HIDE("yet you can't shake the uneasy feeling about it.") },
    { HIDE("There are traces of blood on its head.")           }
  };

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateOneRing()
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->ObjectName = "Ring";
  go->Image = '=';
  go->FgColor = Colors::ItemUniqueColor;
  go->BgColor = Colors::None;

  ItemComponent* ic = go->AddComponent<ItemComponent>();

  ic->Data.EqCategory = EquipmentCategory::RING;
  ic->Data.ItemType_  = ItemType::ACCESSORY;
  ic->Data.Rarity     = ItemRarity::UNIQUE;

  ic->Data.Prefix = ItemPrefix::CURSED;
  ic->Data.IsIdentified = false;

  int rndStr = RNG::Instance().RandomRange(1, 5);

  AddBonusToItem(ic, { ItemBonusType::INVISIBILITY, 1 });
  AddBonusToItem(ic, { ItemBonusType::TELEPATHY,    1 });
  AddBonusToItem(ic, { ItemBonusType::STR,     rndStr });

  ic->Data.UnidentifiedName = "?" + go->ObjectName + "?";
  ic->Data.IdentifiedName = HIDE("The One Ring");
  ic->Data.UnidentifiedDescription =
  {
    Strings::ItemDefaultDescAccessory
  };

  ic->Data.IdentifiedDescription =
  {
   //---------1---------2---------3---------4---------5---------6---------7---------8
    { HIDE("This looks like a simple golden ring.")                          },
    { HIDE("There are strange inscriptions running along the side of it.")   },
    { HIDE("Maybe letters, but then from a language you don't recognize.")   },
    { HIDE("For some reason you have a strange urge to put this ring on...") }
  };

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

GameObject* ItemsFactory::CreateRandomGlass(ItemQuality quality)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel);

  go->Image = '*';

  size_t totalSize = Colors::GemColorNameByType.size();

  int colorIndex = RNG::Instance().RandomRange(1, totalSize);

  GemType t = (GemType)colorIndex;

  go->FgColor = Colors::GemColorByType.at(t).first;
  go->BgColor = Colors::GemColorByType.at(t).second;

  std::string colorDesc = Colors::GemColorNameByType.at(t);
  go->ObjectName = Util::StringFormat("%s Glass", colorDesc.data());

  ItemComponent* ic = go->AddComponent<ItemComponent>();
  ic->Data.ItemType_ = ItemType::GEM;
  ic->Data.IsStackable = false;
  ic->Data.IsIdentified = false;

  ic->Data.ItemQuality_ = (quality != ItemQuality::RANDOM)
                          ? quality
                          : RollItemQuality();

  ic->Data.UnidentifiedDescription = { Strings::ItemDefaultDescGem };
  ic->Data.UnidentifiedName = Util::StringFormat("?%s Gem?", colorDesc.data());

  std::string lowerCase = colorDesc;
  std::transform(lowerCase.begin(),
                 lowerCase.end(),
                 lowerCase.begin(),
                 ::tolower);

  auto str = Util::StringFormat("This is a piece of %s worthless glass",
                                lowerCase.data());

  ic->Data.IdentifiedDescription = { str };

  ic->Data.IdentifiedName = go->ObjectName;

  ic->Data.Cost = 10 * _costMultByQuality.at(ic->Data.ItemQuality_);

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

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

  ic->Data.UnidentifiedDescription = { Strings::ItemDefaultDescGem };
  ic->Data.UnidentifiedName = Util::StringFormat("?%s Gem?", colorDesc.data());

  if (GlobalConstants::GemDescriptionByType.count(t) == 1)
  {
    ic->Data.IdentifiedDescription =
        GlobalConstants::GemDescriptionByType.at(t);
  }
  else
  {
    std::string str =
        Util::StringFormat("%s description goes here",
                           GlobalConstants::GemNameByType.at(t).data());

    ic->Data.IdentifiedDescription = { str };
  }

  ic->Data.ItemQuality_ = (quality != ItemQuality::RANDOM)
                          ? quality
                          : RollItemQuality();

  int additionalCost = 0;

  ItemQuality q = ic->Data.ItemQuality_;

  double baseCost = (double)GlobalConstants::GemCostByType.at(t) *
                    _costMultByQuality.at(q);

  double upperBound = baseCost / 2.0;
  int range = RNG::Instance().RandomRange(0, (int)upperBound);

  switch (q)
  {
    case ItemQuality::DAMAGED:
    case ItemQuality::FLAWED:
      additionalCost = -range;
      break;

    case ItemQuality::NORMAL:
      additionalCost = 0;
      break;

    case ItemQuality::FINE:
    case ItemQuality::EXCEPTIONAL:
      additionalCost = range;
      break;
  }

  ic->Data.IdentifiedName = GlobalConstants::GemNameByType.at(t);

  ic->Data.Cost = (int)baseCost + additionalCost;

  SetItemName(go, ic->Data);

  ic->Data.ItemTypeHash = Util::CalculateItemHash(ic);

  return go;
}

// =============================================================================

void ItemsFactory::AdjustBonusWeightsMapForItem(
    ItemComponent* itemRef,
    std::unordered_map<ItemBonusType, int>& bonusWeightByType
  )
{
  //
  // Certain items shouldn't have certain bonuses
  // that don't make sense (kinda).
  //
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
    bonusWeightByType.erase(ItemBonusType::TRUE_SEEING);
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
    bonusWeightByType.erase(ItemBonusType::TRUE_SEEING);
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

// =============================================================================

void ItemsFactory::TryToAddBonusesToItem(ItemComponent* itemRef,
                                         bool atLeastOne)
{
  std::unordered_map<ItemBonusType, int> bonusWeightByType =
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
    { ItemBonusType::TRUE_SEEING,     25 },
    { ItemBonusType::LEVITATION,      17 }
  };

  AdjustBonusWeightsMapForItem(itemRef, bonusWeightByType);

  int curDungeonLvl = Map::Instance().CurrentLevel->DungeonLevel;

  // Increase chance of traders having magic items in town
  if (Map::Instance().CurrentLevel->MapType_ == MapType::TOWN)
  {
    curDungeonLvl = (int)MapType::THE_END / 2;
  }

  const double scale = 1.25;
  double minSucc = 1.0 / ((double)MapType::THE_END * scale);
  double curSucc = minSucc * curDungeonLvl;
  int chance = (int)(curSucc * 100);

  chance += _chanceModByQ.at(itemRef->Data.ItemQuality_);

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

// =============================================================================

void ItemsFactory::AddRandomValueBonusToItem(ItemComponent* itemRef,
                                             ItemBonusType bonusType)
{
  //
  // No negative bonuses assumed.
  //
  int moneyIncrease =
      GlobalConstants::MoneyCostIncreaseByBonusType.at(bonusType);

  ItemBonusStruct bs;
  bs.Type = bonusType;
  bs.MoneyCostIncrease = (itemRef->Data.Prefix == ItemPrefix::CURSED)
                         ? moneyIncrease / 2
                         : moneyIncrease;

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
      auto res = Util::WeightedRandom(_statIncreaseWeightsMap);
      value = res.first;
      bs.MoneyCostIncrease = res.first * moneyIncrease;
    }
    break;

    case ItemBonusType::HP:
    case ItemBonusType::MP:
    {
      int min = 1 + _multByQ.at(q);
      int max = 10 + _multByQ.at(q) * 2;
      value = RNG::Instance().RandomRange(min, max);
      bs.MoneyCostIncrease = value * moneyIncrease;
    }
    break;

    case ItemBonusType::DMG_ABSORB:
    case ItemBonusType::MAG_ABSORB:
    {
      int min = 1;
      int max = _multByQ.at(q);
      value = RNG::Instance().RandomRange(min, max + 1);
      bs.MoneyCostIncrease = value * moneyIncrease;
    }
    break;

    case ItemBonusType::DAMAGE:
    {
      int min = _multByQ.at(q);
      int max = _multByQ.at(q) * 2;
      value = RNG::Instance().RandomRange(min, max + 1);
      bs.MoneyCostIncrease = value * moneyIncrease;
    }
    break;

    case ItemBonusType::VISIBILITY:
    {
      int min = _multByQ.at(q);
      int max = _multByQ.at(q) * 3;
      value = RNG::Instance().RandomRange(min, max + 1);
      bs.MoneyCostIncrease = value * moneyIncrease;
    }
    break;

    case ItemBonusType::THORNS:
    {
      int min = 20 + 5 * (_multByQ.at(q) - 1);
      int max = 21 + 20 * _multByQ.at(q);
      int percentage = RNG::Instance().RandomRange(min, max);
      value = percentage;
      bs.MoneyCostIncrease = value * moneyIncrease;
    }
    break;

    case ItemBonusType::LEECH:
    {
      int min = 20 + 5 * (_multByQ.at(q) - 1);
      int max = 21 + 15 * _multByQ.at(q);
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

      //
      // Number of turns before effect acts.
      //
      int minVal = min - 2 * _multByQ.at(q);
      int maxVal = max - 4 * _multByQ.at(q);
      bs.Period = RNG::Instance().RandomRange(minVal, maxVal + 1);
      bs.MoneyCostIncrease = (int)(((double)max / (double)bs.Period) *
                                   (double)moneyIncrease);
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
    case ItemBonusType::TRUE_SEEING:
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
      DebugLog("[WAR] ItemsFactory::AddRandomValueBonusToItem() "
               "bonus %i not handled on item %s",
               (int)bonusType,
               itemRef->Data.IdentifiedName.data());
      break;
  }

  bs.BonusValue = value;
  bs.Id = itemRef->OwnerGameObject->ObjectId();
  bs.Persistent = true;

  itemRef->Data.Bonuses.push_back(bs);
}

// =============================================================================

void ItemsFactory::AddBonusToItem(ItemComponent* itemRef,
                                  const ItemBonusStruct& bonusData,
                                  bool forceAdd)
{
  //
  // If bonus doesn't modify anything,
  // (i.e. if during weapon / armor generation total value of modifier
  // became 0 due to being cursed), don't add it unless forced to
  // (if it's some kind of 'special' item bonus
  // like mana shield or knockback where bonus value is not used).
  //
  if (!forceAdd && bonusData.BonusValue == 0)
  {
    return;
  }

  auto copy = bonusData;

  int moneyIncrease =
      GlobalConstants::MoneyCostIncreaseByBonusType.at(bonusData.Type);

  //
  // If bonus is like mana shield, that has no bonus value, count it only once.
  //
  if (bonusData.BonusValue == 0)
  {
    copy.MoneyCostIncrease = moneyIncrease;
  }

  //
  // Negative bonus values aren't taken into account regarding total cost.
  //
  else if (bonusData.BonusValue > 0)
  {
    int total = bonusData.BonusValue * moneyIncrease;

    //
    // MoneyCostIncrease is used to calculate total cost of the item
    // in ShoppingState by accumulating all these
    // values into one resulting variable.
    //
    copy.MoneyCostIncrease = total;
  }

  copy.Id = itemRef->OwnerGameObject->ObjectId();
  copy.Persistent = true;

  itemRef->Data.Bonuses.push_back(copy);
}

// =============================================================================

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
      std::string qualityName =
          GlobalConstants::QualityNameByQuality.at(itemData.ItemQuality_);
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
    case ItemType::POTION:
    {
      switch (itemData.PotionType_)
      {
        case PotionType::HEALING_POTION:
          itemData.IdentifiedName.append(" of Healing");
          go->ObjectName.append(" +HP");
          break;

        case PotionType::MANA_POTION:
          itemData.IdentifiedName.append(" of Mana");
          go->ObjectName.append(" +MP");
          break;

        case PotionType::NP_POTION:
          itemData.IdentifiedName.append(" of Cleansing");
          go->ObjectName.append(" -Psd");
          break;

        case PotionType::JUICE_POTION:
          itemData.IdentifiedName.append(" of Fruit Juice");
          go->ObjectName.append(" +SAT");
          break;

        case PotionType::EXP_POTION:
          itemData.IdentifiedName.append(" of Enlightenment");
          go->ObjectName.append(" +EXP");
          break;

        case PotionType::STR_POTION:
          itemData.IdentifiedName.append(" of Strength");
          go->ObjectName.append(" +STR");
          break;

        case PotionType::DEF_POTION:
          itemData.IdentifiedName.append(" of Defence");
          go->ObjectName.append(" +DEF");
          break;

        case PotionType::MAG_POTION:
          itemData.IdentifiedName.append(" of Magic");
          go->ObjectName.append(" +MAG");
          break;

        case PotionType::RES_POTION:
          itemData.IdentifiedName.append(" of Resistance");
          go->ObjectName.append(" +RES");
          break;

        case PotionType::SKL_POTION:
          itemData.IdentifiedName.append(" of Skill");
          go->ObjectName.append(" +SKL");
          break;

        case PotionType::SPD_POTION:
          itemData.IdentifiedName.append(" of Speed");
          go->ObjectName.append(" +SPD");
          break;

        case PotionType::CW_POTION:
          itemData.IdentifiedName.append(" of Cure Weakness");
          go->ObjectName.append(" -Wea");
          break;

        case PotionType::RA_POTION:
          itemData.IdentifiedName.append(" of Restore Ability");
          go->ObjectName.append(" +Lvl");
          break;
      }
      break;
    }
    break;

    case ItemType::REPAIR_KIT:
    {
      if (itemData.Prefix == ItemPrefix::BLESSED)
      {
        itemData.IdentifiedDescription
            .push_back("Because of its excellent condition,");
        itemData.IdentifiedDescription
            .push_back("repairing will be more effective.");
      }
      else if (itemData.Prefix == ItemPrefix::CURSED)
      {
        itemData.IdentifiedDescription
            .push_back("Because of its poor condition,");
        itemData.IdentifiedDescription
            .push_back("repairing will be less effective.");
      }
    }
    break;

    case ItemType::ARROWS:
    {
      if (itemData.Prefix == ItemPrefix::BLESSED)
      {
        itemData.IdentifiedDescription
            .push_back("These projectiles are blessed");
        itemData.IdentifiedDescription
            .push_back("and thus more likely to hit the enemy.");
      }
      else if (itemData.Prefix == ItemPrefix::CURSED)
      {
        itemData.IdentifiedDescription
            .push_back("These projectiles are cursed");
        itemData.IdentifiedDescription
            .push_back("and thus less likely to hit the enemy.");
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
        itemData.IdentifiedDescription
            .push_back("This one is blessed and will perform better.");
      }
      else if (itemData.Prefix == ItemPrefix::CURSED
           && (itemData.ItemType_ != ItemType::WEAPON
            && itemData.ItemType_ != ItemType::WAND
            && itemData.ItemType_ != ItemType::ARMOR
            && itemData.ItemType_ != ItemType::RANGED_WEAPON))
      {
        itemData.IdentifiedDescription
            .push_back("This one is cursed and should probably be avoided.");
      }
    }
    break;
  }
}

// =============================================================================

void ItemsFactory::SetMagicItemName(
    ItemComponent* itemRef,
    const std::vector<ItemBonusType>& bonusesRolled
  )
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
      //
      // Randomize resulting name a bit in case of more than 2 bonuses.
      //
      std::vector<ItemBonusType> bonusesRolledCopy = bonusesRolled;

      int suffixInd1 = RNG::Instance().RandomRange(0, 3);
      auto prefix1 =
          GlobalConstants::ItemBonusPrefixes.at(bonusesRolledCopy[suffixInd1]);
      rarePrefixes.push_back(prefix1);
      bonusesRolledCopy.erase(bonusesRolledCopy.begin() + suffixInd1);

      int suffixInd2 = RNG::Instance().RandomRange(0, 2);
      auto prefix2 =
          GlobalConstants::ItemBonusPrefixes.at(bonusesRolledCopy[suffixInd2]);
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

  std::string quality =
      GlobalConstants::QualityNameByQuality.at(itemRef->Data.ItemQuality_);
  if (!quality.empty())
  {
    bucStatus.append(1, ' ');
  }

  std::string itemName = Util::StringFormat("%s%s %s",
                                            bucStatus.data(),
                                            quality.data(),
                                            objName.data());
  if (bonusesRolled.size() == 1)
  {
    itemName = Util::StringFormat("%s%s %s %s",
                                  bucStatus.data(),
                                  quality.data(),
                                  prefix.data(),
                                  objName.data());
  }
  else if (bonusesRolled.size() == 2)
  {
    itemName = Util::StringFormat("%s%s %s %s %s",
                                  bucStatus.data(),
                                  quality.data(),
                                  prefix.data(),
                                  objName.data(),
                                  suffix.data());
  }
  else if (bonusesRolled.size() == 3)
  {
    itemName = Util::StringFormat("%s%s %s %s %s %s",
                                  bucStatus.data(),
                                  quality.data(),
                                  rarePrefixes[0].data(),
                                  rarePrefixes[1].data(),
                                  objName.data(),
                                  suffix.data());
  }

  itemRef->Data.IdentifiedName = itemName;
}

// =============================================================================

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
      double durF = (double)itemData.Durability.Max().Get() / 1.5;
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
      double durF = (double)itemData.Durability.Max().Get() * 1.5;
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

  //
  // Just in case, since '0 dice rolls' makes no sense.
  //
  if (itemData.Damage.Min().OriginalValue() == 0)
  {
    itemData.Damage.Min().Set(1);
  }

  //
  // Just to be sure.
  //
  if (itemData.Damage.Max().OriginalValue() == 0)
  {
    itemData.Damage.Max().Set(1);
  }
}

// =============================================================================

int ItemsFactory::CalculateAverageDamage(int numRolls, int diceSides)
{
  int minDmg = (numRolls == 1) ? 0 : numRolls;
  int maxDmg = numRolls * diceSides;

  return (maxDmg - minDmg) / 2;
}

// =============================================================================

ItemPrefix ItemsFactory::RollItemPrefix()
{
  auto res = Util::WeightedRandom(_bucDistr);
  return res.first;
}

// =============================================================================

ItemQuality ItemsFactory::RollItemQuality()
{
  auto res = Util::WeightedRandom(_qualityDistr);
  return res.first;
}
