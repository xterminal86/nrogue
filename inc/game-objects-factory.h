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

    bool HandleItemUse(ItemComponent* item);

  private:
    Player* _playerRef;
};

#endif // GAMEOBJECTSFACTORY_H
