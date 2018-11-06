#ifndef MAPLEVELMINES_H
#define MAPLEVELMINES_H

#include "map-level-base.h"

class MapLevelMines : public MapLevelBase
{
  public:
    MapLevelMines(int sizeX, int sizeY, MapType type, int dungeonLevel);

    void PrepareMap(MapLevelBase* levelOwner) override;

  protected:
    void CreateLevel() override;

  private:
    void CreateRoom(int x, int y, const std::vector<std::string>& layout, bool randomizeOrientation = true);

    std::vector<Position> _emptyCells;
};

#endif // MAPLEVELMINES_H
