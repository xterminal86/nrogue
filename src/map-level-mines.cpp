#include "map-level-mines.h"

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
      "#########",
      ".........",
      ".........",
      ".........",
      ".........",
      ".........",
      "#########",
      "#########",
    },
    // 2
    {
      "###...###",
      "###...###",
      "###...###",
      ".........",
      ".........",
      ".........",
      "###...###",
      "###...###",
      "###...###",
    },
    // 3
    {
      ".........",
      ".##...##.",
      ".##...##.",
      ".........",
      ".........",
      ".........",
      ".##...##.",
      ".##...##.",
      ".........",
    }
  };

  _roomsForLevel =
  {
    { 60, _layoutsForLevel[0] },
    { 90, _layoutsForLevel[1] },
    { 50, _layoutsForLevel[2] },
    { 40, _layoutsForLevel[3] }
  };
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

  Tile t;
  t.Set(false, false, '.', GlobalConstants::GroundColor, GlobalConstants::BlackColor, "Dirt");

  FillArea(0, 0, MapSize.X - 1, MapSize.Y - 1, t);

  LevelBuilder lb;

  lb.BuildLevel(_roomsForLevel, 0, 0, MapSize.X, MapSize.Y);
  //lb.BuildLevel(_roomsForLevel, MapSize.X / 2, MapSize.Y / 2, MapSize.X, MapSize.Y);

  ConstructFromBuilder(lb);

  // Borders

  t.Set(true, true, '#', GlobalConstants::WallColor, GlobalConstants::BlackColor, "Rocks");

  auto bounds = Util::GetPerimeter(0, 0, MapSize.X - 1, MapSize.Y - 1, true);
  for (auto& i : bounds)
  {
    MapArray[i.X][i.Y].get()->MakeTile(t);
  }

  RecordEmptyCells();

  LevelStart.X = _emptyCells[0].X;
  LevelStart.Y = _emptyCells[0].Y;

  GameObjectsFactory::Instance().CreateStairs(this, LevelStart.X, LevelStart.Y, '<', MapType::TOWN);

  // CreateInitialMonsters();
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
            t.Set(true, true, image, GlobalConstants::WallColor, GlobalConstants::BlackColor, objName);
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
