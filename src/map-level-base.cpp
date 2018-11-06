#include "map-level-base.h"
#include "application.h"

MapLevelBase::MapLevelBase(int sizeX, int sizeY, MapType type, int dungeonLevel)
{
  MapSize.X = sizeX;
  MapSize.Y = sizeY;
  MapType_ = type;
  DungeonLevel = dungeonLevel;

  auto levelNames = GlobalConstants::MapLevelRandomNames.at(MapType_);
  int index = RNG::Instance().RandomRange(0, levelNames.size());
  LevelName = levelNames[index];

  _playerRef = &Application::Instance().PlayerInstance;
}

void MapLevelBase::PrepareMap(MapLevelBase* levelOwner)
{
  for (int x = 0; x < MapSize.X; x++)
  {
    std::vector<std::unique_ptr<GameObject>> row;
    for (int y = 0; y < MapSize.Y; y++)
    {
      row.push_back(std::unique_ptr<GameObject>(new GameObject()));
    }

    MapArray.push_back(std::move(row));
  }

  for (int x = 0; x < MapSize.X; x++)
  {
    for (int y = 0; y < MapSize.Y; y++)
    {
      MapArray[x][y]->Init(levelOwner, x, y, ' ', "#000000", "#000000");
    }
  }
}

void MapLevelBase::SetPlayerStartingPosition(int x, int y)
{
  PlayerStart.X = x;
  PlayerStart.Y = y;

  int tw = Printer::Instance().TerminalWidth;
  int th = Printer::Instance().TerminalHeight;

  MapOffsetX = tw / 2 - PlayerStart.X;
  MapOffsetY = th / 2 - PlayerStart.Y;
}

void MapLevelBase::InsertActor(GameObject* actor)
{
  ActorGameObjects.push_back(std::unique_ptr<GameObject>(actor));
}

void MapLevelBase::InsertGameObject(GameObject* goToInsert)
{
  GameObjects.push_back(std::unique_ptr<GameObject>(goToInsert));
}
