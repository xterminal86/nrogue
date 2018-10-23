#include "container-component.h"
#include "application.h"
#include "item-component.h"

ContainerComponent::ContainerComponent()
{
  _hash = typeid(*this).hash_code();
}

void ContainerComponent::Update()
{
}

void ContainerComponent::AddToInventory(GameObject* object)
{
  auto& playerRef = Application::Instance().PlayerInstance;

  bool isStackable = false;

  auto ic = object->GetComponent<ItemComponent>();

  for (auto& i : playerRef.Inventory.Contents)
  {
    auto iic = i->GetComponent<ItemComponent>();
    if (((ItemComponent*)iic)->TypeOfObject == ((ItemComponent*)ic)->TypeOfObject)
    {
      ((ItemComponent*)iic)->Amount += ((ItemComponent*)ic)->Amount;
      isStackable = true;
      break;
    }
  }

  if (!isStackable)
  {
    Contents.push_back(std::unique_ptr<GameObject>(object));
  }
}
