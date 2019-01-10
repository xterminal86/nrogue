#include "map-level-deep-dark.h"

#include "application.h"

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
  std::vector<std::string> msg =
  {
    "Deep Dark level description goes here"
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "Deep Dark", msg);
}

void MapLevelDeepDark::CreateLevel()
{
  RecordEmptyCells();
  PlaceStairs();
}
