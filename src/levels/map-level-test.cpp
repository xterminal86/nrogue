#include "map-level-test.h"

#include "game-object-info.h"

MapLevelTest::MapLevelTest(int sizeX, int sizeY, MapType type, int dungeonLevel)
  : MapLevelBase(sizeX, sizeY, type, dungeonLevel)
{
}

// =============================================================================

void MapLevelTest::PrepareMap()
{
  MapLevelBase::PrepareMap();
  CreateLevel();
}

// =============================================================================

void MapLevelTest::DisplayWelcomeText()
{
}

// =============================================================================

void MapLevelTest::CreateLevel()
{
  VisibilityRadius = 32;
  MonstersRespawnTurns = GlobalConstants::MonstersRespawnTimeout;

  CreateGround('.',
               Colors::ShadesOfGrey::Four,
               Colors::BlackColor,
               Strings::TileNames::GroundText);

  GameObjectInfo t;

  t.Set(true,
        true,
        '#',
        Colors::WhiteColor,
        Colors::ShadesOfGrey::Six,
        Strings::TileNames::RocksText);

  CreateBorders(t);
}

// =============================================================================

void MapLevelTest::CreateCommonObjects(int x, int y, char image)
{
}
