#ifndef DOORCOMPONENT_H
#define DOORCOMPONENT_H

#include "component.h"

#include <string>

class DoorComponent : public Component
{
  public:
    DoorComponent();

    void Update() override;

    void Interact();
    void UpdateDoorState();

    bool IsOpen;

    // Hash of item that opens this door,
    // -1 to leave unlocked
    size_t OpenedBy = -1;

    std::string FgColorOverride;
    std::string BgColorOverride;
};

#endif // DOORCOMPONENT_H
