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

    GameObject* CreateRat(int x, int y, bool randomize = true);
    GameObject* CreateShrine(int x, int y, ShrineType type, int timeout);    
    GameObject* CreateMoney(int amount = 0);
    GameObject* CreateRemains(GameObject* from);
    GameObject* CreateHealingPotion(ItemPrefix prefixOverride = ItemPrefix::UNCURSED);
    GameObject* CreateManaPotion(ItemPrefix prefixOverride = ItemPrefix::UNCURSED);
    GameObject* CreateHungerPotion(ItemPrefix prefixOverride = ItemPrefix::UNCURSED);
    GameObject* CreateExpPotion(ItemPrefix prefixOverride = ItemPrefix::UNCURSED);
    GameObject* CreateFood(FoodType type = FoodType::APPLE, ItemPrefix prefixOverride = ItemPrefix::UNCURSED);
    GameObject* CreateNote(std::string objName, std::vector<std::string> text);
    GameObject* CreateWeapon(WeaponType type, bool overridePrefix = false);
    GameObject* CreateContainer(std::string name, chtype image, int x, int y);

    GameObject* CreateRandomPotion();

    /// Creates stairs on MapArray of current level
    void CreateStairs(MapLevelBase* levelWhereCreate, int x, int y, chtype image, MapType leadsTo);

    bool HandleItemUse(ItemComponent* item);
    bool HandleItemEquip(ItemComponent* item);

  private:
    Player* _playerRef;

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
    void FoodUseHandler(ItemComponent* item);

    void SetItemName(GameObject* go, ItemData& itemData);
    void AdjustItemBonuses(ItemData& itemData);

    void SetStatsModifiers(ItemData& itemData);
    void UnsetStatsModifiers(ItemData& itemData);

    size_t CalculateHash(ItemComponent* item);

    ItemPrefix RollItemPrefix();
};

#endif // GAMEOBJECTSFACTORY_H
