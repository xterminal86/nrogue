#ifndef TRADERCOMPONENT_H
#define TRADERCOMPONENT_H

#include "component.h"
#include "constants.h"

#include "game-object.h"

class AINPC;

class TraderComponent : public Component
{
  public:
    TraderComponent();

    void Update() override;

    void Init(TraderRole traderType, int stockRefreshTurns, int maxItems);
    void RefreshStock();

    std::vector<std::unique_ptr<GameObject>> Items;

    std::string ShopTitle;

    AINPC* NpcRef = nullptr;

    TraderRole Type();

  private:
    int _maxItems = 0;
    int _itemsToCreate = 0;
    int _stockResetCounter = 0;
    int _stockRefreshTurns = 0;

    TraderRole _traderType = TraderRole::NONE;

    void CreateItems();

    void CreateClericItems();
    void CreateCookItems();
    void CreateJunkerItems();
    void CreateBlacksmithItems();

    // =========================================================================

    const std::map<ItemType, int> _clericItemsWeights =
    {
      { ItemType::POTION,    4 },
      { ItemType::WAND,      4 },
      { ItemType::SCROLL,    4 },
      { ItemType::ACCESSORY, 3 },
      { ItemType::RETURNER,  3 }
    };

    const std::map<PotionType, int> _clericPotionWeights =
    {
      { PotionType::HEALING_POTION, 6 },
      { PotionType::MANA_POTION,    6 },
      { PotionType::NP_POTION,      3 },
      { PotionType::RA_POTION,      2 },
      { PotionType::CW_POTION,      2 }
    };

    const std::map<ItemPrefix, int> _clericPrefixWeights =
    {
      { ItemPrefix::BLESSED,  3 },
      { ItemPrefix::UNCURSED, 7 }
    };

    // -------------------------------------------------------------------------

    const std::map<FoodType, int> _cookItemsWeights =
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

    const std::map<ItemPrefix, int> _cookPrefixWeights =
    {
      { ItemPrefix::BLESSED,  1 },
      { ItemPrefix::UNCURSED, 6 }
    };

    // -------------------------------------------------------------------------

    const std::map<ItemType, int> _blacksmithItemsWeights =
    {
      { ItemType::WEAPON,     5 },
      { ItemType::ARROWS,     3 },
      { ItemType::ARMOR,      5 },
      { ItemType::REPAIR_KIT, 2 },
      { ItemType::ACCESSORY,  1 }
    };

    const std::map<ArmorType, int> _blacksmithArmorWeights =
    {
      { ArmorType::PADDING, 6 },
      { ArmorType::LEATHER, 5 },
      { ArmorType::MAIL,    3 },
      { ArmorType::PLATE,   1 }
    };

    const std::map<ItemPrefix, int> _blacksmithPrefixWeights =
    {
      { ItemPrefix::BLESSED,  1 },
      { ItemPrefix::UNCURSED, 6 }
    };

    // =========================================================================
};

#endif // TRADERCOMPONENT_H
