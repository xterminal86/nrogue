#include "map-level-mines.h"

#include "rng.h"
#include "constants.h"
#include "game-objects-factory.h"
#include "player.h"

MapLevelMines::MapLevelMines(int sizeX, int sizeY, MapType type, int dungeonLevel) :
  MapLevelBase(sizeX, sizeY, type, dungeonLevel)
{
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

  t.Set(true, true, '#', GlobalConstants::WallColor, GlobalConstants::BlackColor, "Rocks");

  auto bounds = Util::GetPerimeter(0, 0, MapSize.X - 1, MapSize.Y - 1, true);
  for (auto& i : bounds)
  {
    MapArray[i.X][i.Y].get()->MakeTile(t);
  }

  LevelBuilder lb;

  lb.MaxRooms = 32;

  lb.BuildLevel(MapSize.X, MapSize.Y);

  CopyFromBuilder(lb, MapSize.X / 2, MapSize.Y / 2);

  RecordEmptyCells();

  LevelStart.X = _emptyCells[1].X;
  LevelStart.Y = _emptyCells[1].Y;

  GameObjectsFactory::Instance().CreateStairs(this, LevelStart.X, LevelStart.Y, '<', MapType::TOWN);
}

void MapLevelMines::CreateRoom(int x, int y, const std::vector<std::string>& layout, bool randomizeOrientation)
{
  Tile t;

  int posX = x;
  int posY = y;

  std::vector<std::string> newLayout = layout;

  std::vector<RoomLayoutRotation> rotations =
  {
    RoomLayoutRotation::NONE,
    RoomLayoutRotation::CCW_90,
    RoomLayoutRotation::CCW_180,
    RoomLayoutRotation::CCW_270
  };

  if (randomizeOrientation)
  {
    int index = RNG::Instance().Random() % rotations.size();
    newLayout = Util::RotateRoomLayout(layout, rotations[index]);
  }

  for (auto& row : newLayout)
  {
    for (auto& c : row)
    {
      switch (c)
      {
        case '#':
        {
          t.Set(true, true, c, GlobalConstants::WallColor, GlobalConstants::BlackColor, "Rocks");
          MapArray[posX][posY]->MakeTile(t);
        }
        break;

        case '.':
        {
          t.Set(false, false, '.', GlobalConstants::GroundColor, GlobalConstants::BlackColor, "Dirt");
          MapArray[posX][posY]->MakeTile(t);
        }
        break;
      }

      posX++;
    }

    posX = x;
    posY++;
  }
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

void MapLevelMines::CopyFromBuilder(LevelBuilder& lb, int startX, int startY)
{
  for (auto& chunk : lb.MapChunks)
  {
    for (int x = 0; x < chunk.Layout.size(); x++)
    {
      for (int y = 0; y < chunk.Layout.size(); y++)
      {
        char image = chunk.Layout[x][y];
        std::string objName = (image == '#') ? "Rocks" : "Dirt";

        Tile t;
        t.Set((image == '#'), (image == '#'), image, GlobalConstants::WallColor, GlobalConstants::BlackColor, objName);

        int mapX = startX + chunk.UpperLeftCorner.X + x;
        int mapY = startY + chunk.UpperLeftCorner.Y + y;

        MapArray[mapX][mapY]->MakeTile(t);
      }
    }
  }
}
