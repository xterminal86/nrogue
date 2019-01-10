#include "map-level-lost-city.h"

#include "application.h"

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
  std::vector<std::string> msg =
  {
    "Lost City level description goes here"
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "Lost City", msg);
}

void MapLevelLostCity::CreateLevel()
{
  RecordEmptyCells();
  PlaceStairs();
}
