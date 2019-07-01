#include "map-level-abyss.h"

#include "application.h"
#include "game-object-info.h"

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
  LevelBuilder lb;
  lb.CellularAutomataMethod(MapSize, 40, 5, 4, 12);

  ConstructFromBuilder(lb);

  // Borders

  GameObjectInfo t;

  t.Set(true, true, ' ', GlobalConstants::BlackColor, GlobalConstants::MountainsColor, "Rocks");
  CreateBorders(t);

  RecordEmptyCells();
  PlaceStairs();
}

void MapLevelAbyss::ConstructFromBuilder(LevelBuilder& lb)
{
  Logger::Instance().Print("********** INSTANTIATING LAYOUT **********");

  for (int x = 0; x < MapSize.X; x++)
  {
    for (int y = 0; y < MapSize.Y; y++)
    {
      GameObjectInfo t;
      std::string objName;

      char image = lb.MapRaw[x][y];
      switch (image)
      {
        case '#':
        {
          objName = "Rocks";
          t.Set(true, true, ' ', GlobalConstants::BlackColor, GlobalConstants::MountainsColor, "Rocks");
          InsertStaticObject(x, y, t, -1, GameObjectType::PICKAXEABLE);
        }
        break;
      }
    }
  }
}
