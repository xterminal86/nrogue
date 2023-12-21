#include "container-component.h"
#include "container-interact-state.h"
#include "application.h"
#include "item-component.h"
#include "printer.h"
#include "util.h"

#ifdef DEBUG_BUILD
#include "logger.h"
#endif

ContainerComponent::ContainerComponent(size_t maxCapacity)
{
  _maxCapacity = maxCapacity;

  Contents.reserve(_maxCapacity);
}

// =============================================================================

void ContainerComponent::Update()
{
}

// =============================================================================

bool ContainerComponent::Add(GameObject* object)
{
  bool foundStack = false;

  if (object != nullptr)
  {
    ItemComponent* itemToAdd = object->GetComponent<ItemComponent>();

    if (itemToAdd->Data.IsStackable)
    {
      for (auto& i : Contents)
      {
        ItemComponent* ic = i->GetComponent<ItemComponent>();

        //
        // If item is identified and is stackable, stack it.
        //
        bool isIded = (ic->Data.IsIdentified && itemToAdd->Data.IsIdentified &&
                      (ic->Data.ItemTypeHash == itemToAdd->Data.ItemTypeHash));

        //
        // If item is not identified, but came from the same stack, add it back.
        //
        bool fromTheSameStack = (!ic->Data.IsIdentified
                              && !itemToAdd->Data.IsIdentified
                              && (ic->OwnerGameObject->StackObjectId == itemToAdd->OwnerGameObject->StackObjectId));

        if (isIded || fromTheSameStack)
        {
          ic->Data.Amount += itemToAdd->Data.Amount;
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

#ifdef DEBUG_BUILD
  if (object == nullptr)
  {
    std::string msg = "Trying to add nullptr to inventory!";
    Printer::Instance().AddMessage(msg);
    Logger::Instance().Print(msg, true);
    DebugLog("%s\n", msg.data());
  }
#endif

  return foundStack;
}

// =============================================================================

IR ContainerComponent::Interact()
{
  // TODO: locked containers (lockpicking maybe?)

  if (!CanBeOpened)
  {
    std::string failMsg = Util::StringFormat("%s can't be opened!", OwnerGameObject->ObjectName.data());
    Printer::Instance().AddMessage(failMsg);
    return { InteractionResult::FAILURE, GameStates::MAIN_STATE };
  }

  auto s = Application::Instance().GetGameStateRefByName(GameStates::CONTAINER_INTERACT_STATE);
  ContainerInteractState* cis = static_cast<ContainerInteractState*>(s);
  cis->SetContainerRef(this);

  std::string succMsg = Util::StringFormat("You open %s", OwnerGameObject->ObjectName.data());
  Printer::Instance().AddMessage(succMsg);

  return { InteractionResult::SUCCESS, GameStates::CONTAINER_INTERACT_STATE };
}

// =============================================================================

bool ContainerComponent::IsFull()
{
  return (Contents.size() >= _maxCapacity);
}

// =============================================================================

bool ContainerComponent::IsEmpty()
{
  return (Contents.size() == 0);
}

// =============================================================================

const size_t& ContainerComponent::MaxCapacity()
{
  return _maxCapacity;
}
