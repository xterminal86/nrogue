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
#include "logger.h"

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
        { 0,  _layoutsForLevel[0]  },
        { 60, _layoutsForLevel[1]  },
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
        { 0,  _layoutsForLevel[0]  },
        { 60, _layoutsForLevel[1]  },
        { 70, _layoutsForLevel[3]  },
        { 70, _layoutsForLevel[2]  },
        { 50, _layoutsForLevel[10] },
      };

      _monstersSpawnRateForThisLevel =
      {
        { GameObjectType::RAT, 10 },
        { GameObjectType::BAT,  1 }
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
        { GameObjectType::RAT,         25 },
        { GameObjectType::SPIDER,       2 },
        { GameObjectType::TROLL,        1 },
        { GameObjectType::VAMPIRE_BAT,  1 },
        { GameObjectType::BAT,          5 }
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
        { GameObjectType::RAT,         25 },
        { GameObjectType::SPIDER,       3 },
        { GameObjectType::TROLL,        1 },
        { GameObjectType::VAMPIRE_BAT,  1 },
        { GameObjectType::BAT,          5 }
      };
    }
    break;

    case MapType::MINES_5:
    {
      CM enc =
      {
        {
          { 3022680835, 620865566 },
        },
        {
          { 2382029827, 2520614657 },
          { 3000959246, 4076695811 },
          { 923711235,  3929957377 },
          { 3309145102, 3446480645 },
          { 936108610,  1631121923 },
          { 2692001806, 1594017290 },
          { 69092099,   3579280897 },
          { 2453722382, 2674718469 },
          { 3026943491, 2303951361 },
        },
        {
          { 2026657027, 2026736897 },
          { 3018641934, 3186789377 },
          { 780486684,  2876604417 },
          { 1577187342, 28237825   },
          { 411061507,  3062105345 },
          { 1722434318, 2690568962 },
          { 2282642179, 925348611  },
          { 3176350274, 3082192385 },
          { 4239038213, 4092408833 },
          { 1450604610, 1059405570 },
          { 1315574030, 2115398659 },
          { 2221331266, 4155598337 },
          { 4129736206, 1829500673 },
          { 3230723587, 254815491  },
          { 2216611598, 3647742721 },
          { 4222414595, 3110319873 },
          { 3303655950, 181524225  },
          { 4026109751, 3135913987 },
          { 586285070,  172185601  },
          { 2697643779, 3309185025 },
        },
        {
          { 2976433667, 151496961  },
          { 1175108366, 3602608387 },
          { 2661053955, 3475188225 },
          { 1955833870, 2562162690 },
          { 2958354179, 2594924291 },
          { 2456054082, 3709341443 },
          { 1476873230, 3753234947 },
          { 3958253378, 1623144195 },
          { 3368213251, 4158537475 },
          { 908132110,  3692665089 },
          { 3118180611, 2853779713 },
          { 4126005518, 3702354945 },
          { 4199177015, 1898380803 },
          { 2437964302, 3008347905 },
          { 2558113283, 1086596609 },
        },
        {
          { 3010945539, 2724230402 },
          { 2900488203, 1302238721 },
          { 1924685059, 3582486018 },
          { 2097174030, 2421996290 },
          { 1753176579, 1041227267 },
          { 3729220110, 2713691649 },
          { 3228288834, 2023849473 },
          { 783550990,  90538499   },
          { 1841855554, 3345537794 },
          { 1743815173, 1394245121 },
          { 2689885506, 1353708545 },
          { 2548642051, 1203459    },
          { 2128507406, 1115853313 },
          { 3044776195, 3716008193 },
          { 3775423246, 571655169  },
          { 1303498039, 737629443  },
          { 3429622286, 3815136001 },
          { 1304283651, 2576988161 },
        },
        {
          { 388466179,  3979940353 },
          { 3714237506, 1062774787 },
          { 1732752398, 4277681155 },
          { 135238978,  3933347331 },
          { 2925987598, 945271299  },
          { 3430614787, 3916272387 },
          { 2060534082, 717217539  },
          { 262559502,  3526315780 },
          { 1022215939, 1906273793 },
          { 1019151630, 1637103365 },
          { 521567235,  1352179713 },
        },
        {
          { 1995059203, 1758714369 },
          { 1058253378, 810720769  },
          { 3571154949, 3451347713 },
          { 3777681730, 1310825477 },
          { 3346198277, 2236341761 },
          { 4085699650, 153057794  },
          { 2434276878, 18541058   },
          { 4242600707, 2027836931 },
          { 2540308238, 1911383041 },
          { 377343042,  3155789313 },
          { 3190308366, 1327104259 },
          { 225742114,  3415728897 },
          { 1181964558, 784125185  },
          { 3714578724, 825987329  },
          { 2468126478, 351605763  },
          { 4133289758, 3282566913 },
          { 3876160270, 4144373505 },
          { 1964973571, 81001985   },
        },
        {
          { 511841027,  2845475841 },
          { 2125281090, 759723779  },
          { 2528491790, 1709896961 },
          { 1100048450, 3008720897 },
          { 1524086533, 571375105  },
          { 3324786498, 2881683459 },
          { 1388881422, 3665349122 },
          { 2339142210, 110513665  },
          { 2888023555, 4232555267 },
          { 3571116558, 4071227143 },
          { 1131838723, 3633986817 },
          { 2498592782, 1538992389 },
          { 272348419,  734124289  },
        },
        {
          { 2766580739, 1812994305 },
          { 357702158,  325776385  },
          { 965400642,  2124181249 },
          { 4059905294, 2187003138 },
          { 2204415810, 1525301762 },
          { 1749628931, 3453181699 },
          { 3424477198, 3199506433 },
          { 2644723010, 110199299  },
          { 213976590,  558211842  },
          { 1413561410, 2429833729 },
          { 21280526,   4179323394 },
          { 3454433795, 460902915  },
          { 966678030,  3017287425 },
          { 762389763,  1157462273 },
          { 896849422,  3439617025 },
          { 2753953335, 293707523  },
          { 3974023438, 1734951681 },
          { 2032090371, 95136257   },
        },
        {
          { 350665987,  239760385  },
          { 2477652802, 2417857281 },
          { 2930407694, 3125817092 },
          { 619711810,  2434712065 },
          { 4116793859, 4154244355 },
          { 2648342594, 444311810  },
          { 2617425413, 144653057  },
          { 2681923394, 1751577605 },
          { 3556091150, 1246286081 },
          { 2634401027, 2450582019 },
          { 3568811022, 1818722049 },
          { 2341749251, 1048642049 },
          { 954274574,  1651073025 },
          { 102095927,  2517213699 },
          { 4066378510, 1914814721 },
          { 3951750147, 2101362945 },
        },
        {
          { 472818947,  4013653505 },
          { 38814274,   3870804226 },
          { 670115342,  810810884  },
          { 204252675,  1740400643 },
          { 1350910222, 1087027969 },
          { 2429836610, 2942782725 },
          { 2737314565, 1063300097 },
          { 3726578498, 3208445698 },
          { 1805759747, 3612360451 },
          { 4069699086, 471115265  },
          { 3253708803, 4250305281 },
          { 3417619214, 3992175105 },
          { 3689299255, 1950083075 },
          { 3511735310, 442246657  },
          { 3122228483, 261214209  },
        },
        {
          { 1016322819, 558047489  },
          { 975477765,  4067055105 },
          { 2515084354, 941457154  },
          { 2421586702, 3873299464 },
          { 3475422530, 339076609  },
          { 3161091854, 3644450050 },
          { 3868568898, 451246595  },
          { 3782135310, 3520354565 },
          { 3823902723, 184683777  },
          { 2680606734, 3260949765 },
          { 2652166147, 447000833  },
        },
        {
          { 126771715, 3694579742 },
        },
      };

      _specialLevel = Util::DecodeMap(enc);

      /*
      _specialLevel =
      {
        "##############################",
        "#...#.....bbb..........#.....#",
        "#.<.#..###b%bb...b.###.#.WWW.#",
        "#...#..###bbb...bbb###.#.WWW.#",
        "##+##..###.b...bb%b###.#.WWW.#",
        "#bbb...bbb...###bbb....#.....#",
        "#b%bbbbb%bb..###.b...B.D...>.#",
        "#bbb.b%bbb..b###.......#.....#",
        "#.b..bb###.bbb..b..###.#.WWW.#",
        "#b....b###bb%bbbbb.###.#.WWW.#",
        "#bb....###.bbbbb%bb###.#.WWW.#",
        "#%bb........b..bbb.....#.....#",
        "##############################"
      };
      */

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
      lb.RoomsMethod(MapSize, { 45, 55 }, 7);
      break;

    case MapType::MINES_3:
    {
      int iterations = (MapSize.X * MapSize.Y) / 2;
      lb.TunnelerMethod(MapSize, iterations, { 5, 15 });
    }
    break;

    case MapType::MINES_4:
      lb.BacktrackingTunnelerMethod(MapSize, { 5, 10 }, { 1, 1 }, true);
      break;

    case MapType::MINES_5:
      CreateSpecialLevel();
      break;
  }

  if (MapType_ != MapType::MINES_5)
  {
    if (Util::Rolld100(_shrineRollChance))
    {
      PlaceRandomShrine(lb);
    }

    ConstructFromBuilder(lb);

    RecordEmptyCells();

    if (MapType_ == MapType::MINES_1 || MapType_ == MapType::MINES_2)
    {
      CreateRandomBoxes();
      RecordEmptyCells();
    }

    PlaceStairs();

    CreateInitialMonsters();

    int itemsToCreate = GetEstimatedNumberOfItemsToCreate();
    CreateItemsForLevel(itemsToCreate);
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
          PlaceWall(x, y, ' ', GlobalConstants::BlackColor, GlobalConstants::MountainsColor, "Mine Wall");
          break;

        case '+':
        {
          GameObject* door = GameObjectsFactory::Instance().CreateDoor(x, y, false, DoorMaterials::WOOD);

          if (Util::Rolld100(15))
          {
            DoorComponent* dc = door->GetComponent<DoorComponent>();
            dc->OpenedBy = GlobalConstants::OpenedByNobody;
          }

          InsertStaticObject(door);
        }
        break;

        case '.':
          PlaceGroundTile(x, y, image, GlobalConstants::GroundColor, GlobalConstants::BlackColor, "Dirt");
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
          PlaceShrine({ x, y, }, lb);
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
          GameObject* door = GameObjectsFactory::Instance().CreateDoor(posX, posY, false, DoorMaterials::WOOD);

          DoorComponent* dc = door->GetComponent<DoorComponent>();

          // This door should be kicked out, so OpenedBy doesn't matter
          dc->OpenedBy = GlobalConstants::OpenedByNobody;

          InsertStaticObject(door);
        }
        break;

        case 'D':
        {
          GameObject* door = GameObjectsFactory::Instance().CreateDoor(posX, posY, false, DoorMaterials::IRON, "Iron Door", -1, GlobalConstants::BlackColor, GlobalConstants::IronColor);

          DoorComponent* dc = door->GetComponent<DoorComponent>();
          dc->OpenedBy = key->GetComponent<ItemComponent>()->Data.ItemTypeHash;

          InsertStaticObject(door);
        }
        break;

        case 'B':
        {
          PlaceGroundTile(posX, posY, '.', GlobalConstants::GroundColor, GlobalConstants::BlackColor, "Ground");

          GameObject* boss = GameObjectsFactory::Instance().CreateMonster(posX, posY, GameObjectType::HEROBRINE);

          ContainerComponent* cc = boss->GetComponent<ContainerComponent>();
          cc->Add(key);

          InsertActor(boss);
        }
        break;

        case 'b':
          PlaceGroundTile(posX, posY, '.', GlobalConstants::RedColor, GlobalConstants::BlackColor, "Blood");
          break;

        case '%':
          PlaceGroundTile(posX, posY, c, GlobalConstants::WhiteColor, GlobalConstants::RedColor, "Someone's Remains");
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

void MapLevelMines::CreateRandomBoxes()
{
  //auto curLvl = Map::Instance().CurrentLevel;

  auto emptyCellsCopy = _emptyCells;

  int maxAttempts = 5;
  int maxBoxes = 8;

  for (int i = 0; i < maxAttempts; i++)
  {
    // Calculate range to check needed for required amount of barrels
    size_t boxesNum = RNG::Instance().RandomRange(1, maxBoxes + 1);
    int squareSideLengthRequired = (int)std::ceil(std::sqrt(boxesNum));
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
    if (res.size() >= boxesNum)
    {
      // res.size() is a minimum required square to put numBarrels into,
      // which may be significantly larger than amount of barrels to create
      // (e.g. 10 barrels can only fit in 5x5=25 square area around point),
      // so we need to check the number of barrels created so far separately.
      size_t created = 0;
      for (auto& p : res)
      {
        if (created >= boxesNum)
        {
          break;
        }

        GameObject* box = GameObjectsFactory::Instance().CreateBreakableObjectWithRandomLoot(p.X, p.Y, 'B', "Wooden Box", GlobalConstants::WoodColor, GlobalConstants::BlackColor);
        InsertStaticObject(box);
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

