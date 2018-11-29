#include "trader-component.h"

#include "game-objects-factory.h"
#include "util.h"
#include "ai-npc.h"

TraderComponent::TraderComponent()
{
  _hash = typeid(*this).hash_code();
}

void TraderComponent::Init(TraderRole traderType)
{
  _traderType = traderType;
  RefreshStock();
}

void TraderComponent::RefreshStock()
{
  _itemsToCreate = RNG::Instance().RandomRange(8, 17);

  Items.clear();
  CreateItems();
}

void TraderComponent::Update()
{
  _stockResetCounter++;

  if (_stockResetCounter > 100)
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
        { ItemType::HEALING_POTION, 6 },
        { ItemType::MANA_POTION, 6 },
        { ItemType::HUNGER_POTION, 2 }
        //{ ItemType::SCROLL, 2 }
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

          case ItemType::HUNGER_POTION:
            go = GameObjectsFactory::Instance().CreateHungerPotion(prefixPair.first);
            break;
        }

        Items.push_back(std::unique_ptr<GameObject>(go));
      }
    }
    break;
  }
}
