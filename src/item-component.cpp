#include "item-component.h"

#include "application.h"

ItemComponent::ItemComponent()
{
  _hash = typeid(*this).hash_code();
}

void ItemComponent::Update()
{
}

void ItemComponent::Drop()
{
}

void ItemComponent::Equip()
{
}

void ItemComponent::Inspect()
{  

  //Application::Instance().ShowMessageBox(((GameObject*)OwnerGameObject)->ObjectName);
}

void ItemComponent::Throw()
{
}

void ItemComponent::Use()
{
}
