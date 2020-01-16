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
  t.Set(true, true, ' ', GlobalConstants::BlackColor, GlobalConstants::MountainsColor, "Rocks");

  CreateBorders(t);

  RecordEmptyCells();
  PlaceStairs();
  //CreateInitialMonsters();

  CreateShrines(lb);

  int itemsToCreate = RNG::Instance().RandomRange(1, 3 + DungeonLevel);
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
          PlaceWall(x, y, ' ', GlobalConstants::BlackColor, GlobalConstants::MountainsColor, "Rocks");
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
          t.Set(true, true, img, GlobalConstants::DirtColor, GlobalConstants::BlackColor, objName);
          InsertStaticObject(x, y, t);
        }
        break;

        case 'T':
          PlaceTree(x, y);
          break;

        case '.':
          PlaceGroundTile(x, y, image, GlobalConstants::GroundColor, GlobalConstants::BlackColor, "Ground");
          break;

        case ' ':
          PlaceGroundTile(x, y, image, GlobalConstants::BlackColor, GlobalConstants::StoneColor, "Stone");
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
          PlaceGroundTile(x, y, '.', GlobalConstants::DirtDotColor, GlobalConstants::DirtColor, "Dirt");
          break;

        case 'F':
          t.Set(true, false, image, GlobalConstants::WhiteColor, GlobalConstants::DeepWaterColor, "Fountain");
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
    auto shrineColor = GlobalConstants::ShrineColorsByType.at(shrineType);

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
    "Lost City level description goes here"
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "Lost City", msg);
}
