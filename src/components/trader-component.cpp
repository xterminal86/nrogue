#include "trader-component.h"

#include "game-objects-factory.h"
#include "util.h"
#include "ai-npc.h"

TraderComponent::TraderComponent()
{
  _hash = typeid(*this).hash_code();
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
    {
      std::string shopName = GlobalConstants::ShopNameByType.at(_traderType);
      std::string npcName = NpcRef->Data.Name;
      ShopTitle = Util::StringFormat(" %s's %s ", npcName.data(), shopName.data());

      std::map<ItemType, int> itemsWeights =
      {
        { ItemType::HEALING_POTION, 10 },
        { ItemType::MANA_POTION,    10 },
        { ItemType::WAND,            3 },
        { ItemType::RETURNER,        1 }
      };

      std::map<ItemPrefix, int> prefixWeights =
      {
        { ItemPrefix::BLESSED, 1 },
        { ItemPrefix::UNCURSED, 6 }
      };

      for (int i = 0; i < _itemsToCreate; i++)
      {
        auto itemPair = Util::WeightedRandom(itemsWeights);
        auto prefixPair = Util::WeightedRandom(prefixWeights);

        GameObject* go = nullptr;

        switch (itemPair.first)
        {
          case ItemType::HEALING_POTION:
            go = GameObjectsFactory::Instance().CreateHealingPotion(prefixPair.first);
            break;

          case ItemType::MANA_POTION:
            go = GameObjectsFactory::Instance().CreateManaPotion(prefixPair.first);
            break;

          case ItemType::RETURNER:
            go = GameObjectsFactory::Instance().CreateReturner(0, 0, -1, prefixPair.first);
            break;

          case ItemType::WAND:
            go = GameObjectsFactory::Instance().CreateRandomWand(prefixPair.first);
            break;
        }

        Items.push_back(std::unique_ptr<GameObject>(go));
      }
    }
    break;

    case TraderRole::COOK:
    {
      std::string shopName = GlobalConstants::ShopNameByType.at(_traderType);
      std::string npcName = NpcRef->Data.Name;
      ShopTitle = Util::StringFormat(" %s's %s ", npcName.data(), shopName.data());

      std::map<FoodType, int> itemsWeights =
      {
        { FoodType::APPLE, 5 },
        { FoodType::BREAD, 6 },
        { FoodType::CHEESE, 5 },
        { FoodType::IRON_RATIONS, 1 },
        { FoodType::MEAT, 4 },
        { FoodType::PIE, 4 },
        { FoodType::RATIONS, 2 }
      };

      std::map<ItemPrefix, int> prefixWeights =
      {
        { ItemPrefix::BLESSED, 1 },
        { ItemPrefix::UNCURSED, 6 }
      };

      for (int i = 0; i < _itemsToCreate; i++)
      {
        auto itemPair = Util::WeightedRandom(itemsWeights);
        auto prefixPair = Util::WeightedRandom(prefixWeights);

        GameObject* go = GameObjectsFactory::Instance().CreateFood(0, 0, itemPair.first, prefixPair.first, true);

        Items.push_back(std::unique_ptr<GameObject>(go));
      }
    }
    break;

    case TraderRole::JUNKER:
    {
      std::string shopName = GlobalConstants::ShopNameByType.at(_traderType);
      std::string npcName = NpcRef->Data.Name;
      ShopTitle = Util::StringFormat(" %s's %s ", npcName.data(), shopName.data());

      for (int i = 0; i < _itemsToCreate; i++)
      {
        GameObject* go = GameObjectsFactory::Instance().CreateRandomItem(0, 0, ItemType::COINS);
        if (go != nullptr)
        {
          Items.push_back(std::unique_ptr<GameObject>(go));
        }
      }
    }
    break;

    case TraderRole::BLACKSMITH:
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
        auto itemPair = Util::WeightedRandom(itemsWeights);
        auto prefixPair = Util::WeightedRandom(prefixWeights);

        GameObject* go = nullptr;

        switch (itemPair.first)
        {
          case ItemType::WEAPON:
            go = GameObjectsFactory::Instance().CreateRandomWeapon(prefixPair.first);
            break;

          case ItemType::REPAIR_KIT:
            go = GameObjectsFactory::Instance().CreateRepairKit(0, 0, -1, prefixPair.first);
            break;

          case ItemType::ARMOR:
          {
            auto armorPair = Util::WeightedRandom(armorWeights);
            go = GameObjectsFactory::Instance().CreateArmor(armorPair.first, prefixPair.first);
          }
          break;

          case ItemType::ARROWS:
          {
            int flag = RNG::Instance().RandomRange(0, 2);
            ArrowType arrowsType = (flag == 0) ? ArrowType::ARROWS : ArrowType::BOLTS;
            go = GameObjectsFactory::Instance().CreateArrows(0, 0, arrowsType, prefixPair.first);
          }
          break;
        }

        if (go != nullptr)
        {
          Items.push_back(std::unique_ptr<GameObject>(go));
        }
      }
    }
    break;
  }
}
