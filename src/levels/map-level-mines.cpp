#include "map-level-mines.h"

#include "application.h"
#include "rng.h"
#include "constants.h"
#include "game-objects-factory.h"
#include "door-component.h"
#include "player.h"

MapLevelMines::MapLevelMines(int sizeX, int sizeY, MapType type, int dungeonLevel) :
  MapLevelBase(sizeX, sizeY, type, dungeonLevel)
{
  _layoutsForLevel =
  {
    // 0
    {
      ".........",
      ".........",
      "..#####..",
      "..#####..",
      "..#####..",
      "..#####..",
      "..#####..",
      ".........",
      ".........",
    },
    // 1
    {
      "#########",
      "##.....##",
      ".+.....+.",
      "##.....##",
      "##.....##",
      "##.....##",
      "##.....##",
      "####+####",
      "####.####",
    },
    // 2
    {
      ".........",
      ".#######.",
      ".#######.",
      ".#######.",
      ".#######.",
      ".#######.",
      ".#######.",
      ".#######.",
      ".........",
    },
    // 3
    {
      "#########",
      ".........",
      "#########",
      "#########",
      ".........",
      "#########",
      "#########",
      ".........",
      "#########",
    },
    // 4
    {
      "#.#####.#",
      ".........",
      "#.##.##.#",
      "#.##.##.#",
      ".........",
      "#.##.##.#",
      "#.##.##.#",
      ".........",
      "#.##.##.#",
    },
    // 5
    {
      "#########",
      "...##....",
      "#.###.###",
      "#.###.###",
      "#.......#",
      "####.##.#",
      "#..#.#..#",
      "...###...",
      "#.##.##.#",
    },
    // 6
    {
      "#########",
      "#..##....",
      "..###.#.#",
      "#.#.#.#.#",
      "#.......#",
      "####.##..",
      "#..#.####",
      "...#.....",
      "#.##.##.#",
    },
    // 7
    {
      "#.#####.#",
      "#.#...#.#",
      "#.#.#.#.#",
      "#.#.#.#.#",
      "#.#.#.#.#",
      "#.#.#.#.#",
      "#.#.#.#.#",
      "#...#...#",
      "#########",
    },
    // 8
    {
      "####+####",
      "+.#...#.+",
      "#.#.#.#.#",
      "#.#.#.#.#",
      "#.#.#.#.#",
      "#.#...#.#",
      "#.##.##.#",
      "#.......#",
      "####+####",
    },
    // 9
    {
      "##.....##",
      "##.....##",
      "##.....##",
      "##.....##",
      "##.....##",
      "##.....##",
      "##.....##",
      "##.....##",
      "##.....##",
    }
  };

  switch (DungeonLevel)
  {
    case 1:
    {
      _roomsForLevel =
      {
        { 0, _layoutsForLevel[0] },
        { 30, _layoutsForLevel[9] },
        { 60, _layoutsForLevel[1] },
        { 50, _layoutsForLevel[2] },
        { 80, _layoutsForLevel[3] },
      };

      _monstersSpawnRateForThisLevel =
      {
        { MonsterType::RAT, 1 }
      };
    }
    break;

    case 2:
    {
      _roomsForLevel =
      {
        { 0, _layoutsForLevel[0] },
        { 10, _layoutsForLevel[9] },
        { 60, _layoutsForLevel[2] },
        { 70, _layoutsForLevel[3] },
        { 50, _layoutsForLevel[4] },
      };

      _monstersSpawnRateForThisLevel =
      {
        { MonsterType::RAT, 8 },
        { MonsterType::BAT, 1 }
      };
    }
    break;

    case 3:
    {
      _roomsForLevel =
      {
        { 0, _layoutsForLevel[2] },
        { 80, _layoutsForLevel[5] },
        { 80, _layoutsForLevel[6] },
        { 50, _layoutsForLevel[7] },
        { 50, _layoutsForLevel[8] }
      };

      _monstersSpawnRateForThisLevel =
      {
        { MonsterType::RAT, 8 },
        { MonsterType::SPIDER, 1 },
        { MonsterType::BAT, 4 }
      };
    }
    break;
  }
}

void MapLevelMines::PrepareMap(MapLevelBase* levelOwner)
{
  MapLevelBase::PrepareMap(levelOwner);

  CreateLevel();  
}

void MapLevelMines::CreateLevel()
{
  VisibilityRadius = 5;
  MonstersRespawnTurns = 1000;

  Tile t;
  t.Set(false, false, '.', GlobalConstants::GroundColor, GlobalConstants::BlackColor, "Dirt");

  FillArea(0, 0, MapSize.X - 1, MapSize.Y - 1, t);

  // Build level

  LevelBuilder lb;

  lb.BuildLevelFromLayouts(_roomsForLevel, 1, 1, MapSize.X, MapSize.Y);

  ConstructFromBuilder(lb);

  // Borders

  t.Set(true, true, ' ', GlobalConstants::BlackColor, GlobalConstants::MountainsColor, "Rocks");
  CreateBorders(t);

  RecordEmptyCells();

  PlaceStairs();

  CreateInitialMonsters();
}

void MapLevelMines::FillArea(int ax, int ay, int aw, int ah, const Tile& tileToFill)
{
  for (int x = ax; x <= ax + aw; x++)
  {
    for (int y = ay; y <= ay + ah; y++)
    {
      MapArray[x][y]->MakeTile(tileToFill);
    }
  }
}

void MapLevelMines::ConstructFromBuilder(LevelBuilder& lb)
{
  Logger::Instance().Print("********** INSTANTIATING LAYOUT **********");

  for (int x = 0; x < MapSize.X; x++)
  {
    for (int y = 0; y < MapSize.Y; y++)
    {
      Tile t;
      std::string objName;

      char image = lb.MapLayout[x][y];
      switch (image)
      {
        case '#':
        {
          objName = "Rocks";
          t.Set(true, true, ' ', GlobalConstants::BlackColor, GlobalConstants::MountainsColor, "Rocks");
          MapArray[x][y]->MakeTile(t);
        }
        break;

        case '.':
        {
          objName = "Ground";
          t.Set(false, false, image, GlobalConstants::GroundColor, GlobalConstants::BlackColor, objName);
          MapArray[x][y]->MakeTile(t);
        }
        break;

        case '+':
        {
          CreateDoor(x, y);
        }
        break;
      }
    }
  }
}

void MapLevelMines::CreateInitialMonsters()
{
  // Some rats

  MaxMonsters = std::sqrt(_emptyCells.size()) / 2;

  for (int i = 0; i < MaxMonsters; i++)
  {
    int index = RNG::Instance().RandomRange(0, _emptyCells.size());

    int x = _emptyCells[index].X;
    int y = _emptyCells[index].Y;

    if (!MapArray[x][y]->Blocking && !MapArray[x][y]->Occupied)
    {
      auto res = Util::WeightedRandom(_monstersSpawnRateForThisLevel);
      auto monster = GameObjectsFactory::Instance().CreateMonster(x, y, res.first);
      InsertActor(monster);
    }
  }
}

void MapLevelMines::DisplayWelcomeText()
{
  std::vector<std::string> msg =
  {
    "These mines once were a place of",
    "work and income for this village.",
    "Now it's just crumbling tunnels",
    "with occasional marks here and there",
    "suggesting human presence in the past."
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "Abandoned Mines", msg);
}

void MapLevelMines::TryToSpawnMonsters()
{
  if (_respawnCounter < MonstersRespawnTurns)
  {
    _respawnCounter++;
    return;
  }

  _respawnCounter = 0;

  std::vector<Position> positions;

  int attempts = 20;

  for (int i = 0; i < attempts; i++)
  {
    if (ActorGameObjects.size() >= MaxMonsters)
    {
      break;
    }

    int index = RNG::Instance().RandomRange(0, _emptyCells.size());
    int cx = _emptyCells[index].X;
    int cy = _emptyCells[index].Y;

    if (!MapArray[cx][cy]->Visible && !MapArray[cx][cy]->Occupied)
    {
      auto res = Util::WeightedRandom(_monstersSpawnRateForThisLevel);
      auto monster = GameObjectsFactory::Instance().CreateMonster(cx, cy, res.first);
      InsertActor(monster);
      break;
    }
  }
}

void MapLevelMines::PlaceStairs()
{
  LevelStart.X = _emptyCells[0].X;
  LevelStart.Y = _emptyCells[0].Y;

  MapType stairsDownTo = (MapType)(DungeonLevel + 1);
  MapType stairsUpTo = (MapType)(DungeonLevel - 1);

  GameObjectsFactory::Instance().CreateStairs(this, LevelStart.X, LevelStart.Y, '<', stairsUpTo);

  std::vector<Position> possibleExits;

  int mapSizeMax = std::max(MapSize.X, MapSize.Y);

  for (int i = 1; i < _emptyCells.size(); i++)
  {
    auto& c = _emptyCells[i];

    float d = Util::LinearDistance(LevelStart.X, LevelStart.Y, c.X, c.Y);
    if (d > mapSizeMax / 2)
    {
      possibleExits.push_back(c);
    }
  }

  int index = RNG::Instance().RandomRange(0, possibleExits.size());
  auto exit = possibleExits[index];

  LevelExit.Set(exit.X, exit.Y);

  GameObjectsFactory::Instance().CreateStairs(this, LevelExit.X, LevelExit.Y, '>', stairsDownTo);
}

void MapLevelMines::CreateDoor(int x, int y, bool isOpen)
{
  auto c = MapArray[x][y]->AddComponent<DoorComponent>();
  DoorComponent* dc = static_cast<DoorComponent*>(c);
  dc->IsOpen = isOpen;
  dc->UpdateDoorState();

  // https://stackoverflow.com/questions/15264003/using-stdbind-with-member-function-use-object-pointer-or-not-for-this-argumen/15264126#15264126
  //
  // When using std::bind to bind a member function, the first argument is the object's this pointer.

  MapArray[x][y]->InteractionCallback = std::bind(&DoorComponent::Interact, dc);

  MapArray[x][y]->ObjectName = "Door";
}
