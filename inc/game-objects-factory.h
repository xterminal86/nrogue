#ifndef GAMEOBJECTSFACTORY_H
#define GAMEOBJECTSFACTORY_H

#include "singleton.h"
#include "game-object.h"

class GameObjectsFactory : public Singleton<GameObjectsFactory>
{
  public:
    enum ObjectType
    {
      LOOT = 0,
      FOOD,
      SCROLL
    };

    GameObject* CreateGameObject(ObjectType objType);
    GameObject* CreateMoney(int amount = 0);
};

#endif // GAMEOBJECTSFACTORY_H
