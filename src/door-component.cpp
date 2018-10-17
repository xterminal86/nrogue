#include "door-component.h"
#include "game-object.h"
#include "constants.h"

DoorComponent::DoorComponent()
{
  _hash = typeid(*this).hash_code();
}

void DoorComponent::Update()
{
}

void DoorComponent::Interact()
{
  IsOpen = !IsOpen;
  UpdateDoorState();
}

void DoorComponent::UpdateDoorState()
{  
  ((GameObject*)OwnerGameObject)->Blocking = !IsOpen;
  ((GameObject*)OwnerGameObject)->BlockSight = !IsOpen;
  ((GameObject*)OwnerGameObject)->Image = IsOpen ? '_' : '+';
  ((GameObject*)OwnerGameObject)->FgColor = "#FFFFFF";
  ((GameObject*)OwnerGameObject)->BgColor = IsOpen ? "#000000" : GlobalConstants::DoorHighlightColor;
}
