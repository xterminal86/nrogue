#ifndef MAPLEVELNETHER_H
#define MAPLEVELNETHER_H

#include "map-level-base.h"

class MapLevelNether : public MapLevelBase
{
  public:
    MapLevelNether(int sizeX, int sizeY, MapType type, int dungeonLevel);

    void PrepareMap(MapLevelBase* levelOwner) override;
    void DisplayWelcomeText() override;

  protected:
    void CreateLevel() override;
};

#endif // MAPLEVELNETHER_H
