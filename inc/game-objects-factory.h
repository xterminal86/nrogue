#ifndef GAMEOBJECTSFACTORY_H
#define GAMEOBJECTSFACTORY_H

#include "singleton.h"
#include "game-object.h"
#include "constants.h"

class GameObjectsFactory : public Singleton<GameObjectsFactory>
{
  public:    
    /// For random generation
    GameObject* CreateGameObject(int x, int y, ItemType objType);
    GameObject* CreateMonster(int x, int y, MonsterType monsterType);

    GameObject* CreateRat(int x, int y, bool randomize = true);
    GameObject* CreateShrine(int x, int y, ShrineType type, int timeout);
    GameObject* CreateMoney(int amount = 0);
    GameObject* CreateRemains(GameObject* from);
};

#endif // GAMEOBJECTSFACTORY_H
