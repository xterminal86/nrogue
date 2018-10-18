#include "game-objects-factory.h"

#include "constants.h"
#include "rng.h"
#include "item-component.h"
#include "shrine-component.h"
#include "map.h"

GameObject* GameObjectsFactory::CreateGameObject(ItemType objType)
{
  GameObject* go = nullptr;

  switch (objType)
  {
    case ItemType::COINS:
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
  go->FgColor = GlobalConstants::CoinsColor;
  go->Visible = true;

  Component* c = go->AddComponent<ItemComponent>();

  ((ItemComponent*)c)->Description = { "Some gold coins", "You can buy things with these" };

  int money = (amount == 0) ? RNG::Instance().Random() % 1000 + 1 : amount;
  ((ItemComponent*)c)->Cost = money;
  ((ItemComponent*)c)->Amount = money;
  ((ItemComponent*)c)->IsStackable = true;
  ((ItemComponent*)c)->TypeOfObject = ItemType::COINS;

  return go;
}

GameObject* GameObjectsFactory::CreateShrine(int x, int y, ShrineType type, int timeout)
{
  GameObject* go = new GameObject(x, y, '/',
                                (type == ShrineType::MIGHT) ?
                                GlobalConstants::ShrineMightColor :
                                GlobalConstants::ShrineSpiritColor,
                                GlobalConstants::WallColor);

  auto c = go->AddComponent<ShrineComponent>();
  ShrineComponent* sc = dynamic_cast<ShrineComponent*>(c);

  sc->Type = type;
  sc->Counter = timeout;
  sc->Timeout = timeout;

  go->ObjectName = "Shrine";
  go->InteractionCallback = std::bind(&ShrineComponent::Interact, sc);

  return go;
}
