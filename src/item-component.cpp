#include "item-component.h"

#include "application.h"
#include "map.h"
#include "game-objects-factory.h"

ItemComponent::ItemComponent()
{
  _hash = typeid(*this).hash_code();
}

void ItemComponent::Update()
{
}

/// Transfers GameObject to the destination,
/// or tile occupied by player if destination = nullptr
void ItemComponent::Transfer(ContainerComponent* destination)
{
  // OwnerGameObject should be in released state

  if (destination == nullptr)
  {
    ((GameObject*)OwnerGameObject)->PosX = Application::Instance().PlayerInstance.PosX;
    ((GameObject*)OwnerGameObject)->PosY = Application::Instance().PlayerInstance.PosY;

    Map::Instance().InsertGameObject(((GameObject*)OwnerGameObject));
  }
  else
  {
    destination->AddToInventory((GameObject*)OwnerGameObject);
  }
}

bool ItemComponent::Equip()
{
  return GameObjectsFactory::Instance().HandleItemEquip(this);
}

void ItemComponent::Inspect()
{
  Application::Instance().ShowMessageBox(false, ((GameObject*)OwnerGameObject)->ObjectName, Description);
}

void ItemComponent::Throw()
{
}

bool ItemComponent::Use()
{
  return GameObjectsFactory::Instance().HandleItemUse(this);
}
