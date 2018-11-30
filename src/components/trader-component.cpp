#include "trader-component.h"

#include "game-objects-factory.h"
#include "util.h"
#include "ai-npc.h"

TraderComponent::TraderComponent()
{
  _hash = typeid(*this).hash_code();
}

void TraderComponent::Init(TraderRole traderType, int stockRefreshTurns)
{
  _traderType = traderType;
  _stockRefreshTurns = stockRefreshTurns;
  RefreshStock();
}

void TraderComponent::RefreshStock()
{
  _itemsToCreate = RNG::Instance().RandomRange(11, 20);

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
      ShopTitle = Util::StringFormat("%s's %s", npcName.data(), shopName.data());

      std::map<ItemType, int> itemsWeights =
      {
        { ItemType::HEALING_POTION, 1 },
        { ItemType::MANA_POTION, 1 }
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
        }

        Items.push_back(std::unique_ptr<GameObject>(go));
      }
    }
    break;

    case TraderRole::COOK:
    {
      std::string shopName = GlobalConstants::ShopNameByType.at(_traderType);
      std::string npcName = NpcRef->Data.Name;
      ShopTitle = Util::StringFormat("%s's %s", npcName.data(), shopName.data());

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
  }
}
