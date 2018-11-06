#include "map-level-mines.h"

#include "rng.h"
#include "constants.h"
#include "game-objects-factory.h"

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
  for (int x = 0; x < MapSize.X; x += 5)
  {
    for (int y = 0; y < MapSize.Y; y += 5)
    {
      int roomIndex = RNG::Instance().RandomRange(9, 20);
      CreateRoom(x, y, GlobalConstants::RoomLayouts[roomIndex]);
    }
  }

  for (int x = 0; x < MapSize.X; x += 5)
  {
    for (int y = 0; y < MapSize.Y; y += 5)
    {
      if (!MapArray[x][y]->Blocking)
      {
        Position pos(x, y);
        _emptyCells.push_back(pos);
      }
    }
  }

  LevelStart = _emptyCells[0];

  GameObjectsFactory::Instance().CreateStairs(this, _emptyCells[0].X, _emptyCells[0].Y, '<', MapType::TOWN);
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
          t.Set(true, true, c, GlobalConstants::WallColor, GlobalConstants::BlackColor, "Stone Wall");
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
