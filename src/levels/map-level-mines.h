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

  private:
    void FillArea(int ax, int ay, int aw, int ah, const GameObjectInfo& tileToFill);          

    void CreateRandomBarrels();
};

#endif // MAPLEVELMINES_H
