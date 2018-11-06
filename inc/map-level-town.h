#ifndef MAPLEVELTOWN_H
#define MAPLEVELTOWN_H

#include "map-level-base.h"

class MapLevelTown : public MapLevelBase
{
  public:
    MapLevelTown(int sizeX, int sizeY, MapType type);

    void PrepareMap(MapLevelBase* levelOwner) override;

  protected:
    void CreateLevel() override;

  private:    
    void FillArea(int ax, int ay, int aw, int ah, const Tile& tileToFill);
    void CreateRoom(int x, int y, const std::vector<std::string>& layout, bool randomizeOrientation = false);
    void CreateDoor(int x, int y, bool isOpen = false);
    void CreateChurch(int x, int y);    
};

#endif // MAPLEVELTOWN_H
