#include "map-level-lost-city.h"

#include "game-objects-factory.h"
#include "game-object-info.h"
#include "application.h"

#ifdef DEBUG_BUILD
#include "logger.h"
#endif

MapLevelLostCity::MapLevelLostCity(int sizeX, int sizeY, MapType type, int dungeonLevel)
  : MapLevelBase(sizeX, sizeY, type, dungeonLevel)
{
}

// =============================================================================

void MapLevelLostCity::PrepareMap(MapLevelBase* levelOwner)
{
  MapLevelBase::PrepareMap(levelOwner);

  CreateLevel();
}

// =============================================================================

void MapLevelLostCity::CreateLevel()
{
  VisibilityRadius = 20;
  MonstersRespawnTurns = GlobalConstants::MonstersRespawnTimeout;

  LevelBuilder lb;

  FeatureRoomsWeights weights =
  {
    { FeatureRoomType::EMPTY,    { 10, 0 } },
    { FeatureRoomType::DIAMOND,  {  3, 3 } },
    { FeatureRoomType::FLOODED,  {  1, 3 } },
    { FeatureRoomType::GARDEN,   {  3, 3 } },
    { FeatureRoomType::PILLARS,  {  3, 0 } },
    { FeatureRoomType::ROUND,    {  5, 3 } },
    { FeatureRoomType::POND,     {  3, 3 } },
    { FeatureRoomType::FOUNTAIN, {  3, 2 } },
    { FeatureRoomType::SHRINE,   { 10, 2 } }
  };

  Position roomSize = { 1, 10 };

  lb.FeatureRoomsMethod(MapSize, roomSize, weights, 3, MapSize.X * MapSize.Y);

  ConstructFromBuilder(lb);

  GameObjectInfo t;
  t.Set(true,
        true,
        ' ',
        Colors::BlackColor,
        Colors::ShadesOfGrey::Six,
        Strings::TileNames::RocksText);

  CreateBorders(t);

  RecordEmptyCells();
  PlaceStairs();
  //CreateInitialMonsters();

  CreateShrines(lb);

  int itemsToCreate = GetEstimatedNumberOfItemsToCreate();
  CreateItemsForLevel(itemsToCreate);
}

// =============================================================================

void MapLevelLostCity::CreateCommonObjects(int x, int y, char image)
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
                Colors::ShadesOfGrey::Six,
                Strings::TileNames::RocksText);
      break;

    case '+':
      PlaceDoor(x, y);
      break;

    case 't':
    {
      char img = 'T';

      #ifdef USE_SDL
      img = GlobalConstants::CP437IndexByType[NameCP437::CLUB];
      #endif

      objName = Strings::TileNames::WitheredTreeText;
      t.Set(true,
            true,
            img,
            Colors::DirtColor,
            Colors::BlackColor,
            objName);
      PlaceStaticObject(x, y, t);
    }
    break;

    case 'T':
      PlaceTree(x, y);
      break;

    case '.':
      PlaceGroundTile(x,
                      y,
                      image,
                      Colors::ShadesOfGrey::Four,
                      Colors::BlackColor,
                      Strings::TileNames::GroundText);
      break;

    case ' ':
      PlaceGroundTile(x,
                      y,
                      image,
                      Colors::BlackColor,
                      Colors::ShadesOfGrey::Ten,
                      Strings::TileNames::FlagstoneText);
      break;

    //
    // TODO: can step in lava and get killed / damaged?
    // Or maybe with some item equipped walk in lava?
    // Right now it's an instakill.
    //

    case 'l':
      PlaceLavaTile(x, y);
      break;

    case 'w':
      PlaceShallowWaterTile(x, y);
      break;

    case 'W':
      PlaceDeepWaterTile(x, y);
      break;

    case 'd':
      PlaceGroundTile(x,
                      y,
                      '.',
                      Colors::DirtDotColor,
                      Colors::DirtColor,
                      Strings::TileNames::DirtText);
      break;

    case 'F':
      t.Set(true,
            false,
            'T',
            Colors::WhiteColor,
            Colors::DeepWaterColor,
            Strings::TileNames::FountainText);
      PlaceStaticObject(x, y, t);
      break;

    case 'g':
      PlaceGrassTile(x, y);
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

void MapLevelLostCity::CreateShrines(LevelBuilder& lb)
{
  auto allShrines = lb.ShrinesByPosition();
  for (auto& kvp : allShrines)
  {
    ShrineType shrineType = kvp.second;
    std::string description = GlobalConstants::ShrineNameByType.at(shrineType);
    auto shrineColor = Colors::ShrineColorsByType.at(shrineType);

    GameObjectInfo t;
    t.Set(true,
          false,
          '/',
          shrineColor.first,
          shrineColor.second,
          description,
          "?Shrine?");
    PlaceStaticObject(kvp.first.X, kvp.first.Y, t);

    auto go = GameObjectsFactory::Instance().CreateShrine(kvp.first.X, kvp.first.Y, shrineType, 1000);
    PlaceGameObject(go);
  }
}

// =============================================================================

void MapLevelLostCity::DisplayWelcomeText()
{
  std::vector<std::string> msg =
  {
  // =============================================== <- 47
    "You certainly heard rumors about a settlement  ",
    "that was buried somewhere deep underground upon",
    "which current City is built. Some even say that",
    "back in the past it was all one giant residence",
    "but then for some reason parts of it were      ",
    "abandoned and over time sunk into the ground.  ",
    "Since nobody knows whether it had a name or    ",
    "whether it even existed, people refer to it    ",
    "as just 'Lost City'.                           ",
    "Nobody took its existence seriously,           ",
    "but here you are, standing in its halls.       "
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "Lost City", msg);
}
