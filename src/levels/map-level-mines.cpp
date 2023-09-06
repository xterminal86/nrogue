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
          { 5667, 64030 },
        },
        {
          { 4899, 51201 },
          { 15918, 45827 },
          { 60963, 21505 },
          { 57902, 36101 },
          { 50530, 37379 },
          { 26926, 47626 },
          { 39203, 20225 },
          { 50990, 15109 },
          { 33827, 29697 },
        },
        {
          { 41507, 26113 },
          { 49198, 65281 },
          { 64828, 30209 },
          { 15150, 61953 },
          { 34339, 55041 },
          { 12334, 36354 },
          { 49699, 55555 },
          { 22370, 9985 },
          { 24357, 21761 },
          { 38754, 16386 },
          { 29486, 45571 },
          { 25442, 43265 },
          { 23854, 26881 },
          { 54563, 21251 },
          { 42030, 3329 },
          { 63011, 51457 },
          { 57902, 12289 },
          { 59991, 22787 },
          { 53038, 48641 },
          { 55843, 62465 },
        },
        {
          { 38947, 20993 },
          { 46638, 19203 },
          { 25123, 54273 },
          { 53294, 37378 },
          { 4387, 19971 },
          { 6498, 54531 },
          { 16686, 3 },
          { 53858, 49411 },
          { 6691, 46595 },
          { 32046, 46849 },
          { 13347, 54273 },
          { 46, 6913 },
          { 15959, 29955 },
          { 21550, 54017 },
          { 17955, 19969 },
        },
        {
          { 803, 43522 },
          { 48171, 3329 },
          { 58403, 33794 },
          { 14894, 61186 },
          { 31267, 39427 },
          { 2094, 2561 },
          { 46690, 2817 },
          { 18990, 39171 },
          { 4706, 46338 },
          { 11813, 64257 },
          { 62562, 54017 },
          { 10019, 58627 },
          { 2862, 65025 },
          { 45859, 64513 },
          { 35886, 33537 },
          { 35671, 25091 },
          { 50990, 56065 },
          { 45603, 38145 },
        },
        {
          { 31011, 53505 },
          { 59234, 52227 },
          { 11054, 56323 },
          { 46946, 50179 },
          { 35374, 63747 },
          { 46371, 2563 },
          { 62818, 3587 },
          { 24622, 24580 },
          { 29475, 41473 },
          { 7470, 3077 },
          { 9251, 21761 },
        },
        {
          { 36131, 17921 },
          { 20322, 26369 },
          { 27173, 58881 },
          { 6754, 7685 },
          { 44069, 38145 },
          { 8290, 33026 },
          { 12846, 6402 },
          { 3107, 33539 },
          { 31790, 19713 },
          { 58978, 28161 },
          { 15662, 12291 },
          { 20546, 31489 },
          { 41006, 50433 },
          { 28740, 19201 },
          { 34862, 15875 },
          { 43838, 11265 },
          { 16174, 21505 },
          { 33827, 15361 },
        },
        {
          { 31779, 19201 },
          { 610, 28675 },
          { 27950, 48897 },
          { 5986, 25089 },
          { 12069, 51457 },
          { 36962, 19971 },
          { 21806, 9474 },
          { 63586, 28929 },
          { 12579, 48131 },
          { 38446, 51719 },
          { 8483, 23297 },
          { 27182, 52229 },
          { 35363, 40449 },
        },
        {
          { 60451, 20225 },
          { 50478, 28161 },
          { 50530, 43009 },
          { 23598, 12034 },
          { 42594, 56834 },
          { 22051, 13571 },
          { 9006, 56577 },
          { 28770, 33795 },
          { 44334, 30210 },
          { 17762, 32257 },
          { 60462, 13826 },
          { 34339, 35843 },
          { 34862, 56577 },
          { 56099, 54529 },
          { 33582, 8449 },
          { 60503, 51203 },
          { 60462, 18689 },
          { 21027, 37889 },
        },
        {
          { 49187, 51969 },
          { 5218, 8961 },
          { 10542, 59396 },
          { 34658, 62465 },
          { 49699, 47875 },
          { 49762, 38658 },
          { 17701, 34305 },
          { 9570, 18949 },
          { 52782, 58625 },
          { 22563, 44035 },
          { 47662, 16385 },
          { 41507, 10753 },
          { 47406, 31233 },
          { 10327, 38659 },
          { 7470, 28161 },
          { 16419, 36353 },
        },
        {
          { 24867, 57857 },
          { 5474, 15362 },
          { 3374, 52740 },
          { 43043, 58627 },
          { 44334, 23809 },
          { 60002, 40197 },
          { 27173, 35585 },
          { 56418, 62466 },
          { 2339, 14083 },
          { 9518, 36097 },
          { 63011, 55809 },
          { 26414, 5889 },
          { 5207, 24323 },
          { 30254, 28161 },
          { 56867, 52993 },
        },
        {
          { 29219, 54529 },
          { 22565, 61953 },
          { 52578, 5634 },
          { 53038, 37384 },
          { 18530, 20737 },
          { 53550, 64514 },
          { 37474, 56835 },
          { 64814, 2309 },
          { 43811, 19713 },
          { 17966, 19461 },
          { 28707, 16641 },
        },
        {
          { 44067, 35870 },
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
      { TransformedRoom::FLOODED,   {  3, 1 } },
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

            const std::vector<CS> phrases =
            {
              {
                54350, 49743, 54103, 6432, 38217, 25120, 87, 64073, 56396,
                63564, 29984, 54856, 3905, 62550, 56645, 24096, 7245, 47961,
                3616, 32594, 64581, 57430, 44357, 16718, 39239, 14661, 33569,
              },
              {
                43865, 29263, 39253, 15904, 59212, 12869, 35142, 61268, 30240,
                15181, 50757, 16928, 41288, 29765, 55634, 6981, 14112, 18516,
                38735, 288, 33860, 32073, 53829, 28961,
              },
              {
                50497, 16488, 24366, 1838, 53806, 42528, 15174, 31858, 19301,
                47219, 14184, 800, 63085, 64101, 58209, 41332, 36444, 8993,
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
