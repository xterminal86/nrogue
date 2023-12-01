#include "map-level-abyss.h"

#include "application.h"
#include "game-object-info.h"
#include "game-objects-factory.h"
#include "door-component.h"

#ifdef DEBUG_BUILD
#include "logger.h"
#endif

MapLevelAbyss::MapLevelAbyss(int sizeX, int sizeY, MapType type, int dungeonLevel)
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

  //
  // Note that x and y are swapped to correspond to
  // "world" dimensions.
  //
  int sx = _specialLevel[0].length();
  int sy = _specialLevel.size();

  MapSize.Set(sx, sy);
}

// =============================================================================

void MapLevelAbyss::PrepareMap()
{
  MysteriousForcePresent = true;

  MapLevelBase::PrepareMap();

  CreateLevel();
}

// =============================================================================

void MapLevelAbyss::CreateLevel()
{
  VisibilityRadius = 40;
  MonstersRespawnTurns = GlobalConstants::MonstersRespawnTimeout;

  CreateGround(' ',
               Colors::BlackColor,
               0x440000,
               Strings::TileNames::AbyssalFloorText);

  LevelBuilder lb;
  switch (MapType_)
  {
    case MapType::ABYSS_5:
      CreateSpecialLevel();
      break;

    default:
      lb.CellularAutomataMethod(MapSize, 40, 5, 4, 12);
      break;
  }

  CreateBorders(' ',
                Colors::BlackColor,
                Colors::ShadesOfGrey::Six,
                Strings::TileNames::AbyssalRocksText);

  if (MapType_ != MapType::ABYSS_5)
  {
    ConstructFromBuilder(lb);

    RecordEmptyCells();
    PlaceStairs();
  }
}

// =============================================================================

void MapLevelAbyss::CreateCommonObjects(int x, int y, char image)
{
  GameObjectInfo t;
  std::string objName;

  switch (image)
  {
    case '#':
    {
      objName = Strings::TileNames::AbyssalRocksText;
      t.Set(true, true, ' ', Colors::BlackColor, Colors::RedPoppyColor, objName);
      PlaceStaticObject(x, y, t, -1, GameObjectType::PICKAXEABLE);
    }
    break;

    case '+':
    {
      GameObject* door = GameObjectsFactory::Instance().CreateDoor(x, y, false);
      PlaceStaticObject(door);
    }
    break;

    case '.':
      PlaceGroundTile(x, y, ' ', Colors::BlackColor, 0x440000, Strings::TileNames::AbyssalFloorText);
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
      PlaceGroundTile(x, y, '.', Colors::BlackColor, Colors::ShadesOfGrey::Ten, Strings::TileNames::StoneText);
      break;

    case 'l':
      PlaceLavaTile(x, y);
      break;
  }
}

// =============================================================================

void MapLevelAbyss::DisplayWelcomeText()
{
  std::vector<std::string> msg =
  {
         // ---------0---------0---------0---------0 <- 40
    { HIDE("Now your surroundings are changed into") },
    { HIDE("shades of grey. Spacious chambers with") },
    { HIDE("darkness for the ceiling, ashen floors") },
    { HIDE("and walls incur feelings of sorrow,   ") },
    { HIDE("despair and oblivion.                 ") }
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT,
                                        { HIDE("Stygian Abyss") },
                                         msg);
}
