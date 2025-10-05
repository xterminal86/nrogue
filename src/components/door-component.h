#ifndef DOORCOMPONENT_H
#define DOORCOMPONENT_H

#include "component.h"
#include "constants.h"

#include <string>

class DoorComponent : public Component
{
  public:
    void Update() override;

    IR Interact();

    void UpdateDoorState();
    void PrintInteractionMessage();

    bool InteractBySomeone();

    bool IsOpen = false;

    // Hash of item that opens this door,
    // Set to GlobalConstants::OpenedByAnyone to leave unlocked
    size_t OpenedBy = GlobalConstants::OpenedByAnyone;

    uint32_t FgColorOverride = Colors::BlackColor;
    uint32_t BgColorOverride = Colors::BlackColor;

    DoorMaterials Material = DoorMaterials::WOOD;

#ifdef DEBUG_BUILD
    StringV Dump(size_t indent = 0) override;
#endif
};

#endif // DOORCOMPONENT_H
