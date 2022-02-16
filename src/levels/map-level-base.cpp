#include "map-level-base.h"
#include "application.h"
#include "game-objects-factory.h"
#include "items-factory.h"
#include "game-object-info.h"
#include "printer.h"
#include "logger.h"
#include "door-component.h"
#include "map.h"

MapLevelBase::MapLevelBase(int sizeX, int sizeY, MapType type, int dungeonLevel)
{
  MapSize.X = sizeX;
  MapSize.Y = sizeY;
  MapType_ = type;
  DungeonLevel = dungeonLevel;

  std::string levelName;

  auto GetSpecialName = [this]()
  {
    auto levelNames = GlobalConstants::MapLevelNames.at(MapType_);
    int index = RNG::Instance().RandomRange(0, levelNames.size());
    return levelNames[index];
  };

  if (MapType_ == MapType::TOWN)
  {
    bool shouldGenerate = Util::Rolld100(90);
    if (shouldGenerate)
    {
      levelName = "Village of " + Util::GenerateName(false, true, GlobalConstants::TownNameEndings);
    }
    else
    {
      levelName = GetSpecialName();
    }
  }
  else
  {
    levelName = GetSpecialName();
  }

  LevelName = levelName;

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
  int tw = Printer::TerminalWidth;
  int th = Printer::TerminalHeight;

  MapOffsetX = tw / 2 - _playerRef->PosX;
  MapOffsetY = th / 2 - _playerRef->PosY;
}

void MapLevelBase::InsertActor(GameObject* actor)
{
  if (actor == nullptr)
  {
    #ifdef DEBUG_BUILD
    std::string str = "[WARNING] tried to insert null actor!";
    Printer::Instance().AddMessage(str);
    Logger::Instance().Print(str);
    DebugLog("%s\n", str.data());
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
    std::string str = "[WARNING] tried to insert null object!";
    Printer::Instance().AddMessage(str);
    Logger::Instance().Print(str);
    DebugLog("%s\n", str.data());
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
    std::string str = "[WARNING] tried to insert null static object!";
    Printer::Instance().AddMessage(str);
    Logger::Instance().Print(str);
    DebugLog("%s\n", str.data());
    #endif

    return;
  }

  int x = goToInsert->PosX;
  int y = goToInsert->PosY;

  StaticMapObjects[x][y].reset(goToInsert);
}

void MapLevelBase::RecordEmptyCells()
{
  _emptyCells.clear();

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
    itemsCreated++;

    int index = RNG::Instance().RandomRange(0, _emptyCells.size());

    int x = _emptyCells[index].X;
    int y = _emptyCells[index].Y;

    if (!IsSpotValidForSpawn({ x, y }))
    {
      continue;
    }

    // NOTE: Not all objects may be added to the factory yet,
    // so check against nullptr is needed.
    auto go = ItemsFactory::Instance().CreateRandomItem(x, y);
    if (go != nullptr)
    {
      InsertGameObject(go);
    }
  }
}

int MapLevelBase::GetEstimatedNumberOfItemsToCreate()
{
  // log_2(n) = log_e(n) / log_e(2)

  double count = std::log(EmptyCells().size()) / std::log(2);
  int itemsToCreate = static_cast<int>(std::ceil(count));
  itemsToCreate = RNG::Instance().RandomRange(itemsToCreate / 2, itemsToCreate);

  return itemsToCreate;
}

void MapLevelBase::PlaceRandomShrine(LevelBuilder& lb)
{
  // TODO: only certain shrine types for certain levels.
  std::vector<Position> possibleSpots;

  for (int x = 3; x < MapSize.X - 6; x++)
  {
    for (int y = 1; y < MapSize.Y - 7; y++)
    {
      if (lb.MapRaw[x][y] == '.')
      {
        possibleSpots.push_back({ x, y });
      }
    }
  }

  // Just in case
  if (possibleSpots.empty())
  {
    //DebugLog("couldn't find possible spots!\n");
    return;
  }

  int index = RNG::Instance().RandomRange(0, possibleSpots.size());
  Position p = possibleSpots[index];

  index = RNG::Instance().RandomRange(0, GlobalConstants::ShrineLayoutsByType.size());
  auto it = GlobalConstants::ShrineLayoutsByType.begin();
  std::advance(it, index);
  ShrineType type = it->first;
  int layoutIndex = RNG::Instance().RandomRange(0, it->second.size());
  auto l = it->second[layoutIndex];
  lb.PlaceShrineLayout(p, l);

  auto& sbp = lb.ShrinesByPosition();

  // Shrine position is always assumed to be in the center of the layout
  sbp[{ p.X + 2, p.Y + 2 }] = type;
}

void MapLevelBase::PlaceStairs()
{
  int startIndex = RNG::Instance().RandomRange(0, _emptyCells.size());

  LevelStart.X = _emptyCells[startIndex].X;
  LevelStart.Y = _emptyCells[startIndex].Y;

  MapType stairsDownTo = (MapType)(DungeonLevel + 1);
  MapType stairsUpTo   = (MapType)(DungeonLevel - 1);

  GameObjectsFactory::Instance().CreateStairs(this, LevelStart.X, LevelStart.Y, '<', stairsUpTo);

  _emptyCells.erase(_emptyCells.begin() + startIndex);

  int endIndex = RNG::Instance().RandomRange(0, _emptyCells.size());

  LevelExit.X = _emptyCells[endIndex].X;
  LevelExit.Y = _emptyCells[endIndex].Y;

  GameObjectsFactory::Instance().CreateStairs(this, LevelExit.X, LevelExit.Y, '>', stairsDownTo);
}

void MapLevelBase::CreateInitialMonsters()
{
  MaxMonsters = (size_t)std::ceil(std::log(_emptyCells.size()));

  // FIXME: debug
  //MaxMonsters = 1;

  for (size_t i = 0; i < MaxMonsters; i++)
  {
    int index = RNG::Instance().RandomRange(0, _emptyCells.size());

    int x = _emptyCells[index].X;
    int y = _emptyCells[index].Y;

    bool spawnOk = IsSpotValidForSpawn({ x, y });
    if (spawnOk && !_monstersSpawnRateForThisLevel.empty())
    {
      auto res = Util::WeightedRandom(_monstersSpawnRateForThisLevel);

      // FIXME: debug
      //res = { GameObjectType::SPIDER, 1 };

      auto monster = GameObjectsFactory::Instance().CreateMonster(x, y, res.first);
      InsertActor(monster);
    }
  }

  CreateSpecialMonsters();
}

bool MapLevelBase::IsSpotValidForSpawn(const Position& pos)
{
  bool blocked  = IsCellBlocking(pos);
  bool occupied = false;
  bool danger   = Map::Instance().IsTileDangerous(pos);

  for (auto& i : ActorGameObjects)
  {
    if (i->PosX == pos.X && i->PosY == pos.Y)
    {
      occupied = true;
      break;
    }
  }

  return (!blocked && !occupied && !danger);
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

  int attempts = 3;

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
     && IsSpotValidForSpawn({ cx, cy }))
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
  if (!Util::IsInsideMap(pos, MapSize))
  {
    return true;
  }

  bool groundBlock = MapArray[pos.X][pos.Y]->Blocking;
  bool staticBlock = false;
  if (StaticMapObjects[pos.X][pos.Y] != nullptr)
  {
    staticBlock = StaticMapObjects[pos.X][pos.Y]->Blocking;
  }

  return (groundBlock || staticBlock);
}

void MapLevelBase::PlaceGroundTile(int x, int y,
                                   int image,
                                   const std::string& fgColor,
                                   const std::string& bgColor,
                                   const std::string& objName)
{
  GameObjectInfo t;
  t.Set(false, false, image, fgColor, bgColor, objName);
  MapArray[x][y]->MakeTile(t);
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
  std::string flowerColor = Colors::GrassDotColor;

  int colorChoice = RNG::Instance().RandomRange(0, maxDiceRoll);
  if      (colorChoice == 0) flowerColor = Colors::WhiteColor;
  else if (colorChoice == 1) flowerColor = Colors::DandelionYellowColor;
  else if (colorChoice == 2) flowerColor = Colors::RedPoppyColor;

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
  t.Set(false, false, img, flowerColor, Colors::GrassColor, tileName);

  MapArray[x][y]->MakeTile(t);
}

void MapLevelBase::PlaceShallowWaterTile(int x, int y)
{
  GameObjectInfo t;
  t.Set(false, false, '~', Colors::WhiteColor, Colors::ShallowWaterColor, "Shallow Water");
  MapArray[x][y]->MakeTile(t, GameObjectType::SHALLOW_WATER);
}

void MapLevelBase::PlaceDeepWaterTile(int x, int y)
{
  // int type is to avoid truncation
  // in case of CP437 image which is 247
  int img = '~';

  #ifdef USE_SDL
  img = GlobalConstants::CP437IndexByType[NameCP437::WAVES];
  #endif

  GameObjectInfo t;
  t.Set(false, false, img, Colors::WhiteColor, Colors::DeepWaterColor, "Deep Water");
  MapArray[x][y]->MakeTile(t, GameObjectType::DEEP_WATER);
}

void MapLevelBase::PlaceLavaTile(int x, int y)
{
  GameObjectInfo t;
  t.Set(false, false, '~', Colors::LavaWavesColor, Colors::LavaColor, "Lava");
  MapArray[x][y]->MakeTile(t, GameObjectType::LAVA);
}

void MapLevelBase::PlaceChasmTile(int x, int y)
{
  int img = ' ';

#ifdef USE_SDL
  img = GlobalConstants::CP437IndexByType[NameCP437::SHADING_3];

  std::string fgColor = Colors::ShadesOfGrey::Three;
  std::string bgColor = Colors::BlackColor;
#else
  std::string fgColor = Colors::BlackColor;
  std::string bgColor = Colors::BlackColor;
#endif

  GameObjectInfo t;
  t.Set(false, false, img, fgColor, bgColor, "Chasm");
  MapArray[x][y]->MakeTile(t, GameObjectType::CHASM);
}

void MapLevelBase::PlaceShrine(const Position& pos, LevelBuilder& lb)
{
  GameObjectInfo t;
  ShrineType type = lb.ShrinesByPosition().at(pos);
  auto go = GameObjectsFactory::Instance().CreateShrine(pos.X, pos.Y, type, 1000);
  InsertGameObject(go);

  std::string description = GlobalConstants::ShrineNameByType.at(type);
  t.Set(true, false, '/', Colors::ShadesOfGrey::Four, Colors::BlackColor, description, "?Shrine?");
  InsertStaticObject(pos.X, pos.Y, t);
}

void MapLevelBase::PlaceTree(int x, int y)
{
  char img = 'T';

  #ifdef USE_SDL
  img = GlobalConstants::CP437IndexByType[NameCP437::CLUB];
  #endif

  GameObjectInfo t;
  t.Set(true, true, img, Colors::GreenColor, Colors::BlackColor, "Tree");
  InsertStaticObject(x, y, t);
}

void MapLevelBase::PlaceWall(int x, int y,
                             int image,
                             const std::string& fgColor,
                             const std::string& bgColor,
                             const std::string& objName,
                             GameObjectType pickaxeable)
{
  GameObjectInfo t;
  t.Set(true, true, image, fgColor, bgColor, objName);
  InsertStaticObject(x, y, t, -1, pickaxeable);
}

void MapLevelBase::PlaceDoor(int x, int y, bool isOpen, size_t openedBy, const std::string& objName)
{
  GameObject* door = GameObjectsFactory::Instance().CreateDoor(x, y, isOpen, DoorMaterials::WOOD, objName);
  if (openedBy != GlobalConstants::OpenedByAnyone)
  {
    DoorComponent* dc = door->GetComponent<DoorComponent>();
    dc->OpenedBy = openedBy;
  }
  InsertStaticObject(door);
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

void MapLevelBase::FillArea(int ax, int ay, int aw, int ah, const GameObjectInfo& tileToFill)
{
  for (int x = ax; x <= ax + aw; x++)
  {
    for (int y = ay; y <= ay + ah; y++)
    {
      MapArray[x][y]->MakeTile(tileToFill);
    }
  }
}

void MapLevelBase::CreateSpecialLevel()
{
  // For overriding procedural design generation
}

void MapLevelBase::CreateSpecialMonsters()
{
  // For creation of mini-bosses
}
