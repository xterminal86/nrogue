#ifndef ITEMSFACTORY_H
#define ITEMSFACTORY_H

#include "singleton.h"
#include "constants.h"
#include "item-data.h"

class GameObject;
class ItemComponent;
class Player;

class ItemsFactory : public Singleton<ItemsFactory>
{
  public:
    GameObject* CreateDummyItem(const std::string& objName,
                                char image,
                                const uint32_t& fgColor,
                                const uint32_t& bgColor,
                                const std::vector<std::string>& descText);
    //
    // *************************************************************************
    //
    GameObject* CreateMoney(int amount = 0);
    //
    // *************************************************************************
    //
    GameObject* CreatePotion(ItemType type, ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateHealingPotion(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateNeutralizePoisonPotion(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateManaPotion(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateJuicePotion(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateExpPotion(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateStatPotion(const std::string& statName, ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateCWPotion(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateRAPotion(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateRandomPotion();
    //
    // *************************************************************************
    //
    GameObject* CreateFood(int x, int y, FoodType type, ItemPrefix prefixOverride = ItemPrefix::RANDOM, bool isIdentified = false);
    //
    // *************************************************************************
    //
    GameObject* CreateNote(const std::string& objName, const std::vector<std::string>& text);
    GameObject* CreateScroll(int x, int y, SpellType type, ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateRandomScroll(ItemPrefix prefix = ItemPrefix::RANDOM);
    //
    // *************************************************************************
    //
    GameObject* CreateMeleeWeapon(int x, int y, WeaponType type, ItemPrefix preifx = ItemPrefix::RANDOM, ItemQuality quality = ItemQuality::RANDOM, const std::vector<ItemBonusStruct>& bonuses = std::vector<ItemBonusStruct>());
    GameObject* CreateRangedWeapon(int x, int y, RangedWeaponType type, ItemPrefix prefixOverride = ItemPrefix::RANDOM, ItemQuality quality = ItemQuality::RANDOM, const std::vector<ItemBonusStruct>& bonuses = std::vector<ItemBonusStruct>());
    GameObject* CreateArrows(int x, int y, ArrowType type, ItemPrefix prefixOverride = ItemPrefix::RANDOM, int amount = -1);
    GameObject* CreateRandomMeleeWeapon(WeaponType type = WeaponType::RANDOM, ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateRandomRangedWeapon(RangedWeaponType type = RangedWeaponType::RANDOM, ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateRandomWeapon(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    //
    // *************************************************************************
    //
    GameObject* CreateWand(int x, int y, WandMaterials material, SpellType spellType, ItemPrefix prefixOverride = ItemPrefix::RANDOM, ItemQuality quality = ItemQuality::RANDOM);
    GameObject* CreateRandomWand(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    //
    // *************************************************************************
    //
    GameObject* CreateGem(int x, int y, GemType type = GemType::RANDOM, int actualGemChance = -1);
    GameObject* CreateReturner(int x, int y, int charges = -1, ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    //
    // *************************************************************************
    //
    GameObject* CreateRepairKit(int x, int y, int charges = -1, ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    //
    // *************************************************************************
    //
    GameObject* CreateArmor(int x, int y, ArmorType type, ItemPrefix prefixOverride = ItemPrefix::RANDOM, ItemQuality quality = ItemQuality::RANDOM);
    GameObject* CreateRandomArmor(ArmorType type = ArmorType::RANDOM, ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    //
    // *************************************************************************
    //
    GameObject* CreateAccessory(int x, int y, EquipmentCategory category, const std::vector<ItemBonusStruct>& bonuses, ItemPrefix prefix, ItemQuality quality);
    GameObject* CreateRandomAccessory(int x, int y, ItemPrefix prefixOverride = ItemPrefix::RANDOM, bool atLeastOneBonus = false);
    //
    // *************************************************************************
    //
    GameObject* CreateRandomItem(int x, int y, const std::vector<ItemType>& itemsToExclude = std::vector<ItemType>());

    // **************************** Uniques ************************************
    //
    // TODO:
    //
    // DAGGER,
    GameObject* CreateNeedleShortSword();
    // ARMING_SWORD,
    // LONG_SWORD,
    // GREAT_SWORD,
    // STAFF,
    GameObject* CreateBlockBreakerPickaxe();
    GameObject* CreateOneRing();

  protected:
    void InitSpecific() override;

  private:
    GameObject* CreateRandomGlass();
    GameObject* CreateGemHelper(GemType t, ItemQuality quality = ItemQuality::RANDOM);
    GameObject* ChooseRandomMeleeWeapon(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* ChooseRandomRangedWeapon(ItemPrefix prefixOverride = ItemPrefix::RANDOM);

    void AdjustBonusWeightsMapForItem(ItemComponent* itemRef,
                                      std::map<ItemBonusType, int>& bonusWeightByType);

    void TryToAddBonusesToItem(ItemComponent* itemRef,
                               bool atLeastOne = false);

    void AddRandomValueBonusToItem(ItemComponent* itemRef,
                                   ItemBonusType bonusType);

    void AddBonusToItem(ItemComponent* itemRef,
                        const ItemBonusStruct& bonusData,
                        bool forceAdd = false);

    void InitPotionColors();
    void InitScrolls();

    void SetPotionImage(GameObject* go);

    void SetItemName(GameObject* go, ItemData& itemData);

    void SetMagicItemName(ItemComponent* itemRef,
                          const std::vector<ItemBonusType>& bonusesRolled);

    void BUCQualityAdjust(ItemData& itemData);

    int CalculateAverageDamage(int numRolls, int diceSides);

    ItemPrefix RollItemPrefix();
    ItemQuality RollItemQuality();

    struct PotionInfo
    {
      ItemType PotionType = ItemType::DUMMY;
      std::string PotionName;
      std::pair<uint32_t, uint32_t> FgBgColor;
    };

    struct ScrollInfo
    {
      SpellType SpellType_ = SpellType::NONE;
      std::string ScrollName;
    };

    std::map<ItemType, PotionInfo> _gamePotionsMap;
    std::map<SpellType, ScrollInfo> _gameScrollsMap;

    Player* _playerRef = nullptr;

    // =========================================================================

    const std::map<WandMaterials, int> _wandMaterialsDistribution =
    {
      { WandMaterials::YEW_1,    80 },
      { WandMaterials::IVORY_2,  60 },
      { WandMaterials::EBONY_3,  45 },
      { WandMaterials::ONYX_4,   30 },
      { WandMaterials::GLASS_5,  20 },
      { WandMaterials::COPPER_6, 15 },
      { WandMaterials::GOLDEN_7, 10 },
    };

    const std::map<SpellType, int> _spellsDistribution =
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

    const std::map<FoodType, int> _foodMap =
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

    const std::map<GemType, int> _gemsMap =
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

    const std::map<ItemType, int> _returnerMap =
    {
      { ItemType::RETURNER, 1 },
      { ItemType::NOTHING,  4 }
    };

    const std::map<ItemQuality, int> _chanceModByQ =
    {
      { ItemQuality::DAMAGED,    -10 },
      { ItemQuality::FLAWED,      -5 },
      { ItemQuality::NORMAL,       0 },
      { ItemQuality::FINE,         5 },
      { ItemQuality::EXCEPTIONAL, 10 },
    };

    const std::map<ItemQuality, int> _multByQ =
    {
      { ItemQuality::DAMAGED,      1 },
      { ItemQuality::FLAWED,       2 },
      { ItemQuality::NORMAL,       3 },
      { ItemQuality::FINE,         4 },
      { ItemQuality::EXCEPTIONAL,  5 },
    };

    // Probability of stat increase values
    const std::map<int, int> _statIncreaseWeightsMap =
    {
      { 1, 100 },
      { 2,  75 },
      { 3,  50 },
      { 4,  25 },
      { 5,  12 }
    };

    const std::map<ItemPrefix, int> _bucDistr =
    {
      { ItemPrefix::UNCURSED, 4 },
      { ItemPrefix::CURSED,   4 },
      { ItemPrefix::BLESSED,  1 }
    };

    const std::map<ItemQuality, int> _qualityDistr =
    {
      { ItemQuality::DAMAGED,     8 },
      { ItemQuality::FLAWED,      6 },
      { ItemQuality::NORMAL,      4 },
      { ItemQuality::FINE,        3 },
      { ItemQuality::EXCEPTIONAL, 2 },
    };

    // =========================================================================

    friend class GameObjectsFactory;
};

#endif // ITEMSFACTORY_H
