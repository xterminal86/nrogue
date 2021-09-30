#ifndef TRIGGERCOMPONENT_H
#define TRIGGERCOMPONENT_H

#include "component.h"

class TriggerComponent : public Component
{
  public:
    TriggerComponent();

    void Update() override;
};

#endif // TRIGGERCOMPONENT_H
