#ifndef DOORCOMPONENT_H
#define DOORCOMPONENT_H

#include "component.h"

class DoorComponent : public Component
{
  public:
    DoorComponent();

    void Update() override;

    void Interact();
    void UpdateDoorState();

    bool IsOpen;
};

#endif // DOORCOMPONENT_H
