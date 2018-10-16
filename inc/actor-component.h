#ifndef ACTORCOMPONENT_H
#define ACTORCOMPONENT_H

#include "component.h"

#include "constants.h"

class ActorComponent : public Component
{
  public:
    void Update() override {}

  Attributes Attrs;
};

#endif // ACTORCOMPONENT_H
