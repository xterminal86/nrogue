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

MapLevelMines::MapLevelMines(int sizeX,
                             int sizeY,
                             MapType type,
                             int dungeonLevel) :
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
        { GameObjectType::RAT,       20 },
        { GameObjectType::BAT,        5 },
        { GameObjectType::SPIDER,     3 },
        { GameObjectType::MAD_MINER,  2 }
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
               Colors::Black,
               Strings::TileNames::Dirt,
               GraphicTiles::DIRT);

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

      int ind = Game::gRng.RandomRange(0, splitRatios.size());

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

  CreateBorders(' ',
                Colors::Black,
                Colors::ShadesOfGrey::Six,
                Strings::TileNames::Rocks,
                GraphicTiles::WALL_MINE);

  if (MapType_ != MapType::MINES_5)
  {
    TransformedRoomsWeights weights =
    {
      { TransformedRoom::EMPTY,   {  1, 0 } },
      { TransformedRoom::SHRINE,  {  5, 1 } },
      { TransformedRoom::STORAGE, { 10, 2 } },
      { TransformedRoom::FLOODED, {  3, 1 } },
    };

    lb.TransformRooms(weights);

    ConstructFromBuilder(lb);

    PostProcessWalls(lb);

    RecordEmptyCells();

    PlaceStairs();

    CreateInitialMonsters();

    //int itemsToCreate = GetEstimatedNumberOfItemsToCreate();
    //CreateItemsForLevel(itemsToCreate);
  }
}

// =============================================================================

void MapLevelMines::PostProcessWalls(LevelBuilder& lb)
{
  for (int x = 0; x < MapSize.X; x++)
  {
    for (int y = 0; y < MapSize.Y; y++)
    {
      auto map = lb.GetMapRaw();

      if ((y + 1) <= (MapSize.Y - 1))
      {
        if (StaticMapObjects[x][y] != nullptr && map[x][y] == '#')
        {
          if (StaticMapObjects[x][y + 1] == nullptr 
          || (StaticMapObjects[x][y + 1] != nullptr 
           && StaticMapObjects[x][y + 1]->Image != StaticMapObjects[x][y]->Image))
          {
            StaticMapObjects[x][y]->Graphic.Tile = Util::Rolld100(50) ? 
                                                   GraphicTiles::WALL_MINE :
                                                   GraphicTiles::WALL_MINE2;
          }
        }
      }
    }
  }
}

// =============================================================================

void MapLevelMines::CreateSpecialLevel()
{
  MysteriousForcePresent = true;

  GameObject* key = Game::gIF.CreateDummyItem(
    "Iron Key",
    '1',
    Colors::Iron,
    Colors::Black,
    {
      "A simple iron key.",
      "Now you just need to find a lock."
    }
  );

  key->GetComponent<ItemComponent>()->Data.IsImportant = true;

  MapType stairsDownTo = (MapType)(DungeonLevel + 1);
  MapType stairsUpTo   = (MapType)(DungeonLevel - 1);

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

          Game::gGOF.CreateStairs(this,
                                  LevelStart.X,
                                  LevelStart.Y,
                                  c,
                                  stairsUpTo);
        }
        break;

        case '>':
        {
          LevelExit.X = posX;
          LevelExit.Y = posY;

          Game::gGOF.CreateStairs(this,
                                  LevelExit.X,
                                  LevelExit.Y,
                                  c,
                                  stairsDownTo);
        }
        break;

        case '#':
        {
          PlaceWall(posX,
                    posY,
                    ' ',
                    Colors::Black,
                    Colors::ShadesOfGrey::Six,
                    Strings::TileNames::MineWall,
                    false,
                    Util::Rolld100(50) ?
                    GraphicTiles::WALL_MINE :
                    GraphicTiles::WALL_MINE2);
        }
        break;

        case '+':
          PlaceDoor(posX,
                    posY,
                    false,
                    GlobalConstants::OpenedByAnyone,
                    std::string(),
                    DoorMaterials::STONE);
          break;

        case 'D':
        {
          GameObject* door =
              Game::gGOF.CreateDoor(posX,
                                    posY,
                                    false,
                                    DoorMaterials::IRON,
                                    "Iron Door",
                                    -1,
                                    Colors::Black,
                                    Colors::Iron);

          DoorComponent* dc = door->GetComponent<DoorComponent>();
          dc->OpenedBy = key->GetComponent<ItemComponent>()->Data.ItemTypeHash;

          dc->DoorType_ = DoorGraphicType::IRON;

          auto& dgbt = GlobalConstants::DoorGraphicsByType;
          door->Graphic.Tile = dgbt.at(dc->DoorType_)[2];

          PlaceStaticObject(door);
        }
        break;

        case 'B':
        {
          PlaceGroundTile(posX,
                          posY,
                          '.',
                          Colors::ShadesOfGrey::Four,
                          Colors::Black,
                          Strings::TileNames::Ground);

          GameObject* boss =
              Game::gMI.CreateMonster(posX,
                                      posY,
                                      GameObjectType::HEROBRINE);

          ContainerComponent* cc = boss->GetComponent<ContainerComponent>();
          cc->Add(key);

          PlaceActor(boss);

          Game::gGOF.CreateTrigger(
                TriggerType::ONE_SHOT,
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
                          Colors::Black,
                          Colors::ShadesOfGrey::Six,
                          Strings::TileNames::MineWall,
                          false,
                          GraphicTiles::GRAVEL);
              }
            }

            Game::gPrnt.AddMessage("The tunnel collapses!");

            GameObject* door = Game::gMap.GetStaticGameObjectAtPosition(2, 4);
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

            int index = Game::gRng.RandomRange(0, phrases.size());
            std::string phrase = phrases[index];

            Game::gPrnt.AddMessage(phrase,
                                   Colors::White,
                                   0xAA0000);
          });
        }
        break;

        case 'b':
        {
          if (Game::gApp.AppData.UseGraphicTiles)
          {
            PlaceGroundTile(posX,
                            posY,
                            c,
                            Colors::ShadesOfGrey::Four,
                            Colors::Black,
                            Strings::TileNames::Ground,
                            GraphicTiles::DIRT);

            GameObject* blood =
                Game::gGOF.CreateDummyObject(posX,
                                             posY,
                                             Strings::TileNames::Blood,
                                             '.',
                                             Colors::Red,
                                             Colors::None);

            static const std::unordered_map<GraphicTiles, int> bloodTiles =
            {
                {GraphicTiles::BLOOD_RED,    3 }
              , {GraphicTiles::BLOOD_SPLAT,  9 }
              , {GraphicTiles::BLOOD_SPLAT2, 9 }
            };

            auto r = Util::WeightedRandom(bloodTiles);

            blood->Graphic.Tile = r.first;

            PlaceGameObject(blood);
          }
          else
          {
            PlaceGroundTile(posX,
                            posY,
                            '.',
                            Colors::Red,
                            Colors::Black,
                            Strings::TileNames::Blood);
          }
        }
        break;

        case '%':
          PlaceGroundTile(posX,
                          posY,
                          c,
                          Colors::White,
                          Colors::Red,
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
                          Colors::Black,
                          Strings::TileNames::Ground,
                          GraphicTiles::DIRT);
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
         // ---------1---------2---------3---------4
    { HIDE("These mines once were a place of work ") },
    { HIDE("and income for this village. Now it's ") },
    { HIDE("just crumbling tunnels with occasional") },
    { HIDE("signs and markings here and there     ") },
    { HIDE("suggesting human presence in the past.") }
  };

  Game::gApp.ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT,
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
      int index = Game::gRng.RandomRange(0, _emptyCells.size());
      int x = _emptyCells[index].X;
      int y = _emptyCells[index].Y;
      if (!MapArray[x][y]->Occupied)
      {
        GameObject* m = Game::gMI.CreateMonster(x, y, GameObjectType::SHELOB);
        PlaceActor(m);
      }
    }
  }
}

// =============================================================================

void MapLevelMines::CreateCommonObjects(int x, int y, const CharV2& mapRaw)
{
  GameObjectInfo t;

  char image = mapRaw[x][y];

  switch (image)
  {
    case '#':
    {      
      GraphicTiles gt = Util::Rolld100(50) ? 
                        GraphicTiles::GRAVEL : 
                        GraphicTiles::GRAVEL2;
          
      PlaceWall(x,
                y,
                ' ',
                Colors::Black,
                Colors::ShadesOfGrey::Six,
                Strings::TileNames::MineWall,
                false,
                gt);
    }
    break;

    case '+':
      PlaceDoor(x,
                y,
                false,
                GlobalConstants::OpenedByAnyone,
                std::string(),
                DoorMaterials::WOOD,
                DoorGraphicType::WOOD);
      break;

    case '.':
      PlaceGroundTile(x,
                      y,
                      image,
                      Colors::ShadesOfGrey::Four,
                      Colors::Black,
                      Strings::TileNames::Dirt,
                      GraphicTiles::DIRT);
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
                      Colors::Black,
                      Colors::ShadesOfGrey::Ten,
                      Strings::TileNames::Stone,
                      GraphicTiles::STONE_TILES);
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
                      Colors::Black,
                      (image == '1') ?
                      Colors::ShadesOfGrey::Two :
                      Colors::ShadesOfGrey::Fourteen,
                      Strings::TileNames::TiledFloor,
                      GraphicTiles::TILES_BW);
      break;
  }
}

