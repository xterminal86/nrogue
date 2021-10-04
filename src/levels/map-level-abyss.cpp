#include "map-level-abyss.h"

#include "application.h"
#include "game-object-info.h"
#include "game-objects-factory.h"
#include "door-component.h"
#include "logger.h"

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

  // Note that x and y are swapped to correspond to
  // "world" dimensions.
  int sx = _specialLevel[0].length();
  int sy = _specialLevel.size();

  MapSize.Set(sx, sy);
}

void MapLevelAbyss::PrepareMap(MapLevelBase* levelOwner)
{
  MapLevelBase::PrepareMap(levelOwner);

  CreateLevel();
}

void MapLevelAbyss::DisplayWelcomeText()
{
  std::vector<std::string> msg =
  {
    "Abyss level description goes here"
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "Abyss", msg);
}

void MapLevelAbyss::CreateLevel()
{
  // Borders
  GameObjectInfo t;

  t.Set(true, true, ' ', Colors::BlackColor, Colors::ShadesOfGrey::Six, "Rocks");
  CreateBorders(t);

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

  if (MapType_ != MapType::ABYSS_5)
  {
    if (Util::Rolld100(_shrineRollChance))
    {
      PlaceRandomShrine(lb);
    }

    ConstructFromBuilder(lb);

    RecordEmptyCells();
    PlaceStairs();
  }
}

void MapLevelAbyss::ConstructFromBuilder(LevelBuilder& lb)
{
  Logger::Instance().Print("********** INSTANTIATING LAYOUT **********");

  for (int x = 0; x < MapSize.X; x++)
  {
    for (int y = 0; y < MapSize.Y; y++)
    {
      GameObjectInfo t;
      std::string objName;

      char image = lb.MapRaw[x][y];
      switch (image)
      {
        case '#':
        {
          objName = "Abyss Rocks";
          t.Set(true, true, ' ', Colors::BlackColor, Colors::RedPoppyColor, objName);
          InsertStaticObject(x, y, t, -1, GameObjectType::PICKAXEABLE);
        }
        break;

        case '+':
        {
          GameObject* door = GameObjectsFactory::Instance().CreateDoor(x, y, false);

          if (Util::Rolld100(15))
          {
            DoorComponent* dc = door->GetComponent<DoorComponent>();
            dc->OpenedBy = GlobalConstants::OpenedByNobody;
          }

          InsertStaticObject(door);
        }
        break;

        case '.':
          PlaceGroundTile(x, y, ' ', Colors::BlackColor, "#440000", "Abyss Floor");
          break;

        case 'g':
          PlaceGrassTile(x, y);
          break;

        case 'w':
          PlaceDeepWaterTile(x, y);
          break;

        case ' ':
          PlaceGroundTile(x, y, '.', Colors::BlackColor, Colors::ShadesOfGrey::Ten, "Stone");
          break;

        case 'l':
          PlaceLavaTile(x, y);
          break;

        case '/':
          PlaceShrine({ x, y }, lb);
          break;
      }
    }
  }
}
