#ifndef MAPLEVELMINES_H
#define MAPLEVELMINES_H

#include "map-level-base.h"

#include "level-builder.h"

class MapLevelMines : public MapLevelBase
{
  public:
    MapLevelMines(int sizeX, int sizeY, MapType type, int dungeonLevel);

    void PrepareMap() override;
    void DisplayWelcomeText() override;
    void OnLevelChanged(MapType from) override;

  protected:
    void CreateLevel() override;
    void CreateSpecialLevel() override;
    void CreateSpecialMonsters() override;

    void CreateCommonObjects(int x, int y, char image) override;
};

#endif // MAPLEVELMINES_H
