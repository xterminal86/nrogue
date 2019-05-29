#include "map-level-lost-city.h"

#include "game-objects-factory.h"
#include "game-object-info.h"
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

void MapLevelLostCity::CreateLevel()
{
  VisibilityRadius = 12;
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
    { FeatureRoomType::SHRINE,   { 10, 1 }  }
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

  int itemsToCreate = RNG::Instance().RandomRange(1, 3 + DungeonLevel);
  CreateItemsForLevel(itemsToCreate);
}

void MapLevelLostCity::ConstructFromBuilder(LevelBuilder& lb)
{
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

        case '+':
        {
          GameObject* door = GameObjectsFactory::Instance().CreateDoor(x, y, false, "Door", 30);
          InsertStaticObject(door);
        }
        break;

        case 't':
        {
          objName = "Withered Tree";
          t.Set(true, true, 'T', GlobalConstants::DirtColor, GlobalConstants::BlackColor, objName);
          InsertStaticObject(x, y, t);
        }
        break;

        case 'T':
        {
          objName = "Tree";
          t.Set(true, true, 'T', GlobalConstants::TreeColor, GlobalConstants::BlackColor, objName);
          InsertStaticObject(x, y, t);
        }
        break;

        case '.':
        {
          objName = "Ground";
          t.Set(false, false, image, GlobalConstants::GroundColor, GlobalConstants::BlackColor, objName);
          MapArray[x][y]->MakeTile(t);
        }
        break;

        case ' ':
        {
          objName = "Stone";
          t.Set(false, false, image, GlobalConstants::BlackColor, GlobalConstants::StoneColor, objName);
          MapArray[x][y]->MakeTile(t);
        }
        break;

        // TODO: can step in lava and get killed / damaged?

        case 'l':
        {
          objName = "Lava";
          t.Set(true, false, '~', GlobalConstants::LavaWavesColor, GlobalConstants::LavaColor, objName);
          MapArray[x][y]->MakeTile(t);
        }
        break;

        case 'w':
        {
          objName = "Shallow Water";
          t.Set(false, false, '~', GlobalConstants::WhiteColor, GlobalConstants::ShallowWaterColor, objName);
          MapArray[x][y]->MakeTile(t);
        }
        break;

        case 'W':
        {
          objName = "Deep Water";
          t.Set(true, false, '~', GlobalConstants::ShallowWaterColor, GlobalConstants::DeepWaterColor, objName);
          MapArray[x][y]->MakeTile(t);
        }
        break;

        case 'd':
        {
          objName = "Dirt";
          t.Set(false, false, '.', GlobalConstants::DirtDotColor, GlobalConstants::DirtColor, objName);
          MapArray[x][y]->MakeTile(t);
        }
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
