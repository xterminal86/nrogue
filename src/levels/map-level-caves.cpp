#include "map-level-caves.h"

#include "application.h"
#include "game-objects-factory.h"

MapLevelCaves::MapLevelCaves(int sizeX, int sizeY, MapType type, int dungeonLevel) :
  MapLevelBase(sizeX, sizeY, type, dungeonLevel)
{
}

void MapLevelCaves::PrepareMap(MapLevelBase* levelOwner)
{
  MapLevelBase::PrepareMap(levelOwner);

  CreateLevel();
}

void MapLevelCaves::CreateLevel()
{
  VisibilityRadius = 8;
  MonstersRespawnTurns = 1000;

  // Build level

  LevelBuilder lb;

  //Position start(1, 1);

  lb.RecursiveBacktracker(MapSize);

  ConstructFromBuilder(lb);

  RecordEmptyCells();

  PlaceStairs();
}

void MapLevelCaves::ConstructFromBuilder(LevelBuilder& lb)
{
  Logger::Instance().Print("********** INSTANTIATING LAYOUT **********");

  for (int x = 0; x < MapSize.X; x++)
  {
    for (int y = 0; y < MapSize.Y; y++)
    {
      Tile t;
      std::string objName;

      char image = lb.MapRaw[x][y];
      switch (image)
      {
        case '#':
        {
          objName = "Cave Wall";
          t.Set(true, true, ' ', GlobalConstants::BlackColor, "#964B00", "Cave Wall");
          MapArray[x][y]->MakeTile(t);
        }
        break;

        case '.':
        {
          objName = "Ground";
          t.Set(false, false, image, GlobalConstants::GroundColor, GlobalConstants::BlackColor, objName);
          MapArray[x][y]->MakeTile(t);
        }
        break;
      }
    }
  }
}

void MapLevelCaves::DisplayWelcomeText()
{
  std::vector<std::string> msg =
  {
    "Caves level description goes here"
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "Caves of Circe", msg);
}
