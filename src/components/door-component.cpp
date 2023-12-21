#include "door-component.h"
#include "game-object.h"
#include "constants.h"
#include "printer.h"
#include "util.h"
#include "application.h"

DoorComponent::DoorComponent()
{
}

// =============================================================================

void DoorComponent::Update()
{
}

// =============================================================================

bool DoorComponent::InteractBySomeone()
{
  if (OpenedBy != GlobalConstants::OpenedByAnyone)
  {
    return false;
  }

  IsOpen = !IsOpen;
  UpdateDoorState();

  return true;
}

// =============================================================================

IR DoorComponent::Interact()
{
  if (OpenedBy != GlobalConstants::OpenedByAnyone)
  {
    bool success = false;

    auto playerPref = &Application::Instance().PlayerInstance;
    for (size_t i = 0; i < playerPref->Inventory->Contents.size(); i++)
    {
      auto& itemRef = playerPref->Inventory->Contents[i];
      ItemComponent* ic = itemRef->GetComponent<ItemComponent>();
      if (ic->Data.ItemTypeHash == OpenedBy)
      {
        IsOpen = !IsOpen;
        UpdateDoorState();
        OpenedBy = GlobalConstants::OpenedByAnyone;
        success = true;

        // Destroy "key" item from inventory since it's no longer needed
        ic->Data.IsImportant = false;

        auto it = playerPref->Inventory->Contents.begin();
        playerPref->Inventory->Contents.erase(it + i);

        break;
      }
    }

    if (!success)
    {
      auto str = Util::StringFormat("%s - locked!", OwnerGameObject->ObjectName.data());
      Printer::Instance().AddMessage(str);

      return { InteractionResult::FAILURE, GameStates::MAIN_STATE };
    }
  }
  else
  {
    IsOpen = !IsOpen;
    UpdateDoorState();
  }

  PrintInteractionMessage();

  return { InteractionResult::SUCCESS, GameStates::MAIN_STATE };
}

// =============================================================================

void DoorComponent::UpdateDoorState()
{
  OwnerGameObject->Blocking    = !IsOpen;
  OwnerGameObject->BlocksSight = !IsOpen;
  OwnerGameObject->Image   = IsOpen ? '_' : '+';

  OwnerGameObject->FgColor = IsOpen ?
                               Colors::WhiteColor :
                                (FgColorOverride == Colors::None ?
                                 Colors::WhiteColor :
                                 FgColorOverride);

  OwnerGameObject->BgColor = IsOpen ?
                               Colors::BlackColor :
                                (BgColorOverride == Colors::None ?
                                  Colors::DoorHighlightColor :
                                  BgColorOverride);
}

// =============================================================================

void DoorComponent::PrintInteractionMessage()
{
  auto str = Util::StringFormat("You %s: %s", (IsOpen ? "opened" : "closed"), OwnerGameObject->ObjectName.data());
  Printer::Instance().AddMessage(str);
}
