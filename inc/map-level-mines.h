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
    void CreateRoom(int x, int y, const std::vector<std::string>& layout, bool randomizeOrientation = true);
    void FillArea(int ax, int ay, int aw, int ah, const Tile& tileToFill);    
    void CopyFromBuilder(LevelBuilder& lb, int startX, int startY);
};

#endif // MAPLEVELMINES_H
