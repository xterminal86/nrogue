#include "loot-generators.h"

#include "constants.h"
#include "util.h"
#include "items-factory.h"
#include "map.h"

namespace LootGenerators
{
  void Rat(GameObject* go)
  {
    const std::map<ItemType, int> lootTable =
    {
      { ItemType::FOOD,     4 },
      { ItemType::NOTHING, 20 }
    };

    auto kvp = Util::WeightedRandom(lootTable);
    switch (kvp.first)
    {
      case ItemType::FOOD:
      {
        auto food = ItemsFactory::Instance().CreateFood(go->PosX, go->PosY, FoodType::CHEESE);
        Map::Instance().InsertGameObject(food);
      }
      break;

      default:
        break;
    }
  }

  void MadMiner(GameObject* go)
  {
    const std::map<ItemType, int> lootTable =
    {
      { ItemType::FOOD,    15 },
      { ItemType::COINS,   15 },
      { ItemType::WEAPON,  10 },
      { ItemType::GEM,      5 },
      { ItemType::NOTHING, 45 }
    };

    auto kvp = Util::WeightedRandom(lootTable);
    switch (kvp.first)
    {
      case ItemType::FOOD:
      {
        const std::map<FoodType, int> foodTable =
        {
          { FoodType::BREAD,  20 },
          { FoodType::CHEESE, 20 },
          { FoodType::RATIONS, 5 },
          { FoodType::MEAT,    8 },
        };
        auto f = Util::WeightedRandom(foodTable);

        auto food = ItemsFactory::Instance().CreateFood(go->PosX, go->PosY, f.first);
        Map::Instance().InsertGameObject(food);
      }
      break;

      case ItemType::COINS:
      {
        auto coins = ItemsFactory::Instance().CreateMoney();
        coins->PosX = go->PosX;
        coins->PosY = go->PosY;
        Map::Instance().InsertGameObject(coins);
      }
      break;

      case ItemType::WEAPON:
      {
        auto w = ItemsFactory::Instance().CreateWeapon(go->PosX, go->PosY, WeaponType::PICKAXE);
        Map::Instance().InsertGameObject(w);
      }
      break;

      case ItemType::GEM:
      {
        auto gem = ItemsFactory::Instance().CreateGem(go->PosX, go->PosY);
        Map::Instance().InsertGameObject(gem);
      }
      break;

      default:
        break;
    }
  }

  void Shelob(GameObject* go)
  {
  }
}
