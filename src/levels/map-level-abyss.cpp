#include "map-level-abyss.h"

MapLevelAbyss::MapLevelAbyss(int sizeX, int sizeY, MapType type, int dungeonLevel)
  : MapLevelBase(sizeX, sizeY, type, dungeonLevel)
{
}

void MapLevelAbyss::PrepareMap(MapLevelBase* levelOwner)
{
  MapLevelBase::PrepareMap(levelOwner);

  CreateLevel();
}

void MapLevelAbyss::DisplayWelcomeText()
{
}

void MapLevelAbyss::CreateLevel()
{
  RecordEmptyCells();
  PlaceStairs();
}
