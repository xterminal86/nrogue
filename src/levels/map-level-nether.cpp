#include "map-level-nether.h"

MapLevelNether::MapLevelNether(int sizeX, int sizeY, MapType type, int dungeonLevel)
  : MapLevelBase(sizeX, sizeY, type, dungeonLevel)
{
}

void MapLevelNether::PrepareMap(MapLevelBase* levelOwner)
{
  MapLevelBase::PrepareMap(levelOwner);

  CreateLevel();
}

void MapLevelNether::DisplayWelcomeText()
{
}

void MapLevelNether::CreateLevel()
{
  RecordEmptyCells();
  PlaceStairs();
}
