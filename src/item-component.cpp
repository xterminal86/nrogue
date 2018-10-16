#include "item-component.h"

#include "application.h"
#include "map.h"

ItemComponent::ItemComponent()
{
  _hash = typeid(*this).hash_code();
}

void ItemComponent::Update()
{
}

void ItemComponent::Transfer(ContainerComponent* destination)
{
  if (destination == nullptr)
  {
    ((GameObject*)OwnerGameObject)->PosX = Application::Instance().PlayerInstance.PosX;
    ((GameObject*)OwnerGameObject)->PosY = Application::Instance().PlayerInstance.PosY;

    Map::Instance().InsertGameObject(((GameObject*)OwnerGameObject));
  }
}

void ItemComponent::Equip()
{
}

void ItemComponent::Inspect()
{
  Application::Instance().ShowMessageBox(((GameObject*)OwnerGameObject)->ObjectName, Description);
}

void ItemComponent::Throw()
{
}

void ItemComponent::Use()
{
}
