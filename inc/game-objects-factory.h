#ifndef GAMEOBJECTSFACTORY_H
#define GAMEOBJECTSFACTORY_H

#include "singleton.h"
#include "game-object.h"
#include "constants.h"

class GameObjectsFactory : public Singleton<GameObjectsFactory>
{
  public:    
    /// For random generation
    GameObject* CreateGameObject(ItemType objType);

    GameObject* CreateMoney(int amount = 0);
};

#endif // GAMEOBJECTSFACTORY_H
