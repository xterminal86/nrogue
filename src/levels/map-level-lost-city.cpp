#include "map-level-lost-city.h"

MapLevelLostCity::MapLevelLostCity(int sizeX, int sizeY, MapType type, int dungeonLevel)
  : MapLevelBase(sizeX, sizeY, type, dungeonLevel)
{
}

void MapLevelLostCity::PrepareMap(MapLevelBase* levelOwner)
{
  MapLevelBase::PrepareMap(levelOwner);

  CreateLevel();
}

void MapLevelLostCity::DisplayWelcomeText()
{
}

void MapLevelLostCity::CreateLevel()
{
  RecordEmptyCells();
  PlaceStairs();
}
