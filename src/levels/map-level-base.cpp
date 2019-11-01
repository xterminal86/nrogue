#include "map-level-base.h"
#include "application.h"
#include "game-objects-factory.h"
#include "game-object-info.h"

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
    std::vector<std::unique_ptr<GameObject>> rowStatic;
    for (int y = 0; y < MapSize.Y; y++)
    {
      row.push_back(std::unique_ptr<GameObject>(new GameObject()));      
      rowStatic.push_back(nullptr);
    }

    MapArray.push_back(std::move(row));
    StaticMapObjects.push_back(std::move(rowStatic));
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
  if (actor == nullptr)
  {
    #ifdef DEBUG_BUILD
    Printer::Instance().AddMessage("[WARNING] tried to insert null actor!");
    #endif

    return;
  }

  ActorGameObjects.push_back(std::unique_ptr<GameObject>(actor));
}

void MapLevelBase::InsertGameObject(GameObject* goToInsert)
{
  if (goToInsert == nullptr)
  {
    #ifdef DEBUG_BUILD
    Printer::Instance().AddMessage("[WARNING] tried to insert null object!");
    #endif

    return;
  }

  GameObjects.push_back(std::unique_ptr<GameObject>(goToInsert));
}

void MapLevelBase::InsertStaticObject(int x, int y, const GameObjectInfo& objectInfo, int hitPoints, GameObjectType type)
{
  GameObject* go = GameObjectsFactory::Instance().CreateStaticObject(x, y, objectInfo, hitPoints, type);
  InsertStaticObject(go);
}

void MapLevelBase::InsertStaticObject(GameObject* goToInsert)
{
  if (goToInsert == nullptr)
  {
    #ifdef DEBUG_BUILD
    Printer::Instance().AddMessage("[WARNING] tried to insert null static object!");
    #endif

    return;
  }

  int x = goToInsert->PosX;
  int y = goToInsert->PosY;

  StaticMapObjects[x][y].reset(goToInsert);
}

void MapLevelBase::RecordEmptyCells()
{
  for (int x = 0; x < MapSize.X; x++)
  {
    for (int y = 0; y < MapSize.Y; y++)
    {      
      if (!IsCellBlocking({ x, y }))
      {
        Position pos(x, y);
        _emptyCells.push_back(pos);
      }
    }
  }
}

void MapLevelBase::CreateBorders(GameObjectInfo& t)
{
  auto bounds = Util::GetPerimeter(0, 0, MapSize.X - 1, MapSize.Y - 1, true);
  for (auto& i : bounds)
  {
    InsertStaticObject(i.X, i.Y, t);
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

    // NOTE: Not all objects may be added to the factory yet,
    // so check against nullptr is needed.
    auto go = GameObjectsFactory::Instance().CreateRandomItem(x, y);
    if (go != nullptr)
    {
      InsertGameObject(go);
      itemsCreated++;
    }
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

  _emptyCells.erase(_emptyCells.begin() + startIndex);

  int endIndex = RNG::Instance().RandomRange(0, _emptyCells.size());

  LevelExit.X = _emptyCells[endIndex].X;
  LevelExit.Y = _emptyCells[endIndex].Y;

  GameObjectsFactory::Instance().CreateStairs(this, LevelExit.X, LevelExit.Y, '>', stairsDownTo);
}

void MapLevelBase::CreateInitialMonsters()
{
  // FIXME: debug
  MaxMonsters = 1;

  //MaxMonsters = std::sqrt(_emptyCells.size()) / 2;

  for (int i = 0; i < MaxMonsters; i++)
  {
    int index = RNG::Instance().RandomRange(0, _emptyCells.size());

    int x = _emptyCells[index].X;
    int y = _emptyCells[index].Y;

    if (IsSpotValidForSpawn({ x, y }) && !_monstersSpawnRateForThisLevel.empty())
    {
      auto res = Util::WeightedRandom(_monstersSpawnRateForThisLevel);

      // FIXME: debug
      //res = { GameObjectType::SPIDER, 1 };

      auto monster = GameObjectsFactory::Instance().CreateMonster(x, y, res.first);
      InsertActor(monster);
    }
  }
}

bool MapLevelBase::IsSpotValidForSpawn(const Position& pos)
{  
  return !IsCellBlocking(pos);
}

void MapLevelBase::TryToSpawnMonsters()
{
  if (_respawnCounter < MonstersRespawnTurns)
  {
    _respawnCounter++;
    return;
  }

  _respawnCounter = 0;

  if (_monstersSpawnRateForThisLevel.empty())
  {
    return;
  }

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

    // Spawn monsters on cells invisible to the player
    if (!MapArray[cx][cy]->Visible
     && !MapArray[cx][cy]->Occupied
     && !MapArray[cx][cy]->Blocking)
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

bool MapLevelBase::IsCellBlocking(const Position& pos)
{
  bool groundBlock = MapArray[pos.X][pos.Y]->Blocking;
  if (StaticMapObjects[pos.X][pos.Y] != nullptr)
  {
    bool staticObjectsBlock = StaticMapObjects[pos.X][pos.Y]->Blocking;
    return staticObjectsBlock;
  }

  return groundBlock;
}

/// Places grass tile at [x; y], maxDiceRoll serves as a
/// "frequency" modifier - the more its value, the less is the chance
/// for flowers to appear.
void MapLevelBase::PlaceGrassTile(int x, int y, int maxDiceRoll)
{
  char img = '.';

  // Create 'flowers'
  //int tileChoice = RNG::Instance().RandomRange(0, 10);
  //if (tileChoice < 2) img = '.';

  //std::string flowerColor = GlobalConstants::BlackColor;
  std::string flowerColor = GlobalConstants::GrassDotColor;

  int colorChoice = RNG::Instance().RandomRange(0, maxDiceRoll);
  if      (colorChoice == 0) flowerColor = GlobalConstants::WhiteColor;
  else if (colorChoice == 1) flowerColor = GlobalConstants::DandelionYellowColor;
  else if (colorChoice == 2) flowerColor = GlobalConstants::RedPoppyColor;

  std::map<int, std::string> flowersNameByChoice =
  {
    { 0, "Chamomile" },
    { 1, "Dandelion" },
    { 2, "Poppy"     }
  };

  std::string tileName = "Grass";
  if (flowersNameByChoice.count(colorChoice) == 1)
  {
    tileName = flowersNameByChoice[colorChoice];
  }

  GameObjectInfo t;
  t.Set(false, false, img, flowerColor, GlobalConstants::GrassColor, tileName);

  MapArray[x][y]->MakeTile(t);
}

void MapLevelBase::CreateLevel()
{
  auto str = Util::StringFormat("%s, %s - no level was created!", __PRETTY_FUNCTION__, LevelName.data());
  Logger::Instance().Print(str, true);
}

void MapLevelBase::ConstructFromBuilder(LevelBuilder& lb)
{
  auto str = Util::StringFormat("%s, %s - calling base ConstructFromBuilder()!", __PRETTY_FUNCTION__, LevelName.data());
  Logger::Instance().Print(str, true);
}

void MapLevelBase::CreateSpecialLevel()
{
  // For overriding procedural design generation
}
