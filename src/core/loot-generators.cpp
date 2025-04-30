#include "loot-generators.h"

#include "constants.h"
#include "util.h"
#include "items-factory.h"
#include "map.h"

namespace LootGenerators
{
  void DropLoot(GameObject *go)
  {
    if (go == nullptr)
    {
      DebugLog("DropLoot() - go is nullptr!");
      return;
    }

    switch(go->Type)
    {
      case GameObjectType::RAT:
        Rat(go);
        break;

      case GameObjectType::MAD_MINER:
        MadMiner(go);
        break;

      case GameObjectType::SHELOB:
        Shelob(go);
        break;

      case GameObjectType::KOBOLD:
        Kobold(go);
        break;

      default:
        DebugLog("No loot function specified for object type %d",
                 (int)go->Type);
        break;
    }
  }

  // ===========================================================================

  void Rat(GameObject* go)
  {
    const std::unordered_map<ItemType, int> lootTable =
    {
      { ItemType::FOOD,     4 },
      { ItemType::NOTHING, 20 }
    };

    auto kvp = Util::WeightedRandom(lootTable);
    switch (kvp.first)
    {
      case ItemType::FOOD:
      {
        auto food = ItemsFactory::Instance().CreateFood(go->PosX,
                                                        go->PosY,
                                                        FoodType::CHEESE);
        Map::Instance().PlaceGameObject(food);
      }
      break;

      default:
        break;
    }
  }

  // ===========================================================================

  void MadMiner(GameObject* go)
  {
    const std::unordered_map<ItemType, int> lootTable =
    {
      { ItemType::FOOD,    15 },
      { ItemType::COINS,   15 },
      { ItemType::GEM,      5 },
      { ItemType::NOTHING, 45 }
    };

    auto kvp = Util::WeightedRandom(lootTable);
    switch (kvp.first)
    {
      case ItemType::FOOD:
      {
        const std::unordered_map<FoodType, int> foodTable =
        {
          { FoodType::BREAD,  20 },
          { FoodType::CHEESE, 20 },
          { FoodType::RATIONS, 5 },
          { FoodType::MEAT,    8 },
        };
        auto f = Util::WeightedRandom(foodTable);

        auto food = ItemsFactory::Instance().CreateFood(go->PosX,
                                                        go->PosY,
                                                        f.first);
        Map::Instance().PlaceGameObject(food);
      }
      break;

      case ItemType::COINS:
      {
        auto coins = ItemsFactory::Instance().CreateMoney();
        coins->PosX = go->PosX;
        coins->PosY = go->PosY;
        Map::Instance().PlaceGameObject(coins);
      }
      break;

      case ItemType::GEM:
      {
        auto gem = ItemsFactory::Instance().CreateGem(go->PosX, go->PosY);
        Map::Instance().PlaceGameObject(gem);
      }
      break;

      default:
        break;
    }
  }

  // ===========================================================================

  void Shelob(GameObject* go)
  {
    // TODO:
  }

  // ===========================================================================

  void Kobold(GameObject* go)
  {
    // TODO:
  }
}
