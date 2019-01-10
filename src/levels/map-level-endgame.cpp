#include "map-level-endgame.h"

MapLevelEndgame::MapLevelEndgame(int sizeX, int sizeY, MapType type, int dungeonLevel)
  : MapLevelBase(sizeX, sizeY, type, dungeonLevel)
{
}

void MapLevelEndgame::PrepareMap(MapLevelBase* levelOwner)
{
  MapLevelBase::PrepareMap(levelOwner);

  CreateLevel();
}

void MapLevelEndgame::DisplayWelcomeText()
{
}

void MapLevelEndgame::CreateLevel()
{
}
