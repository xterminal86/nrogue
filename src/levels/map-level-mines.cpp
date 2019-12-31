#include "map-level-mines.h"

#include "map.h"
#include "application.h"
#include "rng.h"
#include "constants.h"
#include "game-objects-factory.h"
#include "game-object-info.h"
#include "door-component.h"
#include "container-component.h"
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
        { 0,  _layoutsForLevel[0] },
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
        { 0,  _layoutsForLevel[0] },
        { 60, _layoutsForLevel[1] },
        { 70, _layoutsForLevel[3] },
        { 70, _layoutsForLevel[2] },
        { 50, _layoutsForLevel[10] },
      };

      _monstersSpawnRateForThisLevel =
      {
        { GameObjectType::RAT, 6 },
        { GameObjectType::BAT, 2 }
      };
    }
    break;

    case MapType::MINES_3:
    {
      _roomsForLevel =
      {
        { 0,  _layoutsForLevel[0] },
        { 80, _layoutsForLevel[2] },
        { 80, _layoutsForLevel[3] },
        { 50, _layoutsForLevel[4] },
        { 50, _layoutsForLevel[9] }
      };

      _monstersSpawnRateForThisLevel =
      {
        { GameObjectType::RAT,    6 },
        { GameObjectType::SPIDER, 1 },
        { GameObjectType::TROLL,  1 },
        { GameObjectType::BAT,    4 }
      };
    }
    break;

    case MapType::MINES_4:
    {
      _roomsForLevel =
      {
        { 0,  _layoutsForLevel[2] },
        { 50, _layoutsForLevel[3] },
        { 50, _layoutsForLevel[4] },
        { 50, _layoutsForLevel[5] },
        { 50, _layoutsForLevel[6] },
        { 50, _layoutsForLevel[7] },
        { 50, _layoutsForLevel[8] }
      };

      _monstersSpawnRateForThisLevel =
      {
        { GameObjectType::RAT,    4 },
        { GameObjectType::SPIDER, 4 },
        { GameObjectType::TROLL,  2 },
        { GameObjectType::BAT,    4 }
      };
    }
    break;

    case MapType::MINES_5:
    {
      _specialLevel =
      {
        "##############################",
        "#...#..................#.....#",
        "#.<.#..###.........###.#.WWW.#",
        "#...#..###.........###.#.WWW.#",
        "##+##..###.........###.#.WWW.#",
        "#............###.......#.....#",
        "#............###.....b.D...>.#",
        "#............###.......#.....#",
        "#......###.........###.#.WWW.#",
        "#......###.........###.#.WWW.#",
        "#......###.........###.#.WWW.#",
        "#......................#.....#",
        "##############################"
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

  // Borders

  t.Set(true, true, ' ', GlobalConstants::BlackColor, GlobalConstants::MountainsColor, "Rocks");
  CreateBorders(t);

  // Build level

  LevelBuilder lb;

  switch (MapType_)
  {
    case MapType::MINES_1:
    case MapType::MINES_2:
    {
      lb.RoomsMethod(MapSize, { 45, 55 }, 7);
    }
    break;

    case MapType::MINES_3:
    case MapType::MINES_4:
    {
      int iterations = (MapSize.X * MapSize.Y) / 2;
      lb.TunnelerMethod(MapSize, iterations, { 5, 15 });
    }
    break;

    case MapType::MINES_5:
      CreateSpecialLevel();
      break;
  }

  if (MapType_ != MapType::MINES_5)
  {
    if (Util::Rolld100(50))
    {
      PlaceRandomShrine(lb);
    }

    ConstructFromBuilder(lb);

    RecordEmptyCells();

    if (MapType_ == MapType::MINES_1 || MapType_ == MapType::MINES_2)
    {
      CreateRandomBarrels();
      RecordEmptyCells();
    }

    PlaceStairs();

    CreateInitialMonsters();

    // FIXME: too many items?
    //int itemsToCreate = RNG::Instance().RandomRange(1, 6 + DungeonLevel);
    int itemsToCreate = DungeonLevel + 5;
    CreateItemsForLevel(itemsToCreate);    
  }
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
          PlaceWall(x, y, ' ', GlobalConstants::BlackColor, GlobalConstants::MountainsColor, "Rocks");
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

        case '.':
          PlaceGroundTile(x, y, image, GlobalConstants::GroundColor, GlobalConstants::BlackColor, "Ground");
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
        {
          ShrineType type = lb.ShrinesByPosition().at({ x, y });
          auto go = GameObjectsFactory::Instance().CreateShrine(x, y, type, 1000);
          InsertGameObject(go);

          std::string description = GlobalConstants::ShrineNameByType.at(type);
          t.Set(true, false, '/', GlobalConstants::GroundColor, GlobalConstants::BlackColor, description, "?Shrine?");
          InsertStaticObject(x, y, t);
        }
        break;
      }
    }
  }
}

void MapLevelMines::CreateSpecialLevel()
{  
  GameObject* key = GameObjectsFactory::Instance().CreateDummyObject("Iron Key",
                                                                     '1',
                                                                     GlobalConstants::IronColor,
                                                                     GlobalConstants::BlackColor,
                                                                     {
                                                                       "A simple iron key.",
                                                                       "Now you just need to find a lock."
                                                                     }
                                                                     );

  // Look for "NOTE:" in DoorComponent::Interact()
  key->GetComponent<ItemComponent>()->Data.IsImportant = true;

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
          objName = "Rocks";
          t.Set(true, true, ' ', GlobalConstants::BlackColor, GlobalConstants::MountainsColor, objName);
          InsertStaticObject(posX, posY, t, -1);
        }
        break;

        case '+':
        {
          GameObject* door = GameObjectsFactory::Instance().CreateDoor(posX, posY, false, "Wooden Door", 30);

          DoorComponent* dc = door->GetComponent<DoorComponent>();

          // This door should be kicked out, so OpenedBy doesn't matter
          dc->OpenedBy = 0;

          InsertStaticObject(door);
        }
        break;

        case 'D':
        {
          GameObject* door = GameObjectsFactory::Instance().CreateDoor(posX, posY, false, "Iron Door", -1, GlobalConstants::BlackColor, GlobalConstants::IronColor);

          DoorComponent* dc = door->GetComponent<DoorComponent>();
          dc->OpenedBy = key->GetComponent<ItemComponent>()->Data.ItemTypeHash;

          InsertStaticObject(door);          
        }
        break;

        case 'b':
        {
          objName = "Ground";
          t.Set(false, false, '.', GlobalConstants::GroundColor, GlobalConstants::BlackColor, objName);
          MapArray[posX][posY]->MakeTile(t);

          GameObject* boss = GameObjectsFactory::Instance().CreateMonster(posX, posY, GameObjectType::HEROBRINE);

          ContainerComponent* cc = boss->GetComponent<ContainerComponent>();
          cc->AddToInventory(key);

          InsertActor(boss);
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

void MapLevelMines::CreateRandomBarrels()
{
  auto curLvl = Map::Instance().CurrentLevel;

  auto emptyCellsCopy = _emptyCells;

  int maxAttempts = 10;
  int maxBarrels = 8;

  for (int i = 0; i < maxAttempts; i++)
  {
    // Calculate range to check needed for required amount of barrels
    int barrelsNum = RNG::Instance().RandomRange(1, maxBarrels + 1);
    int squareSideLengthRequired = (int)std::ceil(std::sqrt(barrelsNum));
    int rangeNeeded = (squareSideLengthRequired % 2 == 0)
                     ? squareSideLengthRequired / 2
                    : (squareSideLengthRequired - 1) / 2;

    if (rangeNeeded == 0)
    {
      rangeNeeded = 1;
    }

    int index = RNG::Instance().RandomRange(0, emptyCellsCopy.size());
    Position pos = emptyCellsCopy[index];    
    auto res = Map::Instance().GetEmptyCellsAround(pos, rangeNeeded);
    if (res.size() >= barrelsNum)
    {
      // res.size() is a minimum required square to put numBarrels into,
      // which may be significantly larger than amount of barrels to create
      // (e.g. 10 barrels can only fit in 5x5=25 in-game square area),
      // so we need to check the number of barrels created so far separately.
      int created = 0;
      for (auto& p : res)
      {
        if (created >= barrelsNum)
        {
          break;
        }

        GameObject* barrel = GameObjectsFactory::Instance().CreateBreakableObjectWithRandomLoot(p.X, p.Y, 'B', "Wooden Barrel", GlobalConstants::WoodColor, GlobalConstants::BlackColor);
        InsertStaticObject(barrel);
        created++;
      }

      emptyCellsCopy.erase(emptyCellsCopy.begin() + index);
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

