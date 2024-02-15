#include "map-level-caves.h"

#include "application.h"
#include "game-objects-factory.h"
#include "game-object-info.h"
#include "door-component.h"
#include "stairs-component.h"
#include "printer.h"

#ifdef DEBUG_BUILD
#include "logger.h"
#endif

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
        { HIDE("#####################################") },
        { HIDE("#....###...#.......####.......#.....#") },
        { HIDE("#.##.....#.#.#.###..#...#.###.#.###.#") },
        { HIDE("#..##.##.....#.####.#.#.....#...#...#") },
        { HIDE("##.##.####.#.#........##.##.#.###.#.#") },
        { HIDE("##....#....#....##.##.....#.......#.#") },
        { HIDE("##.#....##...##.#...#.###.#.##.#.##.#") },
        { HIDE("##.#.##.####.##...<...#.....#.......#") },
        { HIDE("#..#..#..#......#...#.#.###.#.##.####") },
        { HIDE("#.###.#..#.#.##.##.##..............##") },
        { HIDE("#............##....##.#.###.#.####.##") },
        { HIDE("#.#.###.###.##..##....#.....#....#..#") },
        { HIDE("#.#...#......#.###.#..#..##.#.##.##.#") },
        { HIDE("#...#...####.......##...###.........#") },
        { HIDE("#D###################################") },
        { HIDE("#>###################################") },
        { HIDE("#####################################") }
      };

      //
      // Note that x and y are swapped to correspond to
      // "world" dimensions.
      //
      int sx = _specialLevel[0].length();
      int sy = _specialLevel.size();

      MapSize.Set(sx, sy);
    }
    break;
  }
}

// =============================================================================

void MapLevelCaves::PrepareMap()
{
  MapLevelBase::PrepareMap();

  CreateLevel();
}

// =============================================================================

void MapLevelCaves::CreateLevel()
{
  VisibilityRadius = 6;
  MonstersRespawnTurns = GlobalConstants::MonstersRespawnTimeout;

  CreateGround('.',
               Colors::ShadesOfGrey::Four,
               Colors::BlackColor,
               Strings::TileNames::StoneFloorText);

  int tunnelLengthMax = 5; //MapSize.X / 10;
  int tunnelLengthMin = 1; //tunnelLengthMax / 2;

  LevelBuilder lb;

  switch (MapType_)
  {
    case MapType::CAVES_1:
    {
      //
      // To avoid possible degenerate case where start position happened
      // to be at the corner and direction to carve was chosen towards
      // the edge of the map, thus failing to generate anything.
      //
      int iterations = (MapSize.X * MapSize.Y) / 2;
      Position params = { tunnelLengthMin, tunnelLengthMax };
      lb.TunnelerMethod(MapSize, iterations, params);
    }
    break;

    case MapType::CAVES_2:
    {
      RemovalParams params = { 6, 8, 1 };
      lb.RecursiveBacktrackerMethod(MapSize, { -1, -1 }, params);
    }
    break;

    case MapType::CAVES_3:
    {
      RemovalParams params = { 6, 8, 2 };
      lb.RecursiveBacktrackerMethod(MapSize, { -1, -1 }, params);
    }
    break;

    case MapType::CAVES_4:
    {
      RemovalParams params = { 6, 7, 3 };
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
    CreateBorders(' ',
                  Colors::BlackColor,
                  Colors::CaveWallColor,
                  Strings::TileNames::CaveWallText);
  }
  else
  {
    CreateBorders('#',
                  Colors::ObsidianColorHigh,
                  Colors::ObsidianColorLow,
                  Strings::TileNames::ObsidianWallText);
  }

  if (MapType_ != MapType::CAVES_5)
  {
    ConstructFromBuilder(lb);

    CreateRivers();
    RecordEmptyCells();
    PlaceStairs();
    CreateInitialMonsters();

    int itemsToCreate = GetEstimatedNumberOfItemsToCreate();
    CreateItemsForLevel(itemsToCreate);
  }
}

// =============================================================================

void MapLevelCaves::CreateCommonObjects(int x, int y, char image)
{
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
      PlaceShallowWaterTile(x, y);
      break;

    case 'W':
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

    //
    // Black / white tiles.
    //
    case '1':
    case '2':
      PlaceGroundTile(x,
                      y,
                      ' ',
                      Colors::BlackColor,
                      (image == '1')
                    ? Colors::ShadesOfGrey::Four
                    : Colors::ShadesOfGrey::Twelve,
                      Strings::TileNames::TiledFloorText);
      break;
  }
}

// =============================================================================

void MapLevelCaves::CreateSpecialLevel()
{
  // TODO: create boss

  MysteriousForcePresent = true;

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
    //
    // TODO: restore back after boss death.
    //
    StairsComponent* sc = MapArray[startX][startY]->GetComponent<StairsComponent>();
    sc->OwnerGameObject->Image = '.';
    sc->OwnerGameObject->FgColor = Colors::ShadesOfGrey::Four;
    sc->OwnerGameObject->BgColor = Colors::BlackColor;
    sc->OwnerGameObject->ObjectName = Strings::TileNames::GroundText;
    sc->IsEnabled = false;

    Printer::Instance().AddMessage("Suddenly the stairs slide up!");
  });

  for (auto& line : _specialLevel)
  {
    for (auto& c : line)
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

        //
        // Door to next level stairs.
        //
        case 'D':
        {
          PlaceWall(posX,
                    posY,
                    '#',
                    Colors::ObsidianColorHigh,
                    Colors::ObsidianColorLow,
                    Strings::TileNames::ObsidianWallText,
                    true);

          // TODO: add trigger to destroy on boss death and restore stairs up.
        }
        break;

        case '#':
        {
          PlaceWall(posX,
                    posY,
                    c,
                    Colors::ObsidianColorHigh,
                    Colors::ObsidianColorLow,
                    Strings::TileNames::ObsidianWallText,
                    true);
        }
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

// =============================================================================

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

// =============================================================================

void MapLevelCaves::DisplayWelcomeText()
{
  std::vector<std::string> msg =
  {
         // ---------0---------0---------0---------0 <- 40
    { HIDE("It seems there is a complex network of ") },
    { HIDE("caves located deep down under the mines") },
    { HIDE("some of which look like catacombs.     ") },
    { HIDE("Did miners accidentally stubmled upon  ") },
    { HIDE("them during their mining operations?   ") }
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT,
                                        { HIDE("Caves of Circe") },
                                         msg);
}
