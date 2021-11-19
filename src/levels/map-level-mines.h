#ifndef MAPLEVELMINES_H
#define MAPLEVELMINES_H

#include "map-level-base.h"

#include "level-builder.h"

class MapLevelMines : public MapLevelBase
{
  public:
    MapLevelMines(int sizeX, int sizeY, MapType type, int dungeonLevel);

    void PrepareMap(MapLevelBase* levelOwner) override;
    void DisplayWelcomeText() override;

  protected:
    void CreateLevel() override;
    void ConstructFromBuilder(LevelBuilder& lb) override;
    void CreateSpecialLevel() override;
    void CreateSpecialMonsters() override;

  private:
    void CreateRandomBoxes();
};

#endif // MAPLEVELMINES_H
