#include "map-level-nether.h"

#include "application.h"
#include "game-object-info.h"
#include "game-objects-factory.h"
#include "door-component.h"
#include "logger.h"

MapLevelNether::MapLevelNether(int sizeX, int sizeY, MapType type, int dungeonLevel)
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

void MapLevelNether::PrepareMap(MapLevelBase* levelOwner)
{
  MysteriousForcePresent = true;

  MapLevelBase::PrepareMap(levelOwner);

  CreateLevel();
}

void MapLevelNether::CreateLevel()
{
  VisibilityRadius = 20;
  MonstersRespawnTurns = GlobalConstants::MonstersRespawnTimeout;

  GameObjectInfo t;
  t.Set(true, true, ' ', Colors::BlackColor, Colors::CaveWallColor, "Cave Wall");

  CreateBorders(t);

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

  if (MapType_ != MapType::NETHER_5)
  {
    if (Util::Rolld100(_shrineRollChance))
    {
      PlaceRandomShrine(lb);
    }

    ConstructFromBuilder(lb);

    RecordEmptyCells();
    PlaceStairs();

    CreateInitialMonsters();
  }
}

void MapLevelNether::ConstructFromBuilder(LevelBuilder& lb)
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
          PlaceWall(x, y, ' ', Colors::BlackColor, Colors::RedPoppyColor, "Hellrock");
          break;

        case '+':
        {
          GameObject* door = GameObjectsFactory::Instance().CreateDoor(x, y, false, DoorMaterials::STONE);

          if (Util::Rolld100(15))
          {
            DoorComponent* dc = door->GetComponent<DoorComponent>();
            dc->OpenedBy = GlobalConstants::OpenedByNobody;
          }

          PlaceStaticObject(door);
        }
        break;

        case '.':
          PlaceGroundTile(x, y, image, "#440000", Colors::BlackColor, "Hellstone");
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
          PlaceShrine({ x, y, }, lb);
          break;
      }
    }
  }
}

void MapLevelNether::DisplayWelcomeText()
{
  std::vector<std::string> msg =
  {
  // ======================================== <- 40
    "You don't believe your eyes.             ",
    "The very image of Hell that lives inside ",
    "the head of any commoner is materialized ",
    "before you. Pools of lava, fire and heat,",
    "creatures of various shapes and sizes    ",
    "that nobody ever imagined...             ",
    "What is this place?"
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "Nether", msg);
}
