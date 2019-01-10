#include "map-level-endgame.h"

#include "application.h"

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
  std::vector<std::string> msg =
  {
    "You don't know where you are anymore."
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "???", msg);
}

void MapLevelEndgame::CreateLevel()
{
}
