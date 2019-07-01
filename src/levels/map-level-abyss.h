#ifndef MAPLEVELABYSS_H
#define MAPLEVELABYSS_H

#include "map-level-base.h"

class MapLevelAbyss : public MapLevelBase
{
  public:
    MapLevelAbyss(int sizeX, int sizeY, MapType type, int dungeonLevel);

    void PrepareMap(MapLevelBase* levelOwner) override;
    void DisplayWelcomeText() override;

  protected:
    void CreateLevel() override;
    void ConstructFromBuilder(LevelBuilder& lb) override;
};

#endif // MAPLEVELABYSS_H
