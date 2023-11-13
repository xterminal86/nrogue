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
      _specialLevel =
      {
        { HIDE("##############################") },
        { HIDE("#...#.....bbb..........#.....#") },
        { HIDE("#.<.#..###b%bb...b.###.#.WWW.#") },
        { HIDE("#...#..###bbb...bbb###.#.WWW.#") },
        { HIDE("##+##..###.b...bb%b###.#.WWW.#") },
        { HIDE("#bbb...bbb...###bbb....#.....#") },
        { HIDE("#b%bbbbb%bb..###.b...B.D...>.#") },
        { HIDE("#bbb.b%bbb..b###.......#.....#") },
        { HIDE("#.b..bb###.bbb..b..###.#.WWW.#") },
        { HIDE("#b....b###bb%bbbbb.###.#.WWW.#") },
        { HIDE("#bb....###.bbbbb%bb###.#.WWW.#") },
        { HIDE("#%bb........b..bbb.....#.....#") },
        { HIDE("##############################") }
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

// =============================================================================

void MapLevelMines::PrepareMap()
{
  MapLevelBase::PrepareMap();

  CreateLevel();
}

// =============================================================================

void MapLevelMines::CreateLevel()
{
  VisibilityRadius = 8;
  MonstersRespawnTurns = GlobalConstants::MonstersRespawnTimeout;

  CreateGround('.',
               Colors::ShadesOfGrey::Four,
               Colors::BlackColor,
               Strings::TileNames::DirtText);

  LevelBuilder lb;

  switch (MapType_)
  {
    case MapType::MINES_1:
      lb.RoomsMethod(MapSize, { 3, 7 }, MapSize.X);
      break;

    case MapType::MINES_2:
    {
      const std::vector<Position> splitRatios =
      {
        { 30, 70 },
        { 70, 30 },
        { 45, 55 },
        { 60, 40 },
        { 40, 60 }
      };

      int ind = RNG::Instance().RandomRange(0, splitRatios.size());

      lb.BSPRoomsMethod(MapSize, splitRatios[ind], 7);
    }
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

  GameObjectInfo t;

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
          GameObject* door = GameObjectsFactory::Instance().CreateDoor(posX,
                                                                       posY,
                                                                       false,
                                                                       DoorMaterials::IRON,
                                                                       "Iron Door",
                                                                       -1,
                                                                       Colors::BlackColor,
                                                                       Colors::IronColor);

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
            //
            // Mark area where trigger shouldn't activate...
            //
            bool activate = (_playerRef->PosX >= 1 && _playerRef->PosX <= 4
                          && _playerRef->PosY >= 1 && _playerRef->PosY <= 4);

            //
            // ...and set it to activate everywhere else!
            //
            return !activate;
          },
          [this, boss]()
          {
            //
            // Place cave-in.
            //
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

            const StringV phrases =
            {
              { HIDE("NOW I WILL HAVE MY REVENGE!") },
              { HIDE("YOU LEFT ME HERE TO DIE!")    },
              { HIDE("AH... FRESH MEAT!")           }
            };

            int index = RNG::Instance().RandomRange(0, phrases.size());
            std::string phrase = phrases[index];

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

        case 'w':
          PlaceShallowWaterTile(posX, posY);
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
         // ---------0---------0---------0---------0 <- 40
    { HIDE("These mines once were a place of work ") },
    { HIDE("and income for this village. Now it's ") },
    { HIDE("just crumbling tunnels with occasional") },
    { HIDE("signs and markings here and there     ") },
    { HIDE("suggesting human presence in the past.") }
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT,
                                        { HIDE("Abandoned Mines") },
                                         msg);
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

      //
      // May cause softlock if there is a locked door in flooded room
      // since we cannot attack while swimming.
      //

      /*
      if (Util::Rolld100(10))
      {
        DoorComponent* dc = door->GetComponent<DoorComponent>();
        dc->OpenedBy = GlobalConstants::OpenedByNobody;
      }
      */

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

