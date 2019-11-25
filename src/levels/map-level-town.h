#ifndef MAPLEVELTOWN_H
#define MAPLEVELTOWN_H

#include "map-level-base.h"

class Rect;

class MapLevelTown : public MapLevelBase
{
  public:
    MapLevelTown(int sizeX, int sizeY, MapType type);

    void PrepareMap(MapLevelBase* levelOwner) override;

    void CreateNPCs();

  protected:
    void CreateLevel() override;

  private:    
    void FillArea(int ax, int ay, int aw, int ah, const GameObjectInfo& tileToFill);
    void CreateRoom(int x, int y, const std::vector<std::string>& layout, bool randomizeOrientation = false);
    void CreateChurch(int x, int y);
    void CreateBlacksmith(int x, int y, const std::vector<std::string>& layout, bool randomizeOrientation = false);
    void PlaceGarden(int x, int y);
    void PlaceMineEntrance(int x, int y);
    void PlacePortalSquare(int x, int y);
    void CreatePlayerHouse();    
    void CreateTownGates();
    void ReplaceGroundWithGrass();
    void BuildRoads();

    bool SkipArea(const Position& pos, const Rect& area);

    const int FlowersFrequency = 25;
};

#endif // MAPLEVELTOWN_H
