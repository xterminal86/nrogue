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
    GameObject* CreateDummyItem(const std::string& objName, char image, const std::string& fgColor, const std::string& bgColor, const std::vector<std::string>& descText);
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
    GameObject* CreateHungerPotion(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateExpPotion(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateStatPotion(const std::string& statName, ItemPrefix prefixOverride = ItemPrefix::RANDOM);
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
    GameObject* CreateWeapon(int x, int y, WeaponType type, ItemPrefix preifx = ItemPrefix::RANDOM, ItemQuality quality = ItemQuality::RANDOM, const std::vector<ItemBonusStruct>& bonuses = std::vector<ItemBonusStruct>());
    GameObject* CreateRangedWeapon(int x, int y, RangedWeaponType type, ItemPrefix prefixOverride = ItemPrefix::RANDOM, ItemQuality quality = ItemQuality::RANDOM, const std::vector<ItemBonusStruct>& bonuses = std::vector<ItemBonusStruct>());
    GameObject* CreateArrows(int x, int y, ArrowType type, ItemPrefix prefixOverride = ItemPrefix::RANDOM, int amount = -1);
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
    GameObject* CreateRandomArmor(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    //
    // *************************************************************************
    //
    GameObject* CreateAccessory(int x, int y, EquipmentCategory category, const std::vector<ItemBonusStruct>& bonuses, ItemPrefix prefix, ItemQuality quality);
    GameObject* CreateRandomAccessory(int x, int y, ItemPrefix prefixOverride = ItemPrefix::RANDOM, bool atLeastOneBonus = false);
    //
    // *************************************************************************
    //
    GameObject* CreateRandomItem(int x, int y, ItemType exclude = ItemType::NOTHING);

    GameObject* CreateUniquePickaxe();

  protected:
    void InitSpecific() override;

  private:
    GameObject* CreateRandomGlass();
    GameObject* CreateGemHelper(GemType t, ItemQuality quality = ItemQuality::RANDOM);

    void AdjustBonusWeightsMapForItem(ItemComponent* itemRef,
                                      std::map<ItemBonusType, int>& bonusWeightByType);

    void TryToAddBonusesToItem(ItemComponent* itemRef,
                               bool atLeastOne = false);

    void AddRandomBonusToItem(ItemComponent* itemRef,
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

    bool ProcessItemEquiption(ItemComponent* item);
    bool ProcessRingEquiption(ItemComponent* item);

    void EquipItem(ItemComponent* item);
    void UnequipItem(ItemComponent* item);
    void EquipRing(ItemComponent* ring, int index);
    void UnequipRing(ItemComponent* ring, int index);

    int CalculateAverageDamage(int numRolls, int diceSides);

    ItemPrefix RollItemPrefix();
    ItemQuality RollItemQuality();

    size_t CalculateItemHash(ItemComponent* item);

    struct PotionInfo
    {
      ItemType PotionType = ItemType::DUMMY;
      std::string PotionName;
      std::pair<std::string, std::string> FgBgColor;
    };

    struct ScrollInfo
    {
      SpellType SpellType_ = SpellType::NONE;
      std::string ScrollName;
    };

    std::map<ItemType, PotionInfo> _gamePotionsMap;
    std::map<SpellType, ScrollInfo> _gameScrollsMap;

    Player* _playerRef;

    friend class GameObjectsFactory;
};

#endif // ITEMSFACTORY_H
