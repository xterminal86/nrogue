#include "game-objects-factory.h"

#include "constants.h"
#include "rng.h"
#include "item-component.h"

GameObject* GameObjectsFactory::CreateGameObject(ObjectType objType)
{
  GameObject* go = nullptr;

  switch (objType)
  {
    case LOOT:
      go = CreateMoney();
      break;

    default:
      break;
  }

  return go;
}

GameObject* GameObjectsFactory::CreateMoney(int amount)
{
  GameObject* go = new GameObject();

  go->ObjectName = "Some gold coins";

  Component* c = go->AddComponent<ItemComponent>();

  int money = (amount == 0) ? RNG::Instance().Random() % 1000 + 1 : amount;
  ((ItemComponent*)c)->Cost = money;

  return go;
}
