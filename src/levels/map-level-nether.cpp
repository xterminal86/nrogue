#include "map-level-nether.h"

#include "application.h"
#include "game-object-info.h"
#include "game-objects-factory.h"
#include "door-component.h"

#ifdef DEBUG_BUILD
#include "logger.h"
#endif

MapLevelNether::MapLevelNether(int sizeX,
                               int sizeY,
                               MapType type,
                               int dungeonLevel)
  : MapLevelBase(sizeX, sizeY, type, dungeonLevel)
{
  _specialLevel =
  {
    "########",
    "#......#",
    "#.<..>.#",
    "#......#",
    "########"
  };

  // Note that x and y are swapped to correspond to
  // "world" dimensions.
  int sx = _specialLevel[0].length();
  int sy = _specialLevel.size();

  MapSize.Set(sx, sy);
}

// =============================================================================

void MapLevelNether::PrepareMap()
{
  MysteriousForcePresent = true;

  MapLevelBase::PrepareMap();

  CreateLevel();
}

// =============================================================================

void MapLevelNether::CreateLevel()
{
  VisibilityRadius = 20;
  MonstersRespawnTurns = GlobalConstants::MonstersRespawnTimeout;

  CreateGround('.',
               0x440000,
               Colors::BlackColor,
               Strings::TileNames::HellstoneText);

  LevelBuilder lb;
  switch (MapType_)
  {
    case MapType::NETHER_5:
      CreateSpecialLevel();
      break;

    default:
      lb.CellularAutomataMethod(MapSize, 40, 5, 4, 12);
      break;
  }

  CreateBorders(' ',
                Colors::BlackColor,
                Colors::CaveWallColor,
                Strings::TileNames::CaveWallText);

  if (MapType_ != MapType::NETHER_5)
  {
    ConstructFromBuilder(lb);

    RecordEmptyCells();
    PlaceStairs();

    CreateInitialMonsters();
  }
}

// =============================================================================

void MapLevelNether::CreateCommonObjects(int x, int y, char image)
{
  GameObjectInfo t;
  std::string objName;

  switch (image)
  {
    case '#':
      PlaceWall(x,
                y,
                ' ',
                Colors::BlackColor,
                Colors::RedPoppyColor,
                Strings::TileNames::HellrockText);
      break;

    case '+':
    {
      GameObject* door =
          GameObjectsFactory::Instance().CreateDoor(x,
                                                    y,
                                                    false,
                                                    DoorMaterials::STONE);
      PlaceStaticObject(door);
    }
    break;

    case '.':
      PlaceGroundTile(x,
                      y,
                      image,
                      0x440000,
                      Colors::BlackColor,
                      Strings::TileNames::HellstoneText);
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

void MapLevelNether::DisplayWelcomeText()
{
  std::vector<std::string> msg =
  {
         // ---------0---------0---------0---------0 <- 40
    { HIDE("You don't believe your eyes.             ") },
    { HIDE("The very image of Hell that lives inside ") },
    { HIDE("the head of any commoner is materialized ") },
    { HIDE("before you. Pools of lava, fire and heat,") },
    { HIDE("creatures of various shapes and sizes    ") },
    { HIDE("that nobody ever imagined...             ") },
    { HIDE("What is this place?                      ") }
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT,
                                        { HIDE("Nether") },
                                         msg);
}
