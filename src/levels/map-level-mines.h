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
    void OnLevelChanged(MapType from) override;

  protected:
    void CreateLevel() override;
    void CreateSpecialLevel() override;
    void CreateSpecialMonsters() override;

    void CreateCommonObjects(int x, int y, char image) override;
    void CreateSpecialObjects(int x, int y, const MapCell& cell) override;

#ifdef false
  private:
    void CreateRandomBoxes();
#endif
};

#endif // MAPLEVELMINES_H
