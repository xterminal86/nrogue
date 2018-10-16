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

  go->ObjectName = "Coins";
  go->Image = '$';
  go->FgColor = "#FFD700";
  go->Visible = true;

  Component* c = go->AddComponent<ItemComponent>();

  ((ItemComponent*)c)->Description = { "Some gold coins", "You can buy things with these" };

  int money = (amount == 0) ? RNG::Instance().Random() % 1000 + 1 : amount;
  ((ItemComponent*)c)->Cost = money;

  return go;
}
