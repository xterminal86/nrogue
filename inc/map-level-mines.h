#ifndef MAPLEVELMINES_H
#define MAPLEVELMINES_H

#include "map-level-base.h"

#include "level-builder.h"

class MapLevelMines : public MapLevelBase
{
  public:
    MapLevelMines(int sizeX, int sizeY, MapType type, int dungeonLevel);

    void PrepareMap(MapLevelBase* levelOwner) override;

  protected:
    void CreateLevel() override;

  private:
    void FillArea(int ax, int ay, int aw, int ah, const Tile& tileToFill);    
    void ConstructFromBuilder(LevelBuilder& lb);
    void CreateInitialMonsters();
};

#endif // MAPLEVELMINES_H
