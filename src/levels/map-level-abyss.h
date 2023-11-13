#ifndef MAPLEVELABYSS_H
#define MAPLEVELABYSS_H

#include "map-level-base.h"

class MapLevelAbyss : public MapLevelBase
{
  public:
    MapLevelAbyss(int sizeX, int sizeY, MapType type, int dungeonLevel);

    void PrepareMap() override;
    void DisplayWelcomeText() override;

  protected:
    void CreateLevel() override;

    void CreateCommonObjects(int x, int y, char image) override;
};

#endif // MAPLEVELABYSS_H
