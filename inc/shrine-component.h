#ifndef SHRINECOMPONENT_H
#define SHRINECOMPONENT_H

#include "constants.h"

#include "component.h"

class ShrineComponent : public Component
{
  public:
    ShrineComponent();

    void Update() override;

    void Interact();

    int Timeout = -1;
    int Counter = 0;

    ShrineType Type;

  private:
    void Activate();
};

#endif // SHRINECOMPONENT_H
