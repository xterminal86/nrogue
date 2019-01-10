#include "map-level-abyss.h"

#include "application.h"

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
  std::vector<std::string> msg =
  {
    "Abyss level description goes here"
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "Abyss", msg);
}

void MapLevelAbyss::CreateLevel()
{
  RecordEmptyCells();
  PlaceStairs();
}
