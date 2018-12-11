#ifndef MAPLEVELCAVES_H
#define MAPLEVELCAVES_H

#include "map-level-base.h"

#include "level-builder.h"

class MapLevelCaves : public MapLevelBase
{
  public:
    MapLevelCaves(int sizeX, int sizeY, MapType type, int dungeonLevel);

    void PrepareMap(MapLevelBase* levelOwner) override;
    void DisplayWelcomeText() override;
    void TryToSpawnMonsters() override;

  protected:
    void CreateLevel() override;

  private:
    void ConstructFromBuilder(LevelBuilder& lb);
    void PlaceStairs();
};

#endif // MAPLEVELCAVES_H
