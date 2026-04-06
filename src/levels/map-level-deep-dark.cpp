#include "map-level-deep-dark.h"

#include "game-object-info.h"
#include "application.h"
#include "game-objects-factory.h"
#include "items-factory.h"
#include "door-component.h"

#ifdef DEBUG_BUILD
#include "logger.h"
#endif

MapLevelDeepDark::MapLevelDeepDark(int sizeX,
                                   int sizeY,
                                   MapType type,
                                   int dungeonLevel)
  : MapLevelBase(sizeX, sizeY, type, dungeonLevel)
{
  switch (MapType_)
  {
    case MapType::DEEP_DARK_5:
    {
      _specialLevel =
      {
        "########",
        "#......#",
        "#.<....#",
        "#......#",
        "########"
      };

      //
      // Note that x and y are swapped to correspond to "world" dimensions.
      //
      int sx = _specialLevel[0].length();
      int sy = _specialLevel.size();

      MapSize.Set(sx, sy);
    }
    break;
  }
}

// =============================================================================

void MapLevelDeepDark::PrepareMap()
{
  MapLevelBase::PrepareMap();

  CreateLevel();
}

// =============================================================================

void MapLevelDeepDark::CreateLevel()
{
  VisibilityRadius = 3;
  MonstersRespawnTurns = GlobalConstants::MonstersRespawnTimeout;

  CreateGround('.',
               Colors::ShadesOfGrey::Four,
               Colors::Black,
               Strings::TileNames::Ground);

  // NOTE: find out what was planned to do with these

  //int tunnelLengthMax = MapSize.X / 10;
  //int tunnelLengthMin = tunnelLengthMax / 2;

  LevelBuilder lb;

  switch (MapType_)
  {
    case MapType::DEEP_DARK_1:
    case MapType::DEEP_DARK_2:
    {
      int iterations = (MapSize.X * MapSize.Y);

      FeatureRoomsWeights weights =
      {
        { FeatureRoomType::EMPTY, { 10, 0 } }
      };

      lb.FeatureRoomsMethod(MapSize, { 1, 10 }, weights, 30, iterations);
    }
    break;

    case MapType::DEEP_DARK_3:
    {
      DGBase::RemovalParams params = { 6, 7, 3 };
      lb.RecursiveBacktrackerMethod(MapSize, { -1, -1 }, params);
    }
    break;

    case MapType::DEEP_DARK_4:
    {
      DGBase::RemovalParams params = { 5, 6, 3 };
      lb.RecursiveBacktrackerMethod(MapSize, { -1, -1 }, params);
    }
    break;

    case MapType::DEEP_DARK_5:
    {
      CreateSpecialLevel();
    }
    break;
  }

  CreateBorders(' ',
                Colors::Black,
                Colors::ShadesOfGrey::Six,
                Strings::TileNames::StoneWall);

  if (MapType_ != MapType::DEEP_DARK_5)
  {
    ConstructFromBuilder(lb);

    RecordEmptyCells();
    PlaceStairs();
    CreateInitialMonsters();
    CreateItemsForLevel(DungeonLevel + 6);
  }
}

// =============================================================================

void MapLevelDeepDark::CreateSpecialLevel()
{
  MysteriousForcePresent = true;

  GameObject* note = Game::gIF.CreateNote("A4 paper",
  {
    "Sorry, but the game is not finished yet.",
    "So this is as far as you can go. :-)"
  });

  note->PosX = 5;
  note->PosY = 2;

  PlaceGameObject(note);

  MapType stairsDownTo = (MapType)(DungeonLevel + 1);
  MapType stairsUpTo = (MapType)(DungeonLevel - 1);

  int posX = 0;
  int posY = 0;

  for (auto& line : _specialLevel)
  {
    for (auto& c : line)
    {
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
          PlaceWall(posX,
                    posY,
                    ' ',
                    Colors::Black,
                    Colors::ShadesOfGrey::Six,
                    Strings::TileNames::StoneWall,
                    false);
          break;

        case '.':
          PlaceGroundTile(posX,
                          posY,
                          c,
                          Colors::ShadesOfGrey::Four,
                          Colors::Black,
                          Strings::TileNames::Ground);
          break;
      }

      posX++;
    }

    posX = 0;
    posY++;
  }
}

// =============================================================================

void MapLevelDeepDark::CreateCommonObjects(int x, int y, const CharV2& mapRaw)
{
  char image = mapRaw[x][y];

  switch (image)
  {
    case '#':
      PlaceWall(x,
                y,
                ' ',
                Colors::Black,
                Colors::ShadesOfGrey::Six,
                Strings::TileNames::StoneWall,
                false);
      break;

    case '.':
      PlaceGroundTile(x,
                      y,
                      image,
                      Colors::ShadesOfGrey::Four,
                      Colors::Black,
                      Strings::TileNames::Ground);
      break;

    case '+':
    {
      GameObject* door =
          Game::gGOF.CreateDoor(x,
                                y,
                                false,
                                DoorMaterials::STONE);
      PlaceStaticObject(door);
    }
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
                      Colors::Black,
                      Colors::ShadesOfGrey::Ten,
                      Strings::TileNames::Stone);
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
                      Colors::ShadesOfGrey::Four :
                      Colors::ShadesOfGrey::Twelve,
                      Strings::TileNames::TiledFloor);
      break;
  }
}

// =============================================================================

void MapLevelDeepDark::DisplayWelcomeText()
{
  std::vector<std::string> msg =
  {
         // ---------0---------0---------0---------0 <- 40
    { HIDE("This is really surprizing, but it looks   ") },
    { HIDE("like Lost City itself continues even      ") },
    { HIDE("further deep into the ground.             ") },
    { HIDE("The thought makes you feel uneasy though, ") },
    { HIDE("as if history is repeating itself with    ") },
    { HIDE("the miners town above, preparing for it   ") },
    { HIDE("the same fate as befallen the Lost City...") }
  };

  Game::gApp.ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT,
                            { HIDE("Deep Dark") },
                             msg);
}

