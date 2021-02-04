#include "door-component.h"
#include "game-object.h"
#include "constants.h"
#include "printer.h"
#include "util.h"
#include "application.h"

DoorComponent::DoorComponent()
{
  _componentHash = typeid(*this).hash_code();
}

void DoorComponent::Update()
{
}

void DoorComponent::Interact()
{
  if (OpenedBy != GlobalConstants::OpenedByAnyone)
  {
    bool success = false;

    auto playerPref = &Application::Instance().PlayerInstance;
    for (size_t i = 0; i < playerPref->Inventory.Contents.size(); i++)
    {
      auto& itemRef = playerPref->Inventory.Contents[i];
      ItemComponent* ic = itemRef->GetComponent<ItemComponent>();
      if (ic->Data.ItemTypeHash == OpenedBy)
      {
        IsOpen = !IsOpen;
        UpdateDoorState();
        OpenedBy = GlobalConstants::OpenedByAnyone;
        success = true;

        // NOTE: what if the same "key" can open several doors?
        //
        // Destroy "key" item from inventory since it's no longer needed
        //ic->Data.IsImportant = false;
        //
        //auto it = playerPref->Inventory.Contents.begin();
        //playerPref->Inventory.Contents.erase(it + i);

        break;
      }
    }

    if (!success)
    {
      auto str = Util::StringFormat("%s is locked!", OwnerGameObject->ObjectName.data());
      Printer::Instance().AddMessage(str);
    }
  }
  else
  {
    IsOpen = !IsOpen;
    UpdateDoorState();
  }
}

void DoorComponent::UpdateDoorState()
{  
  OwnerGameObject->Blocking = !IsOpen;
  OwnerGameObject->BlocksSight = !IsOpen;
  OwnerGameObject->Image = IsOpen ? '_' : '+';
  OwnerGameObject->FgColor = IsOpen ?
                               "#FFFFFF" :
                                (FgColorOverride.empty() ?
                                 "#FFFFFF" :
                                 FgColorOverride);
  OwnerGameObject->BgColor = IsOpen ?
                               "#000000" :
                                (BgColorOverride.empty() ?
                                  GlobalConstants::DoorHighlightColor :
                                  BgColorOverride);
}
