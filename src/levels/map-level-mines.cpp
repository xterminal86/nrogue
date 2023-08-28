#include "map-level-mines.h"

#include "map.h"
#include "application.h"
#include "rng.h"
#include "constants.h"
#include "game-objects-factory.h"
#include "monsters-inc.h"
#include "items-factory.h"
#include "game-object-info.h"
#include "ai-component.h"
#include "ai-monster-herobrine.h"
#include "door-component.h"
#include "container-component.h"
#include "player.h"
#include "printer.h"

#ifdef DEBUG_BUILD
#include "logger.h"
#endif

MapLevelMines::MapLevelMines(int sizeX, int sizeY, MapType type, int dungeonLevel) :
  MapLevelBase(sizeX, sizeY, type, dungeonLevel)
{
  switch (MapType_)
  {
    case MapType::MINES_1:
    {
      _monstersSpawnRateForThisLevel =
      {
        { GameObjectType::RAT, 1 }
      };
    }
    break;

    case MapType::MINES_2:
    {
      _monstersSpawnRateForThisLevel =
      {
        { GameObjectType::RAT, 10 },
        { GameObjectType::BAT,  1 }
      };
    }
    break;

    case MapType::MINES_3:
    {
      _monstersSpawnRateForThisLevel =
      {
        { GameObjectType::RAT,         20 },
        { GameObjectType::BAT,          5 },
        { GameObjectType::SPIDER,       3 },
        { GameObjectType::MAD_MINER,    2 }
      };
    }
    break;

    case MapType::MINES_4:
    {
      _monstersSpawnRateForThisLevel =
      {
        { GameObjectType::RAT,         20 },
        { GameObjectType::BAT,          5 },
        { GameObjectType::SPIDER,       3 },
        { GameObjectType::MAD_MINER,    2 },
        { GameObjectType::VAMPIRE_BAT,  1 },
      };
    }
    break;

    case MapType::MINES_5:
    {
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

      const CM enc =
      {
        {
          { 1100686371, 2902467102 },
        },
        {
          { 844754723, 2330612737 },
          { 2173774126, 3155204099 },
          { 1650232355, 2730430209 },
          { 561364270, 140080645 },
          { 4178419554, 1690913795 },
          { 1910648622, 3089223434 },
          { 293025059, 4186444545 },
          { 575350830, 1535544581 },
          { 1878449187, 3202141953 },
        },
        {
          { 33175843, 1389273601 },
          { 1299735342, 123502593 },
          { 2119717180, 4248304641 },
          { 2019788078, 1880452097 },
          { 4260915235, 4035686913 },
          { 934253102, 4120204802 },
          { 1111239459, 1958347011 },
          { 1976511586, 3965882881 },
          { 2751142181, 4080930561 },
          { 4236055906, 2206600962 },
          { 811056430, 3881112323 },
          { 1956226658, 2733864449 },
          { 4183074606, 3605462529 },
          { 1532951331, 2940578051 },
          { 2100854062, 1355654913 },
          { 4006438947, 3461061377 },
          { 1073072686, 2225710081 },
          { 1848065111, 4251169027 },
          { 2631668014, 1022740225 },
          { 1212329251, 4094075905 },
        },
        {
          { 3853849123, 2781784833 },
          { 2676115758, 2808129283 },
          { 1838116387, 3772801025 },
          { 3640581678, 4132508930 },
          { 1298704419, 3860400643 },
          { 2725358690, 4038744323 },
          { 1983837230, 1748936195 },
          { 2040717410, 208457987 },
          { 1550782755, 2995046403 },
          { 483705134, 1666793473 },
          { 4045875747, 795142657 },
          { 2627741742, 1588930561 },
          { 3188359511, 1914007299 },
          { 2955932206, 1366330625 },
          { 4175439395, 2039476993 },
        },
        {
          { 3060245027, 1660489986 },
          { 3911932715, 1821840385 },
          { 2867807267, 2545548034 },
          { 199603246, 3949977090 },
          { 4288854307, 2172064003 },
          { 553305134, 3656223489 },
          { 3251797602, 2858849793 },
          { 1404173102, 1538169347 },
          { 1069524322, 682118146 },
          { 3282511141, 2735086593 },
          { 646133602, 3800940033 },
          { 3906754595, 2917602307 },
          { 2629109038, 852706561 },
          { 1178879779, 322381057 },
          { 1976301614, 1527793665 },
          { 4036236375, 2421243651 },
          { 3020831534, 3026369793 },
          { 1591949859, 1723408897 },
        },
        {
          { 1129028643, 3037383681 },
          { 3262271842, 3863909123 },
          { 1483820590, 1914421763 },
          { 3228665186, 228981507 },
          { 3357452334, 555879427 },
          { 1852688675, 1330138115 },
          { 4169085794, 2339464451 },
          { 1121661742, 3966120708 },
          { 2138382115, 3717154817 },
          { 169072430, 2625469957 },
          { 2628699171, 225357569 },
        },
        {
          { 392244003, 671480065 },
          { 2765695330, 4091867393 },
          { 737306661, 1019690497 },
          { 1740852322, 2701256965 },
          { 250056741, 1319842049 },
          { 3672121442, 918017794 },
          { 1276187182, 3913793538 },
          { 886184739, 3422026499 },
          { 1758015022, 462089217 },
          { 2658337122, 4129223425 },
          { 3417299758, 4242750979 },
          { 1990844482, 2083784961 },
          { 2542938670, 2316183553 },
          { 3649869124, 2673609473 },
          { 749110574, 2442817283 },
          { 1542334782, 189161729 },
          { 4234108718, 1470699777 },
          { 1298866211, 3128998145 },
        },
        {
          { 477106467, 3062432513 },
          { 3195403106, 3509053955 },
          { 4055472942, 426467073 },
          { 1307628386, 1519108609 },
          { 1758869285, 3395596801 },
          { 3279715682, 690991107 },
          { 1784193326, 3076014082 },
          { 1845998178, 3532221441 },
          { 1902382627, 3124284163 },
          { 2801631790, 4044427783 },
          { 403681571, 2972169729 },
          { 144773166, 2403024645 },
          { 2235841571, 2356893953 },
        },
        {
          { 3617852195, 2312902913 },
          { 953361966, 1580785921 },
          { 244720738, 540985857 },
          { 90060078, 629139202 },
          { 423320162, 277101314 },
          { 3865696547, 1350838531 },
          { 2256965934, 1345654529 },
          { 1835336802, 1484285187 },
          { 1427202094, 2458690050 },
          { 264150882, 3311671297 },
          { 3505314094, 2070533122 },
          { 4133147939, 2968323587 },
          { 4061793326, 2510901249 },
          { 3672693795, 1326399489 },
          { 3117492014, 3089483521 },
          { 1906132055, 4042855683 },
          { 1431531566, 164749569 },
          { 221223971, 396613889 },
        },
        {
          { 3779226659, 3571652097 },
          { 3076822882, 877788673 },
          { 1168387118, 455845636 },
          { 1776913762, 27021825 },
          { 2772636963, 1943667971 },
          { 2454651490, 2040583426 },
          { 334340389, 1709068289 },
          { 2393268578, 2352711685 },
          { 2925980718, 911867137 },
          { 66013475, 461225987 },
          { 2234846510, 3049959425 },
          { 1985137187, 2370249729 },
          { 723784750, 890271233 },
          { 859926359, 2195810563 },
          { 1136358958, 636651009 },
          { 3650903075, 2000005121 },
        },
        {
          { 3026121507, 3896669953 },
          { 4164112482, 330356994 },
          { 3251383086, 2495191556 },
          { 3264785187, 2125734147 },
          { 2329915950, 1058376449 },
          { 3070695778, 2204954629 },
          { 382394917, 4170640641 },
          { 7695202, 2131839234 },
          { 1919188003, 2497864451 },
          { 911698222, 303991041 },
          { 2655747875, 613425921 },
          { 2278102830, 3892763137 },
          { 640190807, 852488195 },
          { 3689793582, 755088385 },
          { 253623331, 797660161 },
        },
        {
          { 3755783459, 2904184833 },
          { 3081442853, 1170277377 },
          { 3229184610, 2061091330 },
          { 1348679214, 2834435080 },
          { 2121228130, 4069841665 },
          { 1172415790, 847531266 },
          { 459860578, 1073412611 },
          { 3481756718, 728287749 },
          { 2580417571, 1728145921 },
          { 3397873454, 1604151301 },
          { 1553052963, 1763481857 },
        },
        {
          { 1589450275, 3345828638 },
        },
      };

      _specialLevel = Util::DecodeMap(enc);

      // Note that x and y are swapped to correspond to
      // "world" dimensions.
      int sx = _specialLevel[0].length();
      int sy = _specialLevel.size();

      MapSize.Set(sx, sy);
    }
    break;
  }
}

// =============================================================================

void MapLevelMines::PrepareMap(MapLevelBase* levelOwner)
{
  MapLevelBase::PrepareMap(levelOwner);

  CreateLevel();
}

// =============================================================================

void MapLevelMines::CreateLevel()
{
  VisibilityRadius = 8;
  MonstersRespawnTurns = GlobalConstants::MonstersRespawnTimeout;

  GameObjectInfo t;
  t.Set(false,
        false,
        '.',
        Colors::ShadesOfGrey::Four,
        Colors::BlackColor,
        Strings::TileNames::DirtText);

  FillArea(0, 0, MapSize.X - 1, MapSize.Y - 1, t);

  // Build level

  LevelBuilder lb;

  const std::vector<Position> splitRatios =
  {
    { 30, 70 },
    { 70, 30 },
    { 45, 55 },
    { 60, 40 },
    { 40, 60 }
  };

  int ind = RNG::Instance().RandomRange(0, splitRatios.size());

  switch (MapType_)
  {
    case MapType::MINES_1:
    case MapType::MINES_2:
      lb.BSPRoomsMethod(MapSize, splitRatios[ind], 7);
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

  // Borders

  t.Set(true,
        true,
        ' ',
        Colors::BlackColor,
        Colors::ShadesOfGrey::Six,
        Strings::TileNames::RocksText);

  CreateBorders(t);

  if (MapType_ != MapType::MINES_5)
  {
    TransformedRoomsWeights weights =
    {
      { TransformedRoom::EMPTY,     {  1, 0 } },
      { TransformedRoom::TREASURY,  {  5, 1 } },
      { TransformedRoom::SHRINE,    {  5, 1 } },
      { TransformedRoom::STORAGE,   { 10, 2 } },
    };

    lb.TransformRooms(weights);

    ConstructFromBuilder(lb);

    RecordEmptyCells();

    PlaceStairs();

    CreateInitialMonsters();

    //int itemsToCreate = GetEstimatedNumberOfItemsToCreate();
    //CreateItemsForLevel(itemsToCreate);
  }
}

// =============================================================================

void MapLevelMines::CreateSpecialLevel()
{
  MysteriousForcePresent = true;

  GameObject* key = ItemsFactory::Instance().CreateDummyItem("Iron Key",
                                                             '1',
                                                             Colors::IronColor,
                                                             Colors::BlackColor,
                                                             {
                                                               "A simple iron key.",
                                                               "Now you just need to find a lock."
                                                             }
                                                             );

  key->GetComponent<ItemComponent>()->Data.IsImportant = true;

  MapType stairsDownTo = (MapType)(DungeonLevel + 1);
  MapType stairsUpTo = (MapType)(DungeonLevel - 1);

  int posX = 0;
  int posY = 0;

  for (auto& line : _specialLevel)
  {
    for (auto& c : line)
    {
      GameObjectInfo t;

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
          PlaceWall(posX,
                    posY,
                    ' ',
                    Colors::BlackColor,
                    Colors::ShadesOfGrey::Six,
                    Strings::TileNames::MineWallText);
        }
        break;

        case '+':
        {
          GameObject* door = GameObjectsFactory::Instance().CreateDoor(posX, posY, false, DoorMaterials::STONE);

          DoorComponent* dc = door->GetComponent<DoorComponent>();

          dc->OpenedBy = GlobalConstants::OpenedByAnyone;

          PlaceStaticObject(door);
        }
        break;

        case 'D':
        {
          GameObject* door = GameObjectsFactory::Instance().CreateDoor(posX, posY, false, DoorMaterials::IRON, "Iron Door", -1, Colors::BlackColor, Colors::IronColor);

          DoorComponent* dc = door->GetComponent<DoorComponent>();
          dc->OpenedBy = key->GetComponent<ItemComponent>()->Data.ItemTypeHash;

          PlaceStaticObject(door);
        }
        break;

        case 'B':
        {
          PlaceGroundTile(posX,
                          posY,
                          '.',
                          Colors::ShadesOfGrey::Four,
                          Colors::BlackColor,
                          Strings::TileNames::GroundText);

          GameObject* boss = MonstersInc::Instance().CreateMonster(posX, posY, GameObjectType::HEROBRINE);

          ContainerComponent* cc = boss->GetComponent<ContainerComponent>();
          cc->Add(key);

          PlaceActor(boss);

          GameObjectsFactory::Instance().CreateTrigger(TriggerType::ONE_SHOT,
                                                       TriggerUpdateType::FINISH_TURN,
          [this]()
          {
            // Mark area where trigger shouldn't activate ...
            bool activate = (_playerRef->PosX >= 1 && _playerRef->PosX <= 4
                          && _playerRef->PosY >= 1 && _playerRef->PosY <= 4);

            // ... and set it to activate everywhere else!
            return !activate;
          },
          [this, boss]()
          {
            // Place cave-in
            for (int x = 1; x <= 3; x++)
            {
              for (int y = 1; y <= 3; y++)
              {
                PlaceWall(x,
                          y,
                          ' ',
                          Colors::BlackColor,
                          Colors::ShadesOfGrey::Six,
                          Strings::TileNames::MineWallText);
              }
            }

            Printer::Instance().AddMessage("The tunnel collapses!");

            GameObject* door = Map::Instance().GetStaticGameObjectAtPosition(2, 4);
            if (door != nullptr)
            {
              DoorComponent* dc = door->GetComponent<DoorComponent>();
              dc->IsOpen = false;
              dc->UpdateDoorState();
              dc->OpenedBy = GlobalConstants::OpenedByNobody;
            }

            AIComponent* aic = boss->GetComponent<AIComponent>();
            aic->ChangeModel<AIMonsterHerobrine>();

            const std::vector<std::vector<uint32_t>> phrases =
            {
              {
                1213229858, 4201535321, 1152921679, 19612757, 3767896352, 2557531716, 1630884687, 807180878, 3459081511,
                3671298644, 730052640, 1675556171, 2571035214, 2806630223, 2298130007, 2349402400, 3461353815, 2169322056,
                3974235457, 202414932, 486150432, 1070465881, 1860540751, 2596601941, 1770402080, 1332773956, 2256168265,
                549008964, 3432337454, 858309422, 686783022, 4289955106
              },
              {
                1349546530, 2530016857, 3028347215, 1380253013, 1306811936, 810244684, 2514447429, 1740336454, 1665864276,
                1748796704, 2019555405, 2410630981, 2133631008, 1913010760, 3887764805, 4084491858, 2223104069, 3966115360,
                2862413652, 606298191, 3400308000, 1119918148, 3632087881, 3889731397, 4003210529, 2258919458
              },
              {
                3367896354, 3690556751, 1975509838, 1559405388, 3439394905, 191928864, 650194503, 1650704207, 1774788932,
                3471111200, 454396995, 3893791553, 3022789198, 265306400, 1436503880, 4053029445, 111928652, 4109268048,
                3060616224, 354137433, 3384928079, 3703348565, 2952175136, 749821262, 3727966543, 2536419671, 2996813089,
                1789173026
              }
            };

            int index = RNG::Instance().RandomRange(0, phrases.size());
            std::string phrase = Util::DecodeString(phrases[index]);

            Printer::Instance().AddMessage(phrase, Colors::WhiteColor, 0xAA0000);
          });
        }
        break;

        case 'b':
          PlaceGroundTile(posX,
                          posY,
                          '.',
                          Colors::RedColor,
                          Colors::BlackColor,
                          Strings::TileNames::BloodText);
          break;

        case '%':
          PlaceGroundTile(posX,
                          posY,
                          c,
                          Colors::WhiteColor,
                          Colors::RedColor,
                          "Someone's remains...");
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

// =============================================================================

void MapLevelMines::OnLevelChanged(MapType from)
{
  //
  // If we descended from from previous floor into boss room
  // remove wall on top of the stairs (if it's still there).
  // E.g., we defeated the boss, descended to the next dungeon floor,
  // then teleported back to town and went all the way to this level by stairs.
  //
  if (from == MapType::MINES_4 && MapType_ == MapType::MINES_5)
  {
    if (StaticMapObjects[2][2] != nullptr)
    {
      StaticMapObjects[2][2].reset();
    }
  }
}

// =============================================================================

void MapLevelMines::DisplayWelcomeText()
{
  std::vector<std::string> msg =
  {
  // 0--------1---------2---------3---------4 <- 40
    "These mines once were a place of work ",
    "and income for this village. Now it's ",
    "just crumbling tunnels with occasional",
    "signs and markings here and there     ",
    "suggesting human presence in the past."
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "Abandoned Mines", msg);
}

// =============================================================================

void MapLevelMines::CreateSpecialMonsters()
{
  if (MapType_ == MapType::MINES_3)
  {
    if (Util::Rolld100(50))
    {
      int index = RNG::Instance().RandomRange(0, _emptyCells.size());
      int x = _emptyCells[index].X;
      int y = _emptyCells[index].Y;
      if (!MapArray[x][y]->Occupied)
      {
        GameObject* m = MonstersInc::Instance().CreateMonster(x, y, GameObjectType::SHELOB);
        PlaceActor(m);
      }
    }
  }
}

// =============================================================================

void MapLevelMines::CreateCommonObjects(int x, int y, char image)
{
  GameObjectInfo t;

  switch (image)
  {
    case '#':
      PlaceWall(x,
                y,
                ' ',
                Colors::BlackColor,
                Colors::ShadesOfGrey::Six,
                Strings::TileNames::MineWallText);
      break;

    case '+':
    {
      GameObject* door = GameObjectsFactory::Instance().CreateDoor(x, y, false, DoorMaterials::WOOD);

      if (Util::Rolld100(10))
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
                      Strings::TileNames::DirtText);
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
                    ? Colors::ShadesOfGrey::Two
                    : Colors::ShadesOfGrey::Fourteen,
                      Strings::TileNames::TiledFloorText);
      break;
  }
}

// =============================================================================

void MapLevelMines::CreateSpecialObjects(int x, int y, const MapCell& cell)
{
  MapArray[x][y]->ZoneMarker = cell.ZoneMarker;

  switch (cell.ZoneMarker)
  {
    case TransformedRoom::SHRINE:
    {
      if (std::holds_alternative<ShrineType>(cell.ObjectHere))
      {
        ShrineType t = std::get<ShrineType>(cell.ObjectHere);
        PlaceShrine({ x, y }, t);
      }
    }
    break;

    case TransformedRoom::STORAGE:
    {
      if (std::holds_alternative<GameObjectType>(cell.ObjectHere))
      {
        if (std::get<GameObjectType>(cell.ObjectHere) == GameObjectType::CONTAINER)
        {
          GameObject* box = GameObjectsFactory::Instance().CreateBreakableObjectWithRandomLoot(x, y, 'B', "Wooden Box", Colors::WoodColor, Colors::BlackColor);
          PlaceStaticObject(box);
        }
      }
    }
    break;

    case TransformedRoom::TREASURY:
    {
      if (std::holds_alternative<ItemType>(cell.ObjectHere))
      {
        if(std::get<ItemType>(cell.ObjectHere) == ItemType::COINS)
        {
          GameObject* go = ItemsFactory::Instance().CreateMoney();
          go->PosX = x;
          go->PosY = y;
          PlaceGameObject(go);
        }
      }
    }
    break;
  }
}
