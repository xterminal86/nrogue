#include "map-level-deep-dark.h"

#include "game-object-info.h"
#include "application.h"
#include "game-objects-factory.h"
#include "door-component.h"

MapLevelDeepDark::MapLevelDeepDark(int sizeX, int sizeY, MapType type, int dungeonLevel)
  : MapLevelBase(sizeX, sizeY, type, dungeonLevel)
{
  switch (MapType_)
  {
    case MapType::CAVES_5:
    {
      _specialLevel =
      {
        "########",
        "#......#",
        "#.<..>.#",
        "#......#",
        "########"
      };

      // Note that x and y are swapped to correspond to
      // "world" dimensions.
      int sx = _specialLevel[0].length();
      int sy = _specialLevel.size();

      MapSize.Set(sx, sy);
    }
    break;
  }
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
  VisibilityRadius = 3;
  MonstersRespawnTurns = 1000;

  int tunnelLengthMax = MapSize.X / 10;
  int tunnelLengthMin = tunnelLengthMax / 2;

  GameObjectInfo t;
  t.Set(true, true, ' ', GlobalConstants::BlackColor, GlobalConstants::MountainsColor, "Stone Wall");

  CreateBorders(t);

  LevelBuilder lb;

  switch (MapType_)
  {
    case MapType::DEEP_DARK_1:
    {
      int iterations = (MapSize.X * MapSize.Y); // / 10;

      FeatureRoomsWeights weights =
      {
        { FeatureRoomType::EMPTY,    { 10, 0 }  }
      };

      lb.FeatureRoomsMethod(MapSize, { 1, 10 }, weights, 3, iterations);
    }
    break;

    case MapType::DEEP_DARK_2:
    case MapType::DEEP_DARK_3:
    case MapType::DEEP_DARK_4:
    {
      Position start = { 1, 1 };
      lb.BacktrackingTunnelerMethod(MapSize, { 5, 10 }, start, true);
    }
    break;

    case MapType::DEEP_DARK_5:
    {
      CreateSpecialLevel();
    }
    break;
  }

  if (MapType_ != MapType::CAVES_5)
  {
    if (Util::Rolld100(50))
    {
      PlaceRandomShrine(lb);
    }

    ConstructFromBuilder(lb);

    RecordEmptyCells();
    PlaceStairs();
    CreateInitialMonsters();
    CreateItemsForLevel(DungeonLevel + 6);
  }
}

void MapLevelDeepDark::CreateSpecialLevel()
{
}

void MapLevelDeepDark::ConstructFromBuilder(LevelBuilder& lb)
{
  Logger::Instance().Print("********** INSTANTIATING LAYOUT **********");

  for (int x = 0; x < MapSize.X; x++)
  {
    for (int y = 0; y < MapSize.Y; y++)
    {
      char image = lb.MapRaw[x][y];
      switch (image)
      {
        case '#':
          PlaceWall(x, y, ' ', GlobalConstants::BlackColor, GlobalConstants::MountainsColor, "Stone Wall");
          break;

        case '.':
          PlaceGroundTile(x, y, image, GlobalConstants::GroundColor, GlobalConstants::BlackColor, "Ground");
          break;

        case '+':
        {
          GameObject* door = GameObjectsFactory::Instance().CreateDoor(x, y, false, "Door", 30);

          if (Util::Rolld100(15))
          {
            DoorComponent* dc = door->GetComponent<DoorComponent>();
            dc->OpenedBy = 0;
          }

          InsertStaticObject(door);
        }
        break;

        case 'g':
          PlaceGrassTile(x, y);
          break;

        case 'w':
          PlaceDeepWaterTile(x, y);
          break;

        case ' ':
          PlaceGroundTile(x, y, '.', GlobalConstants::BlackColor, GlobalConstants::StoneColor, "Stone");
          break;

        case 'l':
          PlaceLavaTile(x, y);
          break;

        case '/':
          PlaceShrine({ x, y }, lb);
          break;
      }
    }
  }
}
