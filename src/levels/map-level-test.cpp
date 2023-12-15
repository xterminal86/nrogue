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

  {
    GameObject* go = GameObjectsFactory::Instance().CreateChest(5, 5, false);
    PlaceGameObject(go);
  }

  {
    GameObject* go = GameObjectsFactory::Instance().CreateDoor(10, 10, false);
    PlaceStaticObject(go);
  }

  {
    GameObject* go = GameObjectsFactory::Instance().CreateDoor(10, 11, true);
    PlaceStaticObject(go);
  }
}

// =============================================================================

void MapLevelTest::CreateCommonObjects(int x, int y, char image)
{
}
