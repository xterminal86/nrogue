#include "map-level-caves.h"

#include "application.h"
#include "game-objects-factory.h"
#include "game-object-info.h"
#include "door-component.h"
#include "logger.h"

MapLevelCaves::MapLevelCaves(int sizeX, int sizeY, MapType type, int dungeonLevel) :
  MapLevelBase(sizeX, sizeY, type, dungeonLevel)
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

void MapLevelCaves::PrepareMap(MapLevelBase* levelOwner)
{
  MapLevelBase::PrepareMap(levelOwner);

  CreateLevel();
}

void MapLevelCaves::CreateLevel()
{
  VisibilityRadius = 6;
  MonstersRespawnTurns = 1000;

  int tunnelLengthMax = MapSize.X / 10;
  int tunnelLengthMin = tunnelLengthMax / 2;

  GameObjectInfo t;
  t.Set(true, true, ' ', GlobalConstants::BlackColor, GlobalConstants::CaveWallColor, "Cave Wall");

  CreateBorders(t);

  LevelBuilder lb;

  switch (MapType_)
  {
    case MapType::CAVES_1:
    {
      int iterations = (MapSize.X * MapSize.Y) / 2;
      lb.TunnelerMethod(MapSize, iterations, { tunnelLengthMin, tunnelLengthMax });
    }
    break;

    case MapType::CAVES_2:
    {
      lb.RecursiveBacktrackerMethod(MapSize);
    }
    break;

    case MapType::CAVES_3:
    {
      RemovalParams params = { 7, 8, 2 };
      lb.RecursiveBacktrackerMethod(MapSize, { -1, -1 }, params);
    }
    break;

    case MapType::CAVES_4:
    {
      RemovalParams params = { 8, 8, 3 };
      lb.RecursiveBacktrackerMethod(MapSize, { -1, -1 }, params);
    }
    break;

    case MapType::CAVES_5:
    {
      CreateSpecialLevel();
    }
    break;
  }

  if (MapType_ != MapType::CAVES_5)
  {
    if (Util::Rolld100(_shrineRollChance))
    {
      PlaceRandomShrine(lb);
    }

    ConstructFromBuilder(lb);

    CreateRivers();
    RecordEmptyCells();
    PlaceStairs();
    CreateInitialMonsters();

    int itemsToCreate = GetEstimatedNumberOfItemsToCreate();
    CreateItemsForLevel(itemsToCreate);
  }
}

void MapLevelCaves::ConstructFromBuilder(LevelBuilder& lb)
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
          PlaceWall(x, y, ' ', GlobalConstants::BlackColor, GlobalConstants::CaveWallColor, "Cave Wall");
          break;

        case '+':
        {
          GameObject* door = GameObjectsFactory::Instance().CreateDoor(x, y, false);

          if (Util::Rolld100(15))
          {
            DoorComponent* dc = door->GetComponent<DoorComponent>();
            dc->OpenedBy = GlobalConstants::OpenedByNobody;
          }

          InsertStaticObject(door);
        }
        break;

        case '.':
          PlaceGroundTile(x, y, image, GlobalConstants::GroundColor, GlobalConstants::BlackColor, "Stone Floor");
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

void MapLevelCaves::CreateSpecialLevel()
{
  auto convLevel = Util::StringsArray2DToCharArray2D(_specialLevel);

  MapType stairsDownTo = (MapType)(DungeonLevel + 1);
  MapType stairsUpTo = (MapType)(DungeonLevel - 1);

  int posX = 0;
  int posY = 0;

  for (auto& row : convLevel)
  {
    for (auto& c : row)
    {
      GameObjectInfo t;
      std::string objName;

      switch (c)
      {
        case '<':
        {
          LevelStart.X = posX;
          LevelStart.Y = posY;

          GameObjectsFactory::Instance().CreateStairs(this, LevelStart.X, LevelStart.Y, c, stairsUpTo);
        }
        break;

        case '>':
        {
          LevelExit.X = posX;
          LevelExit.Y = posY;

          GameObjectsFactory::Instance().CreateStairs(this, LevelExit.X, LevelExit.Y, c, stairsDownTo);
        }
        break;

        case '#':
        {
          objName = "Cave Wall";
          t.Set(true, true, ' ', GlobalConstants::BlackColor, GlobalConstants::CaveWallColor, objName);
          InsertStaticObject(posX, posY, t, -1);
        }
        break;

        case 'W':
          PlaceDeepWaterTile(posX, posY);
          break;

        case '.':
          PlaceGroundTile(posX, posY, c, GlobalConstants::GroundColor, GlobalConstants::BlackColor, "Ground");
          break;
      }

      posX++;
    }

    posX = 0;
    posY++;
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

void MapLevelCaves::CreateRivers()
{
  int num = RNG::Instance().RandomRange(10, 21);

  for (int i = 0; i < num; i++)
  {
    Position start, end;

    bool isVertical = (RNG::Instance().RandomRange(0, 2) == 0);
    if (isVertical)
    {
      int x1 = RNG::Instance().RandomRange(0, MapSize.X);
      int x2 = RNG::Instance().RandomRange(0, MapSize.X);

      start.Set(0, x1);
      end.Set(MapSize.Y - 1, x2);
    }
    else
    {
      int y1 = RNG::Instance().RandomRange(0, MapSize.Y);
      int y2 = RNG::Instance().RandomRange(0, MapSize.Y);

      start.Set(y1, 0);
      end.Set(y2, MapSize.X - 1);
    }

    auto line = Util::BresenhamLine(start, end);
    for (auto& p : line)
    {
      if (MapArray[p.X][p.Y]->Image == '.')
      {
        GameObjectInfo t;
        std::string objName = "Shallow Water";
        t.Set(false, false, '~', GlobalConstants::WhiteColor, GlobalConstants::ShallowWaterColor, objName);
        MapArray[p.X][p.Y]->MakeTile(t, GameObjectType::SHALLOW_WATER);
      }
    }
  }
}
