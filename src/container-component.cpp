#include "container-component.h"
#include "application.h"
#include "item-component.h"
#include "util.h"

ContainerComponent::ContainerComponent()
{
  _hash = typeid(*this).hash_code();
}

void ContainerComponent::Update()
{
}

void ContainerComponent::AddToInventory(GameObject* object)
{  
  bool foundStack = false;

  auto ic = object->GetComponent<ItemComponent>();

  if (((ItemComponent*)ic)->IsStackable)
  {
    for (auto& i : Contents)
    {
      auto iic = i->GetComponent<ItemComponent>();
      if (((ItemComponent*)iic)->TypeOfObject == ((ItemComponent*)ic)->TypeOfObject)
      {
        ((ItemComponent*)iic)->Amount += ((ItemComponent*)ic)->Amount;
        foundStack = true;
        break;
      }
    }
  }

  //auto msg = Util::StringFormat("Picked up 0x%X", object);
  //Logger::Instance().Print(msg);

  if (!foundStack)
  {
    Contents.push_back(std::unique_ptr<GameObject>(object));
  }
}
