#include "map-level-base.h"
#include "application.h"
#include "game-objects-factory.h"

MapLevelBase::MapLevelBase(int sizeX, int sizeY, MapType type, int dungeonLevel)
{  
  MapSize.X = sizeX;
  MapSize.Y = sizeY;
  MapType_ = type;
  DungeonLevel = dungeonLevel;

  auto levelNames = GlobalConstants::MapLevelNames.at(MapType_);
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
      MapArray[x][y]->Init(levelOwner, x, y, '.', "#000000", "#000000");
    }
  }

  IsInitialized = true;
}

void MapLevelBase::AdjustCamera()
{
  int tw = Printer::Instance().TerminalWidth;
  int th = Printer::Instance().TerminalHeight;

  MapOffsetX = tw / 2 - _playerRef->PosX;
  MapOffsetY = th / 2 - _playerRef->PosY;
}

void MapLevelBase::InsertActor(GameObject* actor)
{
  ActorGameObjects.push_back(std::unique_ptr<GameObject>(actor));
}

void MapLevelBase::InsertGameObject(GameObject* goToInsert)
{
  GameObjects.push_back(std::unique_ptr<GameObject>(goToInsert));
}

void MapLevelBase::RecordEmptyCells()
{
  for (int x = 0; x < MapSize.X; x++)
  {
    for (int y = 0; y < MapSize.Y; y++)
    {
      if (!MapArray[x][y]->Blocking)
      {
        Position pos(x, y);
        _emptyCells.push_back(pos);
      }
    }
  }
}

void MapLevelBase::CreateBorders(Tile& t)
{
  auto bounds = Util::GetPerimeter(0, 0, MapSize.X - 1, MapSize.Y - 1, true);
  for (auto& i : bounds)
  {
    MapArray[i.X][i.Y].get()->MakeTile(t);
  }
}

void MapLevelBase::CreateItemsForLevel(int maxItems)
{
  int itemsCreated = 0;

  while (itemsCreated < maxItems)
  {
    int index = RNG::Instance().RandomRange(0, _emptyCells.size());

    int x = _emptyCells[index].X;
    int y = _emptyCells[index].Y;

    auto go = GameObjectsFactory::Instance().CreateRandomItem(x, y);
    InsertGameObject(go);
    itemsCreated++;
  }
}

void MapLevelBase::DisplayWelcomeText()
{
  std::vector<std::string> msg =
  {
    "You're not supposed to see this text.",
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "MapLevelBase", msg);
}
