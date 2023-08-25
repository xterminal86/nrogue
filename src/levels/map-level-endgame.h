#ifndef MAPLEVELENDGAME_H
#define MAPLEVELENDGAME_H

#include "map-level-base.h"

class MapLevelEndgame : public MapLevelBase
{
  public:
    MapLevelEndgame(int sizeX, int sizeY, MapType type, int dungeonLevel);

    void PrepareMap(MapLevelBase* levelOwner) override;
    void DisplayWelcomeText() override;

  protected:
    void CreateLevel() override;

    void CreateCommonObjects(int x, int y, char image) override;
    void CreateSpecialObjects(int x, int y, const MapCell& cell) override;
};

#endif // MAPLEVELENDGAME_H
