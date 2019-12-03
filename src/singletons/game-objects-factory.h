#ifndef GAMEOBJECTSFACTORY_H
#define GAMEOBJECTSFACTORY_H

#include "singleton.h"
#include "item-data.h"

class GameObjectInfo;
class ItemComponent;
class ContainerComponent;
class GameObject;
class Player;
class MapLevelBase;
class DoorComponent;

class GameObjectsFactory : public Singleton<GameObjectsFactory>
{
  public:    
    void Init() override;

    /// For random generation
    GameObject* CreateGameObject(int x, int y, ItemType objType);
    GameObject* CreateMonster(int x, int y, GameObjectType monsterType);

    /// Various
    GameObject* CreateNPC(int x, int y, NPCType npcType, bool standing = false);
    GameObject* CreateShrine(int x, int y, ShrineType type, int timeout);
    GameObject* CreateMoney(int amount = 0);
    GameObject* CreateRemains(GameObject* from);
    GameObject* CreateHealingPotion(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateManaPotion(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateHungerPotion(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateExpPotion(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateStatPotion(const std::string& statName, ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateRandomPotion();
    GameObject* CreateFood(int x, int y, FoodType type, ItemPrefix prefixOverride = ItemPrefix::RANDOM, bool isIdentified = false);
    GameObject* CreateNote(const std::string& objName, const std::vector<std::string>& text);
    GameObject* CreateDummyObject(const std::string& objName, char image, const std::string& fgColor, const std::string& bgColor, const std::vector<std::string>& descText);
    GameObject* CreateScroll(int x, int y, SpellType type, ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateRandomScroll(ItemPrefix prefix = ItemPrefix::RANDOM);
    GameObject* CreateWeapon(int x, int y, WeaponType type, ItemPrefix preifx = ItemPrefix::RANDOM, ItemQuality quality = ItemQuality::RANDOM);
    GameObject* CreateRandomWeapon(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateContainer(const std::string& name, const std::string& bgColor, int image, int x, int y);
    GameObject* CreateGem(int x, int y, GemType type = GemType::RANDOM, int gemChance = -1);
    GameObject* CreateWand(int x, int y, WandMaterials material, SpellType spellType, ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateRandomWand(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateReturner(int x, int y, int charges = -1, ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateRepairKit(int x, int y, int charges = -1, ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateArmor(int x, int y, ArmorType type, ItemPrefix prefixOverride = ItemPrefix::RANDOM, ItemQuality quality = ItemQuality::RANDOM);
    GameObject* CreateRandomArmor(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateArrows(int x, int y, ArrowType type, ItemPrefix prefixOverride = ItemPrefix::RANDOM, int amount = -1);
    GameObject* CreateRangedWeapon(int x, int y, RangedWeaponType type, ItemPrefix prefixOverride = ItemPrefix::RANDOM, ItemQuality quality = ItemQuality::RANDOM);
    GameObject* CreateRandomAccessory(int x, int y, ItemPrefix prefixOverride = ItemPrefix::RANDOM, bool atLeastOneBonus = false);
    GameObject* CreateAccessory(int x, int y, EquipmentCategory category, const std::vector<ItemBonusStruct>& bonuses, ItemPrefix prefix, ItemQuality quality);
    GameObject* CreateBreakableObjectWithRandomLoot(int x, int y, char image, const std::string& objName, const std::string& fgColor, const std::string& bgColor);

    GameObject* CreateRandomItem(int x, int y, ItemType exclude = ItemType::NOTHING);

    /// Creates stairs on MapArray of current level
    void CreateStairs(MapLevelBase* levelWhereCreate, int x, int y, int image, MapType leadsTo);

    bool HandleItemUse(ItemComponent* item);
    bool HandleItemEquip(ItemComponent* item);

    void GenerateLootIfPossible(int posX, int posY, GameObjectType monsterType);

    GameObject* CreateStaticObject(int x, int y, const GameObjectInfo& objectInfo, int hitPoints = -1, GameObjectType type = GameObjectType::HARMLESS);
    GameObject* CreateDoor(int x, int y,
                           bool isOpen,
                           const std::string& doorName,
                           int hitPoints = -1,
                           const std::string& fgOverrideColor = std::string(),
                           const std::string& bgOverrideColor = std::string());

    ItemComponent* CloneItem(ItemComponent* copyFrom);
    GameObject* CloneObject(GameObject* copyFrom);

  private:
    Player* _playerRef;

    GameObject* CreateRat(int x, int y, bool randomize = true);
    GameObject* CreateBat(int x, int y, bool randomize = true);
    GameObject* CreateSpider(int x, int y, bool randomize = true);
    GameObject* CreateHerobrine(int x, int y);
    GameObject* CreateUniquePickaxe();

    GameObject* CreateRandomGlass();
    GameObject* CreateGemHelper(GemType t, ItemQuality quality = ItemQuality::RANDOM);

    void AdjustBonusWeightsMap(ItemComponent* itemRef, std::map<ItemBonusType, int>& bonusWeightByType);
    void TryToAddBonuses(ItemComponent* itemRef, bool atLeastOne = false);
    void AddRandomBonus(ItemComponent* itemRef, ItemBonusType bonusType);
    void AddBonus(ItemComponent* itemRef, const ItemBonusStruct& bonusData, bool forceAdd = false);

    bool ProcessItemEquiption(ItemComponent* item);
    bool ProcessRingEquiption(ItemComponent* item);

    void EquipItem(ItemComponent* item);
    void UnequipItem(ItemComponent* item);
    void EquipRing(ItemComponent* ring, int index);
    void UnequipRing(ItemComponent* ring, int index);

    bool HealingPotionUseHandler(ItemComponent* item);
    bool ManaPotionUseHandler(ItemComponent* item);
    bool HungerPotionUseHandler(ItemComponent* item);
    bool ExpPotionUseHandler(ItemComponent* item);
    bool StatPotionUseHandler(ItemComponent* item);
    bool ReturnerUseHandler(ItemComponent* item);
    bool RepairKitUseHandler(ItemComponent* item);
    bool ScrollUseHandler(ItemComponent* item);

    bool FoodUseHandler(ItemComponent* item);

    void DoorUseHandler(DoorComponent* dc);

    void SetItemName(GameObject* go, ItemData& itemData);
    void SetMagicItemName(ItemComponent* itemRef, const std::vector<ItemBonusType>& bonusesRolled);
    void AdjustWeaponBonuses(ItemData& itemData);

    size_t CalculateItemHash(ItemComponent* item);

    int CalculateAverageDamage(int numRolls, int diceSides);

    ItemPrefix RollItemPrefix();
    ItemQuality RollItemQuality();

    void GenerateLoot(int posX, int posY, const std::pair<ItemType, int>& kvp, GameObjectType type);

    void InitPotionColors();
    void InitScrolls();

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
};

#endif // GAMEOBJECTSFACTORY_H
