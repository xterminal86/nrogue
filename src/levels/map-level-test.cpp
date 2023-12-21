#include "map-level-test.h"

#include "game-object-info.h"
#include "game-objects-factory.h"

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
  VisibilityRadius = 16;
  MonstersRespawnTurns = GlobalConstants::MonstersRespawnTimeout;

  CreateGround('.',
               Colors::ShadesOfGrey::Four,
               Colors::BlackColor,
               Strings::TileNames::GroundText);

  CreateBorders('#',
                Colors::WhiteColor,
                Colors::ShadesOfGrey::Six,
                Strings::TileNames::RocksText);

  GameObject* go = GameObjectsFactory::Instance().CreateChest(5, 5, false);
  PlaceGameObject(go);

  PlaceDoor(10, 10, false);
  PlaceDoor(10, 11, true);

  PlaceShrine({ 13, 13 }, ShrineType::HEALING);
}

// =============================================================================

void MapLevelTest::CreateCommonObjects(int x, int y, char image)
{
}
