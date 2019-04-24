#ifndef MAPLEVELLOSTCITY_H
#define MAPLEVELLOSTCITY_H

#include "map-level-base.h"

#include "level-builder.h"

class MapLevelLostCity : public MapLevelBase
{
  public:
    MapLevelLostCity(int sizeX, int sizeY, MapType type, int dungeonLevel);

    void PrepareMap(MapLevelBase* levelOwner) override;
    void DisplayWelcomeText() override;

  protected:
    void CreateLevel() override;

  private:
    void ConstructFromBuilder(LevelBuilder& lb);
};

#endif // MAPLEVELLOSTCITY_H
