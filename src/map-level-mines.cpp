#include "map-level-mines.h"

// Avoiding "invalid application of sizeof to incomplete type"
// #include "game-object.h"

MapLevelMines::MapLevelMines(int sizeX, int sizeY, MapType type, int dungeonLevel) :
  MapLevelBase(sizeX, sizeY, type, dungeonLevel)
{
}

void MapLevelMines::PrepareMap(MapLevelBase* levelOwner)
{
  MapLevelBase::PrepareMap(levelOwner);
}

void MapLevelMines::CreateLevel()
{
}
