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
};

#endif // MAPLEVELENDGAME_H
