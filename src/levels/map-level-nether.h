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

    void CreateCommonObjects(int x, int y, char image) override;
    void CreateSpecialObjects(int x, int y, const MapCell& cell) override;
};

#endif // MAPLEVELNETHER_H
