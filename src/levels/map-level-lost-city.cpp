#include "map-level-lost-city.h"

#include "game-objects-factory.h"
#include "game-object-info.h"
#include "application.h"
#include "logger.h"

MapLevelLostCity::MapLevelLostCity(int sizeX, int sizeY, MapType type, int dungeonLevel)
  : MapLevelBase(sizeX, sizeY, type, dungeonLevel)
{
}

void MapLevelLostCity::PrepareMap(MapLevelBase* levelOwner)
{
  MapLevelBase::PrepareMap(levelOwner);

  CreateLevel();
}

void MapLevelLostCity::CreateLevel()
{
  VisibilityRadius = 20;
  MonstersRespawnTurns = 1000;

  LevelBuilder lb;

  FeatureRoomsWeights weights =
  {
    { FeatureRoomType::EMPTY,    { 10, 0 }  },
    { FeatureRoomType::DIAMOND,  { 3, 3 }   },
    { FeatureRoomType::FLOODED,  { 1, 3 }   },
    { FeatureRoomType::GARDEN,   { 3, 3 }   },
    { FeatureRoomType::PILLARS,  { 3, 0 }   },
    { FeatureRoomType::ROUND,    { 5, 3 }   },
    { FeatureRoomType::POND,     { 3, 3 }   },
    { FeatureRoomType::FOUNTAIN, { 3, 2 }   },
    { FeatureRoomType::SHRINE,   { 10, 2 }  }
  };

  Position roomSize = { 1, 10 };

  lb.FeatureRoomsMethod(MapSize, roomSize, weights, 3, MapSize.X * MapSize.Y);

  ConstructFromBuilder(lb);

  GameObjectInfo t;
  t.Set(true, true, ' ', Colors::BlackColor, Colors::ShadesOfGrey::Six, "Rocks");

  CreateBorders(t);

  RecordEmptyCells();
  PlaceStairs();
  //CreateInitialMonsters();

  CreateShrines(lb);

  int itemsToCreate = GetEstimatedNumberOfItemsToCreate();
  CreateItemsForLevel(itemsToCreate);
}

void MapLevelLostCity::ConstructFromBuilder(LevelBuilder& lb)
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
          PlaceWall(x, y, ' ', Colors::BlackColor, Colors::ShadesOfGrey::Six, "Rocks");
          break;

        case '+':
          PlaceDoor(x, y);
          break;

        case 't':
        {
          char img = 'T';

          #ifdef USE_SDL
          img = GlobalConstants::CP437IndexByType[NameCP437::CLUB];
          #endif

          objName = "Withered Tree";
          t.Set(true, true, img, Colors::DirtColor, Colors::BlackColor, objName);
          InsertStaticObject(x, y, t);
        }
        break;

        case 'T':
          PlaceTree(x, y);
          break;

        case '.':
          PlaceGroundTile(x, y, image, Colors::ShadesOfGrey::Four, Colors::BlackColor, "Ground");
          break;

        case ' ':
          PlaceGroundTile(x, y, image, Colors::BlackColor, Colors::ShadesOfGrey::Ten, "Flagstone");
          break;

        // TODO: can step in lava and get killed / damaged?

        case 'l':
          PlaceLavaTile(x, y);
          break;

        case 'w':
          PlaceShallowWaterTile(x, y);
          break;

        case 'W':
          PlaceDeepWaterTile(x, y);
          break;

        case 'd':
          PlaceGroundTile(x, y, '.', Colors::DirtDotColor, Colors::DirtColor, "Dirt");
          break;

        case 'F':
          t.Set(true, false, image, Colors::WhiteColor, Colors::DeepWaterColor, "Fountain");
          InsertStaticObject(x, y, t);
          break;

        case 'g':
          PlaceGrassTile(x, y);
          break;
      }
    }
  }
}

void MapLevelLostCity::CreateShrines(LevelBuilder& lb)
{
  auto allShrines = lb.ShrinesByPosition();
  for (auto& kvp : allShrines)
  {
    ShrineType shrineType = kvp.second;
    std::string description = GlobalConstants::ShrineNameByType.at(shrineType);
    auto shrineColor = Colors::ShrineColorsByType.at(shrineType);

    GameObjectInfo t;
    t.Set(true, false, '/', shrineColor.first, shrineColor.second, description, "?Shrine?");
    InsertStaticObject(kvp.first.X, kvp.first.Y, t);

    auto go = GameObjectsFactory::Instance().CreateShrine(kvp.first.X, kvp.first.Y, shrineType, 1000);
    InsertGameObject(go);
  }
}

void MapLevelLostCity::DisplayWelcomeText()
{
  std::vector<std::string> msg =
  {
  // ============================================ <- 44
    "You certainly heard rumors about a city that",
    "was buried somewhere deep underground upon  ",
    "which current City is built.                ",
    "Some even say that back in the past it was  ",
    "all one giant city, but then for some reason",
    "part of it was abandoned and over time sunk ",
    "into the ground. Since nobody knows whether ",
    "it had a name or whether it even existed,   ",
    "people refer to it as just 'Lost City'.     ",
    "Nobody took its existence seriously,        ",
    "but here you are, standing in its halls.    "
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "Lost City", msg);
}
