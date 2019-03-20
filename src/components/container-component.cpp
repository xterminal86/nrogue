#include "container-component.h"
#include "container-interact-state.h"
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
  bool foundStack = false;

  ItemComponent* itemToAdd = object->GetComponent<ItemComponent>();

  if (itemToAdd->Data.IsStackable)
  {
    for (auto& i : Contents)
    {
      auto ic = i->GetComponent<ItemComponent>();
      ItemComponent* inventoryItemComponent = static_cast<ItemComponent*>(ic);
      if (itemToAdd->Data.IsIdentified && inventoryItemComponent->Data.IsIdentified
       && inventoryItemComponent->Data.ItemTypeHash == itemToAdd->Data.ItemTypeHash)
      {
        inventoryItemComponent->Data.Amount += itemToAdd->Data.Amount;
        foundStack = true;

        // !!! GameObject* object should not be destroyed here !!!

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

void ContainerComponent::Interact()
{
  auto s = Application::Instance().GetGameStateRefByName(GameStates::CONTAINER_INTERACT_STATE);
  ContainerInteractState* cis = static_cast<ContainerInteractState*>(s);
  cis->SetContainerRef(this);
  Application::Instance().ChangeState(GameStates::CONTAINER_INTERACT_STATE);
}

bool ContainerComponent::IsFull()
{
  return (Contents.size() >= MaxCapacity);
}

bool ContainerComponent::IsEmpty()
{
  return (Contents.size() == 0);
}
