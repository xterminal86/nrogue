#include "trader-component.h"

#include "items-factory.h"
#include "util.h"
#include "ai-npc.h"

TraderComponent::TraderComponent()
{
  _componentHash = typeid(*this).hash_code();
}

void TraderComponent::Init(TraderRole traderType, int stockRefreshTurns, int maxItems)
{
  _traderType = traderType;
  _stockRefreshTurns = stockRefreshTurns;
  _maxItems = maxItems;

  RefreshStock();
}

void TraderComponent::RefreshStock()
{
  int min = _maxItems / 2;
  if (min == 0)
  {
    min = 1;
  }

  _itemsToCreate = RNG::Instance().RandomRange(min, _maxItems + 1);

  Items.clear();

  CreateItems();
}

void TraderComponent::Update()
{
  _stockResetCounter++;

  if (_stockResetCounter > _stockRefreshTurns)
  {
    RefreshStock();
    _stockResetCounter = 0;
  }
}

void TraderComponent::CreateItems()
{
  switch (_traderType)
  {
    case TraderRole::CLERIC:
      CreateClericItems();
      break;

    case TraderRole::COOK:
      CreateCookItems();
      break;

    case TraderRole::JUNKER:
      CreateJunkerItems();
      break;

    case TraderRole::BLACKSMITH:
      CreateBlacksmithItems();
      break;
  }
}

void TraderComponent::CreateClericItems()
{
  std::string shopName = GlobalConstants::ShopNameByType.at(_traderType);
  std::string npcName = NpcRef->Data.Name;
  ShopTitle = Util::StringFormat(" %s's %s ", npcName.data(), shopName.data());

  std::map<ItemType, int> itemsWeights =
  {
    { ItemType::HEALING_POTION, 10 },
    { ItemType::MANA_POTION,    10 },
    { ItemType::NP_POTION,      10 },
    { ItemType::RA_POTION,       3 },
    { ItemType::CW_POTION,       3 },
    { ItemType::WAND,            4 },
    { ItemType::SCROLL,          4 },
    { ItemType::ACCESSORY,       3 },
    { ItemType::RETURNER,        3 }
  };

  std::map<ItemPrefix, int> prefixWeights =
  {
    { ItemPrefix::BLESSED,  3 },
    { ItemPrefix::UNCURSED, 7 }
  };

  for (int i = 0; i < _itemsToCreate; i++)
  {
    auto itemPair   = Util::WeightedRandom(itemsWeights);
    auto prefixPair = Util::WeightedRandom(prefixWeights);

    GameObject* go = nullptr;

    switch (itemPair.first)
    {
      case ItemType::HEALING_POTION:
        go = ItemsFactory::Instance().CreateHealingPotion(prefixPair.first);
        break;

      case ItemType::MANA_POTION:
        go = ItemsFactory::Instance().CreateManaPotion(prefixPair.first);
        break;

      case ItemType::NP_POTION:
        go = ItemsFactory::Instance().CreateNeutralizePoisonPotion(prefixPair.first);
        break;

      case ItemType::CW_POTION:
        go = ItemsFactory::Instance().CreateCWPotion(prefixPair.first);
        break;

      case ItemType::RA_POTION:
        go = ItemsFactory::Instance().CreateRAPotion(prefixPair.first);
        break;

      case ItemType::RETURNER:
        go = ItemsFactory::Instance().CreateReturner(0, 0, -1, prefixPair.first);
        break;

      case ItemType::WAND:
        go = ItemsFactory::Instance().CreateRandomWand(prefixPair.first);
        break;

      case ItemType::SCROLL:
        go = ItemsFactory::Instance().CreateRandomScroll(prefixPair.first);
        break;

      case ItemType::ACCESSORY:
        go = ItemsFactory::Instance().CreateRandomAccessory(0, 0, prefixPair.first, true);
        break;
    }

    Items.push_back(std::unique_ptr<GameObject>(go));
  }
}

void TraderComponent::CreateCookItems()
{
  std::string shopName = GlobalConstants::ShopNameByType.at(_traderType);
  std::string npcName = NpcRef->Data.Name;
  ShopTitle = Util::StringFormat(" %s's %s ", npcName.data(), shopName.data());

  std::map<FoodType, int> itemsWeights =
  {
    { FoodType::APPLE,        10 },
    { FoodType::CHEESE,        8 },
    { FoodType::BREAD,         9 },
    { FoodType::FISH,          8 },
    { FoodType::PIE,           6 },
    { FoodType::MEAT,          7 },
    { FoodType::TIN,           6 },
    { FoodType::RATIONS,       5 },
    { FoodType::IRON_RATIONS,  3 }
  };

  std::map<ItemPrefix, int> prefixWeights =
  {
    { ItemPrefix::BLESSED,  1 },
    { ItemPrefix::UNCURSED, 6 }
  };

  for (int i = 0; i < _itemsToCreate; i++)
  {
    auto itemPair   = Util::WeightedRandom(itemsWeights);
    auto prefixPair = Util::WeightedRandom(prefixWeights);

    GameObject* go = ItemsFactory::Instance().CreateFood(0, 0, itemPair.first, prefixPair.first, true);

    Items.push_back(std::unique_ptr<GameObject>(go));
  }
}

void TraderComponent::CreateJunkerItems()
{
  std::string shopName = GlobalConstants::ShopNameByType.at(_traderType);
  std::string npcName = NpcRef->Data.Name;
  ShopTitle = Util::StringFormat(" %s's %s ", npcName.data(), shopName.data());

  for (int i = 0; i < _itemsToCreate; i++)
  {
    GameObject* go = ItemsFactory::Instance().CreateRandomItem(0, 0, { ItemType::COINS, ItemType::FOOD });
    if (go != nullptr)
    {
      Items.push_back(std::unique_ptr<GameObject>(go));
    }
  }
}

void TraderComponent::CreateBlacksmithItems()
{
  std::string shopName = GlobalConstants::ShopNameByType.at(_traderType);
  std::string npcName = NpcRef->Data.Name;
  ShopTitle = Util::StringFormat(" %s's %s ", npcName.data(), shopName.data());

  std::map<ItemType, int> itemsWeights =
  {
    { ItemType::WEAPON,     5 },
    { ItemType::ARROWS,     3 },
    { ItemType::ARMOR,      5 },
    { ItemType::REPAIR_KIT, 2 },
    { ItemType::ACCESSORY,  1 }
  };

  std::map<ArmorType, int> armorWeights =
  {
    { ArmorType::PADDING, 6 },
    { ArmorType::LEATHER, 5 },
    { ArmorType::MAIL,    3 },
    { ArmorType::PLATE,   1 }
  };

  std::map<ItemPrefix, int> prefixWeights =
  {
    { ItemPrefix::BLESSED,  1 },
    { ItemPrefix::UNCURSED, 6 }
  };

  for (int i = 0; i < _itemsToCreate; i++)
  {
    auto itemPair   = Util::WeightedRandom(itemsWeights);
    auto prefixPair = Util::WeightedRandom(prefixWeights);

    GameObject* go = nullptr;

    switch (itemPair.first)
    {
      case ItemType::WEAPON:
        go = ItemsFactory::Instance().CreateRandomWeapon(prefixPair.first);
        break;

      case ItemType::REPAIR_KIT:
        go = ItemsFactory::Instance().CreateRepairKit(0, 0, 30, prefixPair.first);
        break;

      case ItemType::ARMOR:
      {
        auto armorPair = Util::WeightedRandom(armorWeights);
        go = ItemsFactory::Instance().CreateArmor(0, 0, armorPair.first, prefixPair.first);
      }
      break;

      case ItemType::ARROWS:
      {
        int flag = RNG::Instance().RandomRange(0, 2);
        ArrowType arrowsType = (flag == 0) ? ArrowType::ARROWS : ArrowType::BOLTS;
        int amount = RNG::Instance().RandomRange(10, 21);
        go = ItemsFactory::Instance().CreateArrows(0, 0, arrowsType, prefixPair.first, amount);
      }
      break;

      case ItemType::ACCESSORY:
        go = ItemsFactory::Instance().CreateRandomAccessory(0, 0, prefixPair.first, true);
        break;
    }

    if (go != nullptr)
    {
      Items.push_back(std::unique_ptr<GameObject>(go));
    }
  }
}

TraderRole TraderComponent::Type()
{
  return _traderType;
}
