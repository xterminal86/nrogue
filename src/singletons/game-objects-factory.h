#ifndef GAMEOBJECTSFACTORY_H
#define GAMEOBJECTSFACTORY_H

#include "singleton.h"
#include "constants.h"

class ItemComponent;
class ContainerComponent;
class GameObject;
class Player;
class MapLevelBase;

class GameObjectsFactory : public Singleton<GameObjectsFactory>
{
  public:    
    void Init() override;

    /// For random generation
    GameObject* CreateGameObject(int x, int y, ItemType objType);
    GameObject* CreateMonster(int x, int y, MonsterType monsterType);

    GameObject* CreateNPC(int x, int y, NPCType npcType, bool standing = false);
    GameObject* CreateShrine(int x, int y, ShrineType type, int timeout);
    GameObject* CreateMoney(int amount = 0);
    GameObject* CreateRemains(GameObject* from);
    GameObject* CreateHealingPotion(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateManaPotion(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateHungerPotion(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateExpPotion(ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateStatPotion(std::string statName, ItemPrefix prefixOverride = ItemPrefix::RANDOM);
    GameObject* CreateFood(int x, int y, FoodType type, ItemPrefix prefixOverride = ItemPrefix::RANDOM, bool isIdentified = false);
    GameObject* CreateNote(std::string objName, std::vector<std::string> text);
    GameObject* CreateWeapon(WeaponType type, ItemPrefix preifx = ItemPrefix::RANDOM);
    GameObject* CreateContainer(std::string name, chtype image, int x, int y);
    GameObject* CreateGem(int x, int y, GemType type = GemType::RANDOM);

    GameObject* CreateRandomPotion();
    GameObject* CreateRandomWeapon();

    GameObject* CreateRandomItem(int x, int y, ItemType exclude = ItemType::NOTHING);

    /// Creates stairs on MapArray of current level
    void CreateStairs(MapLevelBase* levelWhereCreate, int x, int y, chtype image, MapType leadsTo);

    bool HandleItemUse(ItemComponent* item);
    bool HandleItemEquip(ItemComponent* item);

    void GenerateLootIfPossible(int posX, int posY, MonsterType monsterType);

  private:
    Player* _playerRef;

    GameObject* CreateRat(int x, int y, bool randomize = true);
    GameObject* CreateBat(int x, int y, bool randomize = true);
    GameObject* CreateSpider(int x, int y, bool randomize = true);

    GameObject* CreateRandomGlass();
    GameObject* CreateGemHelper(GemType t);

    bool ProcessItemEquiption(ItemComponent* item);
    bool ProcessRingEquiption(ItemComponent* item);

    void EquipItem(ItemComponent* item);
    void UnequipItem(ItemComponent* item);
    void EquipRing(ItemComponent* ring, int index);
    void UnequipRing(ItemComponent* ring, int index);

    void HealingPotionUseHandler(ItemComponent* item);
    void ManaPotionUseHandler(ItemComponent* item);
    void HungerPotionUseHandler(ItemComponent* item);
    void ExpPotionUseHandler(ItemComponent* item);
    void StatPotionUseHandler(ItemComponent* item);

    void FoodUseHandler(ItemComponent* item);

    void SetItemName(GameObject* go, ItemData& itemData);
    void AdjustWeaponBonuses(ItemData& itemData);

    size_t CalculateHash(ItemComponent* item);

    int CalculateAverageDamage(int numRolls, int diceSides);

    ItemPrefix RollItemPrefix();

    void GenerateLoot(int posX, int posY, std::pair<ItemType, int> kvp, MonsterType type);
};

#endif // GAMEOBJECTSFACTORY_H