#include "map-level-mines.h"

#include "application.h"
#include "rng.h"
#include "constants.h"
#include "game-objects-factory.h"
#include "player.h"

MapLevelMines::MapLevelMines(int sizeX, int sizeY, MapType type, int dungeonLevel) :
  MapLevelBase(sizeX, sizeY, type, dungeonLevel)
{
  _layoutsForLevel =
  {
    // 0
    {
      "#########",
      "#########",
      ".........",
      ".........",
      ".........",
      ".........",
      ".........",
      "#########",
      "#########",
    },
    // 1
    {
      "#########",
      "#########",
      ".......##",
      ".......##",
      ".......##",
      ".......##",
      ".......##",
      "##.....##",
      "##.....##",
    },
    // 2
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
      "#.##.##.#",
      "#.#...#.#",
      "#.#.#.#.#",
      "#.#.#.#.#",
      "#.#.#.#.#",
      "#.#.#.#.#",
      "#.#.#.#.#",
      "#...#...#",
      "##.###.##",
    }
  };

  switch (DungeonLevel)
  {
    case 1:
    {
      _roomsForLevel =
      {
        { 80, _layoutsForLevel[0] },
        { 80, _layoutsForLevel[1] },
        { 50, _layoutsForLevel[2] },
      };
    }
    break;

    case 2:
    {
      _roomsForLevel =
      {
        { 60, _layoutsForLevel[2] },
        { 70, _layoutsForLevel[3] },
        { 50, _layoutsForLevel[4] },
      };
    }
    break;

    case 3:
    {
      _roomsForLevel =
      {
        { 80, _layoutsForLevel[4] },
        { 80, _layoutsForLevel[5] },
        { 80, _layoutsForLevel[6] },
        { 50, _layoutsForLevel[7] }
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
  VisibilityRadius = 10;
  MaxMonsters = 15 * DungeonLevel;
  MonstersRespawnTurns = 1000;

  Tile t;
  t.Set(false, false, '.', GlobalConstants::GroundColor, GlobalConstants::BlackColor, "Dirt");

  FillArea(0, 0, MapSize.X - 1, MapSize.Y - 1, t);

  LevelBuilder lb;

  lb.BuildLevel(_roomsForLevel, 0, 0, MapSize.X, MapSize.Y);

  ConstructFromBuilder(lb);

  // Borders

  t.Set(true, true, ' ', GlobalConstants::BlackColor, GlobalConstants::MountainsColor, "Rocks");

  auto bounds = Util::GetPerimeter(0, 0, MapSize.X - 1, MapSize.Y - 1, true);
  for (auto& i : bounds)
  {
    MapArray[i.X][i.Y].get()->MakeTile(t);
  }

  RecordEmptyCells();

  PlaceExits();

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
  Logger::Instance().Print("********** PROCESSING CHUNKS **********");

  for (auto& chunk : lb.MapChunks)
  {
    int sx = chunk.UpperLeftCorner.X;
    int sy = chunk.UpperLeftCorner.Y;
    int size = chunk.Layout.size();

    //auto str = Util::StringFormat("Processing chunk [%i;%i] size %i...", sx, sy, size);
    //Logger::Instance().Print(str);
    //Util::PrintLayout(chunk.Layout);

    int mapX = sx;
    int mapY = sy;

    for (int x = 0; x < size; x++)
    {
      mapY = sy;

      for (int y = 0; y < size; y++)
      {
        Tile t;
        std::string objName;

        char image = chunk.Layout[x][y];
        switch (image)
        {
          case '#':
          {
            objName = "Rocks";
            t.Set(true, true, ' ', GlobalConstants::BlackColor, GlobalConstants::MountainsColor, "Rocks");
          }
          break;

          case '.':
          {
            objName = "Ground";
            t.Set(false, false, image, GlobalConstants::GroundColor, GlobalConstants::BlackColor, objName);
          }
          break;
        }

        MapArray[mapX][mapY]->MakeTile(t);

        mapY++;
      }

      mapX++;
    }
  }
}

void MapLevelMines::CreateInitialMonsters()
{
  // Some rats

  for (int i = 0; i < MaxMonsters; i++)
  {
    int index = RNG::Instance().RandomRange(0, _emptyCells.size());

    int x = _emptyCells[index].X;
    int y = _emptyCells[index].Y;

    if (!MapArray[x][y]->Blocking && !MapArray[x][y]->Occupied)
    {
      // Special rats
      //auto rat = GameObjectsFactory::Instance().CreateRat(x, y, false);

      // Normal rats
      auto rat = GameObjectsFactory::Instance().CreateRat(x, y);

      InsertActor(rat);
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
      return;
    }

    int index = RNG::Instance().RandomRange(0, _emptyCells.size());
    int cx = _emptyCells[index].X;
    int cy = _emptyCells[index].Y;

    if (!MapArray[cx][cy]->Visible && !MapArray[cx][cy]->Occupied)
    {
      auto rat = GameObjectsFactory::Instance().CreateRat(cx, cy);
      InsertActor(rat);
    }
  }
}

void MapLevelMines::PlaceExits()
{
  LevelStart.X = _emptyCells[0].X;
  LevelStart.Y = _emptyCells[0].Y;

  GameObjectsFactory::Instance().CreateStairs(this, LevelStart.X, LevelStart.Y, '<', MapType::TOWN);

  std::vector<Position> possibleExits;

  int mapSizeMax = std::max(MapSize.X, MapSize.Y);

  for (auto& c : _emptyCells)
  {
    int d = Util::LinearDistance(_playerRef->PosX, _playerRef->PosY, c.X, c.Y);
    if (d > mapSizeMax / 2)
    {
      possibleExits.push_back(c);
    }
  }

  int index = RNG::Instance().RandomRange(0, possibleExits.size());
  auto exit = possibleExits[index];

  LevelExit.Set(exit.X, exit.Y);

  int cl = (int)MapType_;
  GameObjectsFactory::Instance().CreateStairs(this, LevelExit.X, LevelExit.Y, '>', (MapType)(cl + 1));
}
