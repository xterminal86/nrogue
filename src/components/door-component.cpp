#include "door-component.h"
#include "game-object.h"
#include "constants.h"
#include "printer.h"
#include "util.h"

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
  OwnerGameObject->Blocking = !IsOpen;
  OwnerGameObject->BlocksSight = !IsOpen;
  OwnerGameObject->Image = IsOpen ? '_' : '+';
  OwnerGameObject->FgColor = "#FFFFFF";
  OwnerGameObject->BgColor = IsOpen ? "#000000" : GlobalConstants::DoorHighlightColor;
}
