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
  if (OpenedBy != -1)
  {
    bool success = false;

    auto playerPref = &Application::Instance().PlayerInstance;
    for (auto& i : playerPref->Inventory.Contents)
    {
      ItemComponent* ic = i->GetComponent<ItemComponent>();
      if (ic->Data.ItemTypeHash == OpenedBy)
      {
        IsOpen = !IsOpen;
        UpdateDoorState();
        OpenedBy = -1;
        success = true;
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
