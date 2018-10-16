#ifndef GAMEOBJECTSFACTORY_H
#define GAMEOBJECTSFACTORY_H

#include "singleton.h"
#include "game-object.h"

class GameObjectsFactory : public Singleton<GameObjectsFactory>
{
  public:
    void Init();

    GameObject* CreateGameObject();
};

#endif // GAMEOBJECTSFACTORY_H
