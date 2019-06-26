#ifndef TOWNPORTALCOMPONENT_H
#define TOWNPORTALCOMPONENT_H

#include "component.h"
#include "constants.h"
#include "position.h"

class TownPortalComponent : public Component
{
  public:
    TownPortalComponent();

    void Update() override;

    void SavePosition(MapType mapToReturn, const Position& posToReturn);
    void TeleportBack();

  private:
    std::pair<MapType, Position> _posToReturn;
};

#endif // TOWNPORTALCOMPONENT_H
