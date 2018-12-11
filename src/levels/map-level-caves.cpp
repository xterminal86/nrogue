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
  VisibilityRadius = 4;
  MonstersRespawnTurns = 1000;

  // Build level

  LevelBuilder lb;

  Position start(1, 1);

  lb.RecursiveBacktracker(MapSize, start);

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
          objName = "Rocks";
          t.Set(true, true, ' ', GlobalConstants::BlackColor, "#964B00", "Rocks");
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
    "Caves description goes here"
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "Caves of Circe", msg);
}

void MapLevelCaves::PlaceStairs()
{
  int startIndex = RNG::Instance().RandomRange(0, _emptyCells.size());

  LevelStart.X = _emptyCells[startIndex].X;
  LevelStart.Y = _emptyCells[startIndex].Y;

  MapType stairsDownTo = (MapType)(DungeonLevel + 1);
  MapType stairsUpTo = (MapType)(DungeonLevel - 1);

  GameObjectsFactory::Instance().CreateStairs(this, LevelStart.X, LevelStart.Y, '<', stairsUpTo);

  std::vector<Position> possibleExits;

  int mapSizeMax = std::max(MapSize.X, MapSize.Y);

  for (int i = 0; i < _emptyCells.size(); i++)
  {
    auto& c = _emptyCells[i];

    float d = Util::LinearDistance(LevelStart.X, LevelStart.Y, c.X, c.Y);
    if (i != startIndex && d > mapSizeMax / 2)
    {
      possibleExits.push_back(c);
    }
  }

  int endIndex = RNG::Instance().RandomRange(0, possibleExits.size());
  auto exit = possibleExits[endIndex];

  LevelExit.Set(exit.X, exit.Y);

  GameObjectsFactory::Instance().CreateStairs(this, LevelExit.X, LevelExit.Y, '>', stairsDownTo);
}

void MapLevelCaves::TryToSpawnMonsters()
{
}
