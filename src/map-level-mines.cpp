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
      ".........",
      "...###...",
      "...###...",
      "...###...",
      ".........",
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
    { 90, _layoutsForLevel[0] },
    { 80, _layoutsForLevel[1] },
    { 70, _layoutsForLevel[2] },
    { 60, _layoutsForLevel[3] }
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

  Tile t;
  t.Set(false, false, '.', GlobalConstants::WallColor, GlobalConstants::BlackColor, "Dirt");

  FillArea(0, 0, MapSize.X - 1, MapSize.Y - 1, t);

  /*
  for (int x = 0; x < MapSize.X; x += 5)
  {
    for (int y = 0; y < MapSize.Y; y += 5)
    {
      int roomIndex = RNG::Instance().RandomRange(9, 20);
      CreateRoom(x, y, GlobalConstants::RoomLayouts[roomIndex]);
    }
  }
  */

  LevelBuilder lb;

  lb.BuildLevel(_roomsForLevel, 0, 0, MapSize.X, MapSize.Y);

  ConstructFromBuilder(lb);

  // Borders

  t.Set(true, true, '#', GlobalConstants::WallColor, GlobalConstants::BlackColor, "Rocks");

  auto bounds = Util::GetPerimeter(0, 0, MapSize.X - 1, MapSize.Y - 1, true);
  for (auto& i : bounds)
  {
    MapArray[i.X][i.Y].get()->MakeTile(t);
  }

  RecordEmptyCells();

  LevelStart.X = _emptyCells[1].X;
  LevelStart.Y = _emptyCells[1].Y;

  GameObjectsFactory::Instance().CreateStairs(this, LevelStart.X, LevelStart.Y, '<', MapType::TOWN);
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
        char image = chunk.Layout[x][y];
        std::string objName = (image == '#') ? "Rocks" : "Dirt";

        Tile t;
        t.Set((image == '#'), (image == '#'), image, GlobalConstants::WallColor, GlobalConstants::BlackColor, objName);

        MapArray[mapX][mapY]->MakeTile(t);

        mapY++;
      }

      mapX++;
    }
  }
}
