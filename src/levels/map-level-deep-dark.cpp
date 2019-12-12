#include "map-level-deep-dark.h"

#include "application.h"

MapLevelDeepDark::MapLevelDeepDark(int sizeX, int sizeY, MapType type, int dungeonLevel)
  : MapLevelBase(sizeX, sizeY, type, dungeonLevel)
{
  switch (MapType_)
  {
    case MapType::CAVES_5:
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
    break;
  }
}

void MapLevelDeepDark::PrepareMap(MapLevelBase* levelOwner)
{
  MapLevelBase::PrepareMap(levelOwner);

  CreateLevel();
}

void MapLevelDeepDark::DisplayWelcomeText()
{
  std::vector<std::string> msg =
  {
    "Deep Dark level description goes here"
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "Deep Dark", msg);
}

void MapLevelDeepDark::CreateLevel()
{
  VisibilityRadius = 3;
  MonstersRespawnTurns = 1000;

  int tunnelLengthMax = MapSize.X / 10;
  int tunnelLengthMin = tunnelLengthMax / 2;

  LevelBuilder lb;

  switch (MapType_)
  {
    case MapType::DEEP_DARK_1:
    {
      int iterations = (MapSize.X * MapSize.Y); // / 10;

      FeatureRoomsWeights weights =
      {
        { FeatureRoomType::EMPTY,    { 10, 0 }  }
      };

      lb.FeatureRoomsMethod(MapSize, { 1, 10 }, weights, 3, iterations);
    }
    break;

    case MapType::DEEP_DARK_2:
    case MapType::DEEP_DARK_3:
    case MapType::DEEP_DARK_4:
    {
      Position start = { 1, 1 };
      lb.BacktrackingTunnelerMethod(MapSize, { 5, 10 }, start, true);
    }
    break;

    case MapType::DEEP_DARK_5:
    {
      CreateSpecialLevel();
    }
    break;
  }

  if (MapType_ != MapType::CAVES_5)
  {
    ConstructFromBuilder(lb);

    RecordEmptyCells();
    PlaceStairs();
    CreateInitialMonsters();
    CreateItemsForLevel(DungeonLevel + 6);
  }
}

void MapLevelDeepDark::CreateSpecialLevel()
{
}
