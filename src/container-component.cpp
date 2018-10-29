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

  auto c = object->GetComponent<ItemComponent>();
  ItemComponent* ic = static_cast<ItemComponent*>(c);

  if (ic->Data.IsStackable)
  {
    for (auto& i : Contents)
    {
      auto iic = i->GetComponent<ItemComponent>();
      ItemComponent* iicc = static_cast<ItemComponent*>(iic);
      if (iicc->Data.TypeOfItem == iicc->Data.TypeOfItem)
      {
        iicc->Data.Amount += iicc->Data.Amount;
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
