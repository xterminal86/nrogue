#include "map-level-nether.h"

#include "application.h"

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
  std::vector<std::string> msg =
  {
    "Nether level description goes here"
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "Nether", msg);
}

void MapLevelNether::CreateLevel()
{
  RecordEmptyCells();
  PlaceStairs();
}