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

void MapLevelBase::PlaceStairs()
{
  int startIndex = RNG::Instance().RandomRange(0, _emptyCells.size());

  LevelStart.X = _emptyCells[startIndex].X;
  LevelStart.Y = _emptyCells[startIndex].Y;

  MapType stairsDownTo = (MapType)(DungeonLevel + 1);
  MapType stairsUpTo = (MapType)(DungeonLevel - 1);

  GameObjectsFactory::Instance().CreateStairs(this, LevelStart.X, LevelStart.Y, '<', stairsUpTo);

  std::vector<Position> possibleExits;

  int mapSizeMax = std::max(MapSize.X, MapSize.Y);

  for (int i = 0; i < _emptyCells.size(); i++)
  {
    auto& c = _emptyCells[i];

    float d = Util::LinearDistance(LevelStart.X, LevelStart.Y, c.X, c.Y);
    if (i != startIndex && d > mapSizeMax / 2)
    {
      possibleExits.push_back(c);
    }
  }

  int index = RNG::Instance().RandomRange(0, possibleExits.size());
  auto exit = possibleExits[index];

  LevelExit.Set(exit.X, exit.Y);

  GameObjectsFactory::Instance().CreateStairs(this, LevelExit.X, LevelExit.Y, '>', stairsDownTo);
}

void MapLevelBase::CreateInitialMonsters()
{
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

void MapLevelBase::TryToSpawnMonsters()
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

void MapLevelBase::DisplayWelcomeText()
{
  std::vector<std::string> msg =
  {
    "You're not supposed to see this text.",
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "MapLevelBase", msg);
}
