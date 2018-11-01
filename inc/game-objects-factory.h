#ifndef GAMEOBJECTSFACTORY_H
#define GAMEOBJECTSFACTORY_H

#include "singleton.h"
#include "game-object.h"
#include "constants.h"
#include "player.h"
#include "item-component.h"

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
    GameObject* CreateHealingPotion();
    GameObject* CreateManaPotion();
    GameObject* CreateRandomPotion();

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

    void SetItemName(GameObject* go, ItemData& itemData);
};

#endif // GAMEOBJECTSFACTORY_H
