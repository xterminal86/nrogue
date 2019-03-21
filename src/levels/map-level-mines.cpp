#include "map-level-mines.h"

#include "application.h"
#include "rng.h"
#include "constants.h"
#include "game-objects-factory.h"
#include "door-component.h"
#include "player.h"

MapLevelMines::MapLevelMines(int sizeX, int sizeY, MapType type, int dungeonLevel) :
  MapLevelBase(sizeX, sizeY, type, dungeonLevel)
{
  _layoutsForLevel =
  {
    // 0
    {
      ".........",
      ".........",
      "..#####..",
      "..#####..",
      "..#####..",
      "..#####..",
      "..#####..",
      ".........",
      ".........",
    },
    // 1
    {
      "#########",
      "##.....##",
      ".+.....+.",
      "##.....##",
      "##.....##",
      "##.....##",
      "##.....##",
      "####+####",
      "####.####",
    },
    // 2
    {
      ".........",
      ".#######.",
      ".#######.",
      ".#######.",
      ".#######.",
      ".#######.",
      ".#######.",
      ".#######.",
      ".........",
    },
    // 3
    {
      "#########",
      ".........",
      "####.####",
      "####.####",
      ".........",
      "####.####",
      "####.####",
      ".........",
      "#########",
    },
    // 4
    {
      "#.#####.#",
      ".........",
      "#.##.##.#",
      "#.##.##.#",
      ".........",
      "#.##.##.#",
      "#.##.##.#",
      ".........",
      "#.##.##.#",
    },
    // 5
    {
      "#########",
      "...##....",
      "#.###.###",
      "#.###.###",
      "#.......#",
      "####.##.#",
      "#..#.#..#",
      "...###...",
      "#.#####.#",
    },
    // 6
    {
      "#########",
      "#..##....",
      "..###.#.#",
      "#.#.#.#.#",
      "#.......#",
      "####.##..",
      "#..#.####",
      "...#.....",
      "#.##.##.#",
    },
    // 7
    {
      "#.#####.#",
      "#.#...#.#",
      "#.#.#.#.#",
      "#.#.#.#.#",
      "#.#.#.#.#",
      "#.#.#.#.#",
      "#.#.#.#.#",
      "#...#...#",
      "#########",
    },
    // 8
    {
      "####+####",
      "+.#...#.+",
      "#.#.#.#.#",
      "#.#.#.#.#",
      "#.#.#.#.#",
      "#.#...#.#",
      "#.##.##.#",
      "#.......#",
      "####+####",
    },
    // 9
    {
      "##.....##",
      "##.....##",
      "##.....##",
      "##.....##",
      "##.....##",
      "##.....##",
      "##.....##",
      "##.....##",
      "##.....##",
    },
    // 10
    {
      "#########",
      "#.......#",
      "#.......#",
      "#.......#",
      "+.......#",
      "#.......#",
      "#.......#",
      "#.......#",
      "#########",
    }
  };

  switch (MapType_)
  {
    case MapType::MINES_1:
    {
      _roomsForLevel =
      {
        { 0, _layoutsForLevel[0] },
        { 60, _layoutsForLevel[1] },
        { 50, _layoutsForLevel[10] },
      };

      _monstersSpawnRateForThisLevel =
      {
        { GameObjectType::RAT, 1 }
      };
    }
    break;

    case MapType::MINES_2:
    {
      _roomsForLevel =
      {
        { 0, _layoutsForLevel[0] },
        { 60, _layoutsForLevel[1] },
        { 70, _layoutsForLevel[3] },
        { 70, _layoutsForLevel[2] },
        { 50, _layoutsForLevel[10] },
      };

      _monstersSpawnRateForThisLevel =
      {
        { GameObjectType::RAT, 8 },
        { GameObjectType::BAT, 1 }
      };
    }
    break;

    case MapType::MINES_3:
    {
      _roomsForLevel =
      {
        { 0, _layoutsForLevel[0] },
        { 80, _layoutsForLevel[2] },
        { 80, _layoutsForLevel[3] },
        { 50, _layoutsForLevel[4] },
        { 50, _layoutsForLevel[9] }
      };

      _monstersSpawnRateForThisLevel =
      {
        { GameObjectType::RAT, 8 },
        { GameObjectType::SPIDER, 1 },
        { GameObjectType::BAT, 4 }
      };
    }
    break;

    case MapType::MINES_4:
    {
      _roomsForLevel =
      {
        { 0, _layoutsForLevel[2] },
        { 50, _layoutsForLevel[3] },
        { 50, _layoutsForLevel[4] },
        { 50, _layoutsForLevel[5] },
        { 50, _layoutsForLevel[6] },
        { 50, _layoutsForLevel[7] },
        { 50, _layoutsForLevel[8] }
      };

      _monstersSpawnRateForThisLevel =
      {
        { GameObjectType::RAT, 4 },
        { GameObjectType::SPIDER, 4 },
        { GameObjectType::BAT, 4 }
      };
    }
    break;

    case MapType::MINES_5:
    {
      _roomsForLevel =
      {
        { 0, _layoutsForLevel[2] },
        { 50, _layoutsForLevel[3] },
        { 50, _layoutsForLevel[4] },
        { 50, _layoutsForLevel[5] },
        { 50, _layoutsForLevel[6] },
        { 50, _layoutsForLevel[7] },
        { 50, _layoutsForLevel[8] }
      };

      _monstersSpawnRateForThisLevel =
      {
        { GameObjectType::RAT, 4 },
        { GameObjectType::SPIDER, 4 },
        { GameObjectType::BAT, 4 }
      };
    }
    break;
  }
}

void MapLevelMines::PrepareMap(MapLevelBase* levelOwner)
{
  MapLevelBase::PrepareMap(levelOwner);

  CreateLevel();  
}

void MapLevelMines::CreateLevel()
{
  VisibilityRadius = 8;
  MonstersRespawnTurns = 1000;

  GameObjectInfo t;
  t.Set(false, false, '.', GlobalConstants::GroundColor, GlobalConstants::BlackColor, "Dirt");

  FillArea(0, 0, MapSize.X - 1, MapSize.Y - 1, t);

  // Build level

  LevelBuilder lb;

  switch (MapType_)
  {
    case MapType::MINES_1:
    {
      lb.RoomsMethod(MapSize, { 45, 55 }, 7);
    }
    break;

    case MapType::MINES_2:
    case MapType::MINES_3:
    {
      int iterations = (MapSize.X * MapSize.Y) / 2;
      lb.TunnelerMethod(MapSize, iterations, { 5, 15 });
    }
    break;

    case MapType::MINES_4:
    case MapType::MINES_5:
    {
      lb.RecursiveBacktrackerMethod(MapSize);
    }
    break;
  }

  ConstructFromBuilder(lb);

  // Borders

  t.Set(true, true, ' ', GlobalConstants::BlackColor, GlobalConstants::MountainsColor, "Rocks");
  CreateBorders(t);

  RecordEmptyCells();

  PlaceStairs();

  CreateInitialMonsters();

  int itemsToCreate = RNG::Instance().RandomRange(1, 3 + DungeonLevel);
  CreateItemsForLevel(itemsToCreate);
}

void MapLevelMines::FillArea(int ax, int ay, int aw, int ah, const GameObjectInfo& tileToFill)
{
  for (int x = ax; x <= ax + aw; x++)
  {
    for (int y = ay; y <= ay + ah; y++)
    {
      MapArray[x][y]->MakeTile(tileToFill);
    }
  }
}

void MapLevelMines::ConstructFromBuilder(LevelBuilder& lb)
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
          InsertStaticObject(x, y, t, 1, GameObjectType::PICKAXEABLE);
        }
        break;

        case '+':
        {
          CreateDoor(x, y);
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

void MapLevelMines::DisplayWelcomeText()
{
  std::vector<std::string> msg =
  {
    "These mines once were a place of",
    "work and income for this village.",
    "Now it's just crumbling tunnels",
    "with occasional trails and marks here and there",
    "suggesting human presence in the past."
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "Abandoned Mines", msg);
}

void MapLevelMines::CreateDoor(int x, int y, bool isOpen)
{
  GameObject* door = GameObjectsFactory::Instance().CreateDoor(x, y, isOpen, "Door");
  InsertStaticObject(door);
}
