#ifndef STAIRSCOMPONENT_H
#define STAIRSCOMPONENT_H

#include "component.h"
#include "enumerations.h"

class StairsComponent : public Component
{
  public:
    StairsComponent();

    void Update() override;

    MapType LeadsTo = MapType::NOWHERE;
};

#endif // STAIRSCOMPONENT_H
