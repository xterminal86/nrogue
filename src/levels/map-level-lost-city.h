#ifndef MAPLEVELLOSTCITY_H
#define MAPLEVELLOSTCITY_H

#include "map-level-base.h"

class MapLevelLostCity : public MapLevelBase
{
  public:
    MapLevelLostCity(int sizeX, int sizeY, MapType type, int dungeonLevel);

    void PrepareMap(MapLevelBase* levelOwner) override;
    void DisplayWelcomeText() override;

  protected:
    void CreateLevel() override;
};

#endif // MAPLEVELLOSTCITY_H
