#ifndef MAPLEVELCAVES_H
#define MAPLEVELCAVES_H

#include "map-level-base.h"

#include "level-builder.h"

class MapLevelCaves : public MapLevelBase
{
  public:
    MapLevelCaves(int sizeX, int sizeY, MapType type, int dungeonLevel);

    void PrepareMap() override;
    void DisplayWelcomeText() override;

  protected:
    void CreateLevel() override;
    void CreateSpecialLevel() override;

    void CreateCommonObjects(int x, int y, char image) override;

  private:
    void CreateRivers();
};

#endif // MAPLEVELCAVES_H
