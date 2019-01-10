#include "map-level-deep-dark.h"

MapLevelDeepDark::MapLevelDeepDark(int sizeX, int sizeY, MapType type, int dungeonLevel)
  : MapLevelBase(sizeX, sizeY, type, dungeonLevel)
{
}

void MapLevelDeepDark::PrepareMap(MapLevelBase* levelOwner)
{
  MapLevelBase::PrepareMap(levelOwner);

  CreateLevel();
}

void MapLevelDeepDark::DisplayWelcomeText()
{
}

void MapLevelDeepDark::CreateLevel()
{
  RecordEmptyCells();
  PlaceStairs();
}
