#ifndef MAPLEVELLOSTCITY_H
#define MAPLEVELLOSTCITY_H

#include "map-level-base.h"

#include "level-builder.h"

class MapLevelLostCity : public MapLevelBase
{
  public:
    MapLevelLostCity(int sizeX, int sizeY, MapType type, int dungeonLevel);

    void PrepareMap() override;
    void DisplayWelcomeText() override;

  protected:
    void CreateLevel() override;

    void CreateCommonObjects(int x, int y, char image) override;

  private:
    void CreateShrines(LevelBuilder& lb);
};

#endif // MAPLEVELLOSTCITY_H
