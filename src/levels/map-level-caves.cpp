#include "map-level-caves.h"

#include "application.h"
#include "game-objects-factory.h"
#include "game-object-info.h"
#include "door-component.h"
#include "logger.h"
#include "printer.h"

MapLevelCaves::MapLevelCaves(int sizeX, int sizeY, MapType type, int dungeonLevel) :
  MapLevelBase(sizeX, sizeY, type, dungeonLevel)
{
  switch (MapType_)
  {
    case MapType::CAVES_1:
    {
      _monstersSpawnRateForThisLevel =
      {
        { GameObjectType::KOBOLD,    10 },
        { GameObjectType::MAD_MINER, 5  },
        { GameObjectType::SPIDER,    2  },
        { GameObjectType::TROLL,     1  }
      };
    }
    break;

    case MapType::CAVES_2:
    {
      _monstersSpawnRateForThisLevel =
      {
        { GameObjectType::KOBOLD,    10 },
        { GameObjectType::ZOMBIE,    5  },
        { GameObjectType::SPIDER,    1  },
        { GameObjectType::SKELETON,  5  }
      };
    }
    break;

    case MapType::CAVES_3:
    case MapType::CAVES_4:
    {
      _monstersSpawnRateForThisLevel =
      {
        { GameObjectType::KOBOLD,    5  },
        { GameObjectType::ZOMBIE,    10 },
        { GameObjectType::SKELETON,  10 }
      };
    }
    break;

    case MapType::CAVES_5:
    {
      _specialLevel =
      {
        "#####################################",
        "#....###...#.......####.......#.....#",
        "#.##.....#.#.#.###..#...#.###.#.###.#",
        "#..##.##.....#.####.#.#.....#...#...#",
        "##.##.####.#.#........##.##.#.###.#.#",
        "##....#....#....##.##.....#.......#.#",
        "##.#....##...##.#...#.###.#.##.#.##.#",
        "##.#.##.####.##...<...#.....#.......#",
        "#..#..#..#......#...#.#.###.#.##.####",
        "#.###.#..#.#.##.##.##..............##",
        "#............##....##.#.###.#.####.##",
        "#.#.###.###.##..##....#.....#....#..#",
        "#.#...#......#.###.#..#..##.#.##.##.#",
        "#...#...####.......##...###.........#",
        "#D###################################",
        "#>###################################",
        "#####################################"
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
  MonstersRespawnTurns = GlobalConstants::MonstersRespawnTimeout;

  int tunnelLengthMax = MapSize.X / 10;
  int tunnelLengthMin = tunnelLengthMax / 2;

  GameObjectInfo t;
  t.Set(true, true, ' ', Colors::BlackColor, Colors::CaveWallColor, Strings::TileNames::CaveWallText);

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
    if (Util::Instance().Rolld100(_shrineRollChance))
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
          PlaceWall(x,
                    y,
                    ' ',
                    Colors::BlackColor,
                    Colors::CaveWallColor,
                    Strings::TileNames::CaveWallText);
          break;

        case '+':
        {
          GameObject* door = GameObjectsFactory::Instance().CreateDoor(x, y, false);

          if (Util::Instance().Rolld100(15))
          {
            DoorComponent* dc = door->GetComponent<DoorComponent>();
            dc->OpenedBy = GlobalConstants::OpenedByNobody;
          }

          PlaceStaticObject(door);
        }
        break;

        case '.':
          PlaceGroundTile(x,
                          y,
                          image,
                          Colors::ShadesOfGrey::Four,
                          Colors::BlackColor,
                          Strings::TileNames::StoneFloorText);
          break;

        case 'g':
          PlaceGrassTile(x, y);
          break;

        case 'w':
          PlaceDeepWaterTile(x, y);
          break;

        case ' ':
          PlaceGroundTile(x,
                          y,
                          '.',
                          Colors::BlackColor,
                          Colors::ShadesOfGrey::Ten,
                          Strings::TileNames::StoneText);
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
  MysteriousForcePresent = true;

  auto convLevel = Util::Instance().StringsArray2DToCharArray2D(_specialLevel);

  MapType stairsDownTo = (MapType)(DungeonLevel + 1);
  MapType stairsUpTo = (MapType)(DungeonLevel - 1);

  int posX = 0;
  int posY = 0;

  const int startX = 18;
  const int startY = 7;

  GameObjectsFactory::Instance().CreateTrigger(TriggerType::ONE_SHOT,
                                               TriggerUpdateType::FINISH_TURN,
  [this, startX, startY]()
  {
    return !(_playerRef->PosX == startX
          && _playerRef->PosY == startY);
  },
  [this, startX, startY]()
  {
    GameObjectInfo t;
    t.Set(true,
          true,
          '#',
          Colors::ObsidianColorHigh,
          Colors::ObsidianColorLow,
          Strings::TileNames::ObsidianWallText);
    PlaceStaticObject(startX, startY, t, -1);
    Printer::Instance().AddMessage("Suddenly the stairs slide up!");
  });

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

        case 'D':
        {
          objName = Strings::TileNames::ObsidianWallText;
          t.Set(true,
                true,
                '#',
                Colors::ObsidianColorHigh,
                Colors::ObsidianColorLow,
                objName);
          PlaceStaticObject(posX, posY, t, -1);

          // TODO: add trigger to destroy on boss death
        }
        break;

        case '#':
        {
          objName = Strings::TileNames::ObsidianWallText;
          t.Set(true,
                true,
                c,
                Colors::ObsidianColorHigh,
                Colors::ObsidianColorLow,
                objName);
          PlaceStaticObject(posX, posY, t, -1);
        }
        break;

        case 'W':
          PlaceDeepWaterTile(posX, posY);
          break;

        case '.':
          PlaceGroundTile(posX,
                          posY,
                          c,
                          Colors::ShadesOfGrey::Four,
                          Colors::BlackColor,
                          Strings::TileNames::GroundText);
          break;
      }

      posX++;
    }

    posX = 0;
    posY++;
  }
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

    auto line = Util::Instance().BresenhamLine(start, end);
    for (auto& p : line)
    {
      if (MapArray[p.X][p.Y]->Image == '.')
      {
        GameObjectInfo t;
        std::string objName = Strings::TileNames::ShallowWaterText;
        t.Set(false,
              false,
              '~',
              Colors::WhiteColor,
              Colors::ShallowWaterColor,
              objName);
        MapArray[p.X][p.Y]->MakeTile(t, GameObjectType::SHALLOW_WATER);
      }
    }
  }
}

void MapLevelCaves::DisplayWelcomeText()
{
  std::vector<std::string> msg =
  {
  // ======================================== <- 40
    "It seems there is a complex network of ",
    "caves located deep down under the mines",
    "some of which look like catacombs.     ",
    "Did miners accidentally stubmled upon  ",
    "them during their mining operations?   "
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "Caves of Circe", msg);
}
