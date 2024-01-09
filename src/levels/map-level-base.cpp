#include "map-level-base.h"
#include "application.h"
#include "game-objects-factory.h"
#include "gid-generator.h"
#include "monsters-inc.h"
#include "items-factory.h"
#include "printer.h"
#include "door-component.h"
#include "map.h"

#ifdef DEBUG_BUILD
#include "logger.h"
#endif

MapLevelBase::MapLevelBase(int sizeX, int sizeY, MapType type, int dungeonLevel)
{
  MapSize.X = sizeX;
  MapSize.Y = sizeY;
  MapType_ = type;
  DungeonLevel = dungeonLevel;

  std::string levelName;

  if (MapType_ == MapType::TOWN)
  {
    auto dm = Util::GetDayAndMonth();
    levelName = Util::GetTownName(dm);
    if (levelName.empty())
    {
      levelName = "Village of " + Util::GenerateName(false,
                                                     true,
                                                     GlobalConstants::TownNameEndings);
    }
  }
  else
  {
    levelName = GlobalConstants::MapLevelNames.at(MapType_);
  }

  LevelName = levelName;

  _playerRef = &Application::Instance().PlayerInstance;
}

// =============================================================================

MapLevelBase::~MapLevelBase()
{
  GlobalTriggers.clear();
  FinishTurnTriggers.clear();
  ActorGameObjects.clear();
  GameObjects.clear();
  StaticMapObjects.clear();
  MapArray.clear();
}

// =============================================================================

void MapLevelBase::PrepareMap()
{
  MapArray.reserve(MapSize.X);
  StaticMapObjects.reserve(MapSize.X);

  GameObjects.reserve(100);
  ActorGameObjects.reserve(100);

  FinishTurnTriggers.reserve(100);
  GlobalTriggers.reserve(100);

  for (int x = 0; x < MapSize.X; x++)
  {
    std::vector<std::unique_ptr<GameObject>> row;
    std::vector<std::unique_ptr<GameObject>> rowStatic;

    row.reserve(MapSize.Y);
    rowStatic.reserve(MapSize.Y);

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
      MapArray[x][y]->Init(this,
                           x,
                           y,
                           '?',
                           Colors::WhiteColor,
                           Colors::MagentaColor);
    }
  }
}

// =============================================================================

void MapLevelBase::AdjustCamera()
{
  int tw = Printer::TerminalWidth;
  int th = Printer::TerminalHeight;

  MapOffsetX = tw / 2 - _playerRef->PosX;
  MapOffsetY = th / 2 - _playerRef->PosY;
}

// =============================================================================

const int& MapLevelBase::RespawnCounter()
{
  return _respawnCounter;
}

// =============================================================================

const std::vector<Position>& MapLevelBase::EmptyCells()
{
  return _emptyCells;
}

// =============================================================================

#ifdef DEBUG_BUILD
GameObject* MapLevelBase::FindObjectByAddress(const std::string& addressString)
{
  GameObject* res = nullptr;

  if (_playerRef->HexAddressString == addressString)
  {
    return _playerRef;
  }

  res = FindInVV(MapArray, addressString);
  if (res == nullptr)
  {
    res = FindInVV(StaticMapObjects, addressString);
    if (res == nullptr)
    {
      res = FindInV(GameObjects, addressString);
      if (res == nullptr)
      {
        res = FindInV(ActorGameObjects, addressString);
        if (res == nullptr)
        {
          res = FindInV(FinishTurnTriggers, addressString);
        }
      }
    }
  }

  return res;
}
#endif

// =============================================================================

void MapLevelBase::PlaceActor(GameObject* actor)
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

// =============================================================================

void MapLevelBase::PlaceGameObject(GameObject* goToInsert)
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

// =============================================================================

void MapLevelBase::PlaceStaticObject(int x, int y,
                                     const GameObjectInfo& objectInfo,
                                     int hitPoints,
                                     GameObjectType type)
{
  if (IsOutOfBounds(x, y))
  {
    return;
  }

  GameObject* go = GameObjectsFactory::Instance().CreateStaticObject(x, y,
                                                                     objectInfo,
                                                                     hitPoints,
                                                                     type);
  PlaceStaticObject(go);
}

// =============================================================================

void MapLevelBase::PlaceStaticObject(GameObject* goToInsert)
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

// =============================================================================

void MapLevelBase::PlaceTrigger(GameObject* trigger, TriggerUpdateType updateType)
{
  if (trigger == nullptr)
  {
    #ifdef DEBUG_BUILD
    std::string str = "[WARNING] tried to insert null trigger object!";
    Printer::Instance().AddMessage(str);
    Logger::Instance().Print(str);
    DebugLog("%s\n", str.data());
    #endif
    return;
  }

  switch (updateType)
  {
    case TriggerUpdateType::FINISH_TURN:
      FinishTurnTriggers.push_back(std::unique_ptr<GameObject>(trigger));
      break;

    case TriggerUpdateType::GLOBAL:
      GlobalTriggers.push_back(std::unique_ptr<GameObject>(trigger));
      break;
  }
}

// =============================================================================

void MapLevelBase::RecordEmptyCells()
{
  _emptyCells.clear();

  for (int x = 0; x < MapSize.X; x++)
  {
    for (int y = 0; y < MapSize.Y; y++)
    {
      if (!IsCellBlocking({ x, y })
       && (MapArray[x][y]->ZoneMarker == TransformedRoom::UNMARKED
        || MapArray[x][y]->ZoneMarker == TransformedRoom::EMPTY))
      {
        Position pos(x, y);
        _emptyCells.push_back(pos);
      }
    }
  }
}

// =============================================================================

void MapLevelBase::CreateBorders(char img,
                                 uint32_t fgColor,
                                 uint32_t bgColor,
                                 const std::string& objectName)
{
  using GOF = GameObjectsFactory;

  GameObjectInfo oi;

  oi.Image        = img;
  oi.FgColor      = fgColor;
  oi.BgColor      = bgColor;
  oi.IsBlocking   = true;
  oi.BlocksSight  = true;
  oi.ObjectName   = objectName;

  auto bounds = Util::GetPerimeter(0, 0, MapSize.X - 1, MapSize.Y - 1, true);
  for (auto& i : bounds)
  {
    //
    // Borders are to ignore IsOutOfBounds() check, so pasting contents
    // of PlaceStaticObject() method directly.
    //
    GameObject* go = GOF::Instance().CreateStaticObject(i.X,
                                                        i.Y,
                                                        oi,
                                                       -1,
                                                        GameObjectType::BORDER);
    PlaceStaticObject(go);
  }
}

// =============================================================================

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

    //
    // NOTE: Not all objects may have been added
    // to the factory yet, so check against nullptr is needed.
    //
    auto go = ItemsFactory::Instance().CreateRandomItem(x, y);
    if (go != nullptr)
    {
      ItemComponent* ic = go->GetComponent<ItemComponent>();
      if (Util::CanBeSpawned(ic))
      {
        PlaceGameObject(go);
      }
      else
      {
        delete go;
      }
    }
  }
}

// =============================================================================

void MapLevelBase::MaskToBoolFlags(const uint16_t mask)
{
  std::map<int, bool&> traverseMap =
  {
    { 0, WelcomeTextDisplayed   },
    { 1, Peaceful               },
    { 2, ExitFound              },
    { 3, MysteriousForcePresent },
  };

  uint64_t index = 0x1;

  for (auto& kvp : traverseMap)
  {
    kvp.second = (mask & index);
    index <<= 1;
  }
}

// =============================================================================

int MapLevelBase::GetEstimatedNumberOfItemsToCreate()
{
  double count = std::log2(EmptyCells().size());
  int itemsToCreate = static_cast<int>(std::ceil(count));
  itemsToCreate = RNG::Instance().RandomRange(1, (itemsToCreate / 2) + 1);

  return itemsToCreate;
}

// =============================================================================

void MapLevelBase::PlaceRandomShrine(LevelBuilder& lb)
{
  // TODO: only certain shrine types for certain levels.
  std::vector<Position> possibleSpots;

  for (int x = 3; x < MapSize.X - 6; x++)
  {
    for (int y = 1; y < MapSize.Y - 7; y++)
    {
      auto& cell = lb.MapRaw[x][y];

      if (cell == '.')
      {
        possibleSpots.push_back({ x, y });
      }
    }
  }

  //
  // Just in case...
  //
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

  //
  // Shrine position is always assumed to be in the center of the layout.
  //
  sbp[{ p.X + 2, p.Y + 2 }] = type;
}

// =============================================================================

void MapLevelBase::PlaceStairs()
{
  int startIndex = RNG::Instance().RandomRange(0, _emptyCells.size());

  LevelStart.X = _emptyCells[startIndex].X;
  LevelStart.Y = _emptyCells[startIndex].Y;

  MapType stairsDownTo = (MapType)(DungeonLevel + 1);
  MapType stairsUpTo   = (MapType)(DungeonLevel - 1);

  GameObjectsFactory::Instance().CreateStairs(this,
                                              LevelStart.X,
                                              LevelStart.Y,
                                              '<',
                                              stairsUpTo);

  _emptyCells.erase(_emptyCells.begin() + startIndex);

  int endIndex = RNG::Instance().RandomRange(0, _emptyCells.size());

  LevelExit.X = _emptyCells[endIndex].X;
  LevelExit.Y = _emptyCells[endIndex].Y;

  GameObjectsFactory::Instance().CreateStairs(this,
                                              LevelExit.X,
                                              LevelExit.Y,
                                              '>',
                                              stairsDownTo);

  _emptyCells.erase(_emptyCells.begin() + endIndex);
}

// =============================================================================

void MapLevelBase::CreateInitialMonsters()
{
  MaxMonsters = (size_t)std::ceil(std::log2(_emptyCells.size()));

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

      auto monster = MonstersInc::Instance().CreateMonster(x, y, res.first);
      PlaceActor(monster);
    }
  }

  CreateSpecialMonsters();
}

// =============================================================================

bool MapLevelBase::IsOutOfBounds(int x, int y)
{
  return !Util::IsInsideMap({ x, y }, MapSize);
}

// =============================================================================

bool MapLevelBase::IsSpotValidForSpawn(const Position& pos)
{
  auto& map = Map::Instance().CurrentLevel->MapArray;

  bool blocked   = IsCellBlocking(pos);
  bool occupied  = false;
  bool danger    = Map::Instance().IsTileDangerous(pos);
  bool farEnough = false;
  bool unmarked  = (map[pos.X][pos.Y]->ZoneMarker == TransformedRoom::UNMARKED
                 || map[pos.X][pos.Y]->ZoneMarker == TransformedRoom::EMPTY);
  bool special   = map[pos.X][pos.Y]->Special;

  int distanceToPlayer = Util::BlockDistance(_playerRef->GetPosition(), pos);

  int spawnPointMinDistance = 20;

  //
  // If map size is greater than spawnPointMinDistance
  // in block direction of 80x25 terminal, potential spawn points
  // distance should be greater than this value.
  //
  // Otherwise take minimum of MapSize values,
  // halve it and compare with that.
  //
  int mapMinSize = std::min(MapSize.X, MapSize.Y);
  if (mapMinSize < spawnPointMinDistance)
  {
    spawnPointMinDistance = mapMinSize / 2;

    //
    // Add certain remainder after each dimension increase since 2x2
    // to get maximum available block distance for the square map
    // of that dimension.
    //
    // E.g.:
    //
    //  12
    // S--
    // ..|3
    // ..E4
    //
    //  123
    // S---
    // ...|4
    // ...|5
    // ...E6
    //
    //  1234
    // S----
    // ....|5
    // ....|6
    // ....|7
    // ....E8
    //
    // 3x3 have maximum block distance of 3 + (3 - 2) = 4
    // 4x4 -> 4 + (4 - 2) = 6
    // 5x5 -> 5 + (5 - 2) = 8
    // and so on
    //
    spawnPointMinDistance += (spawnPointMinDistance - 2);
  }

  farEnough = (distanceToPlayer >= spawnPointMinDistance);

  if (!farEnough)
  {
    return false;
  }

  for (auto& i : ActorGameObjects)
  {
    if (i->PosX == pos.X && i->PosY == pos.Y)
    {
      occupied = true;
      break;
    }
  }

  return (!blocked && !occupied && !danger && unmarked && !special);
}

// =============================================================================

void MapLevelBase::TryToSpawnMonsters()
{
  if (_respawnCounter < MonstersRespawnTurns)
  {
    //
    // To average out monsters' respawning speed,
    // adjust respawn counter with regards to player's SPD.
    //
    _respawnCounter += (_playerRef->Attrs.Spd.Get() <= 0)
                       ? 1
                       : (_playerRef->Attrs.Spd.Get() * GlobalConstants::TurnTickValue);
    return;
  }

  _respawnCounter = 0;

  if (_monstersSpawnRateForThisLevel.empty()
    || (ActorGameObjects.size() >= MaxMonsters))
  {
    return;
  }

  int index = RNG::Instance().RandomRange(0, _emptyCells.size());

  int cx = _emptyCells[index].X;
  int cy = _emptyCells[index].Y;

  //
  // Spawn monsters on cells invisible to the player.
  //
  if (!MapArray[cx][cy]->Visible
   && IsSpotValidForSpawn({ cx, cy }))
  {
    auto res = Util::WeightedRandom(_monstersSpawnRateForThisLevel);
    auto monster = MonstersInc::Instance().CreateMonster(cx, cy, res.first);
    PlaceActor(monster);
  }
}

// =============================================================================

void MapLevelBase::DisplayWelcomeText()
{
  std::vector<std::string> msg =
  {
    "You're not supposed to see this text.",
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT, "MapLevelBase", msg);
}

// =============================================================================

void MapLevelBase::OnLevelChanged(MapType from)
{
}

// =============================================================================

void MapLevelBase::Serialize(NRS& saveTo)
{
  SerializeLayout(saveTo);
  SerializeObjects(saveTo);
  SerializeItems(saveTo);
  SerializeTriggers(saveTo);
  SerializeActors(saveTo);
}

// =============================================================================

void MapLevelBase::SerializeLayout(NRS& saveTo)
{
  namespace SK = Strings::SerializationKeys;

  NRS& root = saveTo[SK::Root];

  NRS& levelNode = root[std::to_string((int)MapType_)];

  levelNode[SK::Size].SetInt(MapSize.X, 0);
  levelNode[SK::Size].SetInt(MapSize.Y, 1);

  levelNode[SK::Name].SetString(LevelName);

  levelNode[SK::Visibility].SetInt(VisibilityRadius);
  levelNode[SK::Respawn].SetInt(MonstersRespawnTurns);

  uint16_t mask = Util::BoolFlagsToMask({
                                          WelcomeTextDisplayed,
                                          Peaceful,
                                          ExitFound,
                                          MysteriousForcePresent
                                        });

  levelNode[SK::Mask].SetUInt(mask);

  // ---------------- BUILD OBJECTS LAYOUT LIBRARY ----------------

  using SDM = GameObject::SaveDataMinimal;
  std::unordered_map<std::string, SDM> saveData;

  auto WriteSaveData = [this, &saveData](GameObject* go)
  {
    if (go == nullptr)
    {
      return;
    }

    const SDM& d = go->GetSaveDataMinimal();

    std::string key = d.ToStringKey();

    if (saveData.count(key) == 0)
    {
      saveData[d.ToStringKey()] = d;
    }
  };

  for (int y = 0; y < MapSize.Y; y++)
  {
    for (int x = 0; x < MapSize.X; x++)
    {
      WriteSaveData(MapArray[x][y].get());

      GameObject* so = StaticMapObjects[x][y].get();
      if (so != nullptr && (so->Type == GameObjectType::PICKAXEABLE
                         || so->Type == GameObjectType::BORDER))
      {
        WriteSaveData(StaticMapObjects[x][y].get());
      }
    }
  }

  std::unordered_map<std::string, int> indexByKey;

  {
    NRS& node = levelNode[SK::MapObjects];

    int index = 0;
    for (auto& kvp : saveData)
    {
      const SDM& sdm = kvp.second;

      indexByKey[sdm.ToStringKey()] = index;

      NRS& n = node[std::to_string(index)];

      n[SK::Type].SetInt((int)sdm.Type);
      n[SK::Zone].SetInt((int)sdm.ZoneMarker);
      n[SK::Image].SetInt(sdm.Image);
      n[SK::Color].SetString(Util::NumberToHexString(sdm.FgColor), 0);
      n[SK::Color].SetString(Util::NumberToHexString(sdm.BgColor), 1);
      n[SK::Name].SetString(sdm.Name);

      if (!sdm.FowName.empty())
      {
        n[SK::FowName].SetString(sdm.FowName);
      }

      n[SK::Mask].SetUInt(sdm.Mask);

      index++;
    }
  }

  // ---------------- SAVE MAP LAYOUT ----------------

  std::string cell;

  int lineInd = 0;

  NRS& levelLayout = levelNode[SK::Layout];

  for (int y = 0; y < MapSize.Y; y++)
  {
    int listInd = 0;

    NRS& n = levelLayout[std::to_string(lineInd)];

    for (int x = 0; x < MapSize.X; x++)
    {
      const SDM& sdm = MapArray[x][y]->GetSaveDataMinimal();

      const std::string& key = sdm.ToStringKey();

      int ind1 = indexByKey[key];

      GameObject* so = StaticMapObjects[x][y].get();

      if (so != nullptr
      && (so->Type == GameObjectType::PICKAXEABLE
       || so->Type == GameObjectType::BORDER))
      {
        const SDM& sdm = so->GetSaveDataMinimal();

        const std::string& key = sdm.ToStringKey();

        int ind2 = indexByKey[key];

        cell = Util::StringFormat("%d|%d", ind1, ind2);
      }
      else
      {
        cell = Util::StringFormat("%d|", ind1);
      }

      n.SetString(cell, listInd);

      listInd++;
    }

    lineInd++;
  }
}

// =============================================================================

void MapLevelBase::SerializeObjects(NRS& saveTo)
{
  namespace SK = Strings::SerializationKeys;

  NRS& root    = saveTo[SK::Root];
  NRS& objects = root[SK::Objects];

  int index = 0;

  for (int y = 0; y < MapSize.Y; y++)
  {
    for (int x = 0; x < MapSize.X; x++)
    {
      GameObject* so = StaticMapObjects[x][y].get();

      if (so != nullptr
      && (so->Type != GameObjectType::PICKAXEABLE
       && so->Type != GameObjectType::BORDER))
      {
        NRS& node = objects[std::to_string(index)];
        so->Serialize(node);

        index++;
      }
    }
  }
}

// =============================================================================

void MapLevelBase::SerializeItems(NRS& saveTo)
{
  // TODO:
}

// =============================================================================

void MapLevelBase::SerializeTriggers(NRS& saveTo)
{
  // TODO:
}

// =============================================================================

void MapLevelBase::SerializeActors(NRS& saveTo)
{
  namespace SK = Strings::SerializationKeys;

  NRS& root = saveTo[SK::Root];

  // TODO:
}

// =============================================================================

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

// =============================================================================

void MapLevelBase::PlaceGroundTile(int x, int y,
                                   int image,
                                   const uint32_t& fgColor,
                                   const uint32_t& bgColor,
                                   const std::string& objName)
{
  if (IsOutOfBounds(x, y))
  {
    return;
  }

  GameObjectInfo t;
  t.Set(false, false, image, fgColor, bgColor, objName);
  MapArray[x][y]->MakeTile(t);
}

// =============================================================================

//
// Places grass tile at [x; y], maxDiceRoll serves as a
// "frequency" modifier - the more its value, the less is the chance
// for flowers to appear.
//
void MapLevelBase::PlaceGrassTile(int x, int y, int maxDiceRoll)
{
  if (IsOutOfBounds(x, y))
  {
    return;
  }

  char img = '.';

  // Create 'flowers'
  //int tileChoice = RNG::Instance().RandomRange(0, 10);
  //if (tileChoice < 2) img = '.';

  //uint32_t flowerColor = GlobalConstants::BlackColor;
  uint32_t flowerColor = Colors::GrassDotColor;

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

  std::string tileName = Strings::TileNames::GrassText;
  if (flowersNameByChoice.count(colorChoice) == 1)
  {
    tileName = flowersNameByChoice[colorChoice];
  }

  GameObjectInfo t;
  t.Set(false, false, img, flowerColor, Colors::GrassColor, tileName);

  MapArray[x][y]->MakeTile(t);
}

// =============================================================================

void MapLevelBase::PlaceShallowWaterTile(int x, int y)
{
  if (IsOutOfBounds(x, y))
  {
    return;
  }

  GameObjectInfo t;
  t.Set(false,
        false,
        '~',
        Colors::WhiteColor,
        Colors::ShallowWaterColor,
        Strings::TileNames::ShallowWaterText);
  MapArray[x][y]->MakeTile(t, GameObjectType::SHALLOW_WATER);
}

// =============================================================================

void MapLevelBase::PlaceDeepWaterTile(int x, int y)
{
  if (IsOutOfBounds(x, y))
  {
    return;
  }

  //
  // int type is to avoid truncation
  // in case of CP437 image which is 247
  //
  int img = '~';

  #ifdef USE_SDL
  img = GlobalConstants::CP437IndexByType[NameCP437::WAVES];
  #endif

  GameObjectInfo t;
  t.Set(false,
        false,
        img,
        Colors::WhiteColor,
        Colors::DeepWaterColor,
        Strings::TileNames::DeepWaterText);
  MapArray[x][y]->MakeTile(t, GameObjectType::DEEP_WATER);
}

// =============================================================================

void MapLevelBase::PlaceLavaTile(int x, int y)
{
  if (IsOutOfBounds(x, y))
  {
    return;
  }

  GameObjectInfo t;
  t.Set(false,
        false,
        '~',
        Colors::LavaWavesColor,
        Colors::LavaColor,
        Strings::TileNames::LavaText);
  MapArray[x][y]->MakeTile(t, GameObjectType::LAVA);
}

// =============================================================================

void MapLevelBase::PlaceChasmTile(int x, int y)
{
  if (IsOutOfBounds(x, y))
  {
    return;
  }

  int img = ' ';

#ifdef USE_SDL
  img = GlobalConstants::CP437IndexByType[NameCP437::SHADING_3];

  uint32_t fgColor = Colors::ShadesOfGrey::Three;
  uint32_t bgColor = Colors::BlackColor;
#else
  uint32_t fgColor = Colors::BlackColor;
  uint32_t bgColor = Colors::BlackColor;
#endif

  GameObjectInfo t;
  t.Set(false,
        false,
        img,
        fgColor,
        bgColor,
        Strings::TileNames::ChasmText);
  MapArray[x][y]->MakeTile(t, GameObjectType::CHASM);
}

// =============================================================================

void MapLevelBase::PlaceShrine(const Position& pos, LevelBuilder& lb)
{
  if (IsOutOfBounds(pos.X, pos.Y))
  {
    return;
  }

  GameObjectInfo t;
  ShrineType type = lb.ShrinesByPosition().at(pos);
  auto go = GameObjectsFactory::Instance().CreateShrine(pos.X, pos.Y, type, 1000);
  PlaceGameObject(go);

  std::string description = GlobalConstants::ShrineNameByType.at(type);
  t.Set(true,
        false,
        '/',
        Colors::ShadesOfGrey::Four,
        Colors::BlackColor,
        description,
        "?Shrine?");

  PlaceStaticObject(pos.X, pos.Y, t);
}

// =============================================================================

void MapLevelBase::PlaceShrine(const Position& pos, ShrineType type)
{
  if (IsOutOfBounds(pos.X, pos.Y))
  {
    return;
  }

  GameObjectInfo t;
  auto go = GameObjectsFactory::Instance().CreateShrine(pos.X, pos.Y, type, 1000);
  PlaceGameObject(go);

  std::string description = GlobalConstants::ShrineNameByType.at(type);
  t.Set(true,
        false,
        '/',
        Colors::ShadesOfGrey::Four,
        Colors::BlackColor,
        description,
        "?Shrine?");

  PlaceStaticObject(pos.X, pos.Y, t);
}

// =============================================================================

void MapLevelBase::PlaceTree(int x, int y)
{
  if (IsOutOfBounds(x, y))
  {
    return;
  }

  char img = 'T';

  #ifdef USE_SDL
  img = GlobalConstants::CP437IndexByType[NameCP437::CLUB];
  #endif

  GameObjectInfo t;
  t.Set(true, true, img, Colors::GreenColor, Colors::BlackColor, Strings::TileNames::TreeText);
  PlaceStaticObject(x, y, t);
}

// =============================================================================

void MapLevelBase::PlaceWall(int x, int y,
                             int image,
                             const uint32_t& fgColor,
                             const uint32_t& bgColor,
                             const std::string& objName,
                             bool cannotBePickaxed)
{
  if (IsOutOfBounds(x, y))
  {
    return;
  }

  GameObjectInfo t;
  t.Set(true, true, image, fgColor, bgColor, objName);

  //
  // HP is hardcoded to 1 to set Attrs.Indestructible flag to false,
  // but at the same time GameObjectType::PICKAXEABLE will be checked
  // in GameObject::ReceiveDamage() to prevent destruction of walls
  // by not wielding a pickaxe.
  // Attrs.Indestructible = false will allow us to go into shouldTearDownWall
  // branch in Player::ProcessMeleeAttack() to allow walls to be destroyed
  // using pickaxe.
  //
  PlaceStaticObject(x,
                    y,
                    t,
                    cannotBePickaxed ? -1 : 1,
                    GameObjectType::PICKAXEABLE);
}

// =============================================================================

void MapLevelBase::PlaceDoor(int x, int y,
                             bool isOpen,
                             size_t openedBy,
                             const std::string& objName)
{
  if (IsOutOfBounds(x, y))
  {
    return;
  }

  GameObject* door = GameObjectsFactory::Instance().CreateDoor(x, y, isOpen, DoorMaterials::WOOD, objName);
  if (openedBy != GlobalConstants::OpenedByAnyone)
  {
    DoorComponent* dc = door->GetComponent<DoorComponent>();
    dc->OpenedBy = openedBy;
  }

  PlaceStaticObject(door);
}

// =============================================================================

void MapLevelBase::CreateLevel()
{
  auto str = Util::StringFormat("%s, %s - no level was created!",
                                __PRETTY_FUNCTION__, LevelName.data());
  LogPrint(str, true);
}

// =============================================================================

void MapLevelBase::ConstructFromBuilder(LevelBuilder& lb)
{
  for (int x = 0; x < MapSize.X; x++)
  {
    for (int y = 0; y < MapSize.Y; y++)
    {
      auto map = lb.GeneratedMap();

      //
      // This ensures that all common objects will share the same
      // visual style that is defined for the current map.
      //
      CreateCommonObjects(x, y, lb.MapRaw[x][y]);

      if (map[x][y].ZoneMarker != TransformedRoom::UNMARKED)
      {
        CreateSpecialObjects(x, y, map[x][y]);
      }
    }
  }
}

// =============================================================================

void MapLevelBase::CreateGround(char img,
                                uint32_t fgColor,
                                uint32_t bgColor,
                                const std::string& tileName)
{
  GameObjectInfo t;
  t.Set(false, false, img, fgColor, bgColor, tileName);

  FillArea(0, 0, MapSize.X - 1, MapSize.Y - 1, t);
}

// =============================================================================

void MapLevelBase::FillArea(int ax,
                            int ay,
                            int aw,
                            int ah,
                            const GameObjectInfo& tileToFill)
{
  for (int x = ax; x <= ax + aw; x++)
  {
    for (int y = ay; y <= ay + ah; y++)
    {
      MapArray[x][y]->MakeTile(tileToFill);
    }
  }
}

// =============================================================================

void MapLevelBase::CreateSpecialLevel()
{
  // For overriding procedural design generation
}

void MapLevelBase::CreateSpecialMonsters()
{
  // For creation of mini-bosses
}

// =============================================================================

void MapLevelBase::CreateSpecialObjects(int x, int y, const MapCell& cell)
{
  MapArray[x][y]->ZoneMarker = cell.ZoneMarker;

  switch (cell.ZoneMarker)
  {
    case TransformedRoom::SHRINE:
    {
      if (std::holds_alternative<ShrineType>(cell.ObjectHere))
      {
        ShrineType t = std::get<ShrineType>(cell.ObjectHere);
        PlaceShrine({ x, y }, t);
      }
    }
    break;

    case TransformedRoom::STORAGE:
    {
      if (std::holds_alternative<GameObjectType>(cell.ObjectHere))
      {
        if (std::get<GameObjectType>(cell.ObjectHere) == GameObjectType::BREAKABLE)
        {
          GameObject* box = GameObjectsFactory::Instance().CreateBreakableObjectWithRandomLoot(x, y, 'B', "Wooden Box", Colors::WoodColor, Colors::BlackColor);
          PlaceStaticObject(box);
        }
      }
    }
    break;

    case TransformedRoom::TREASURY:
    {
      if (std::holds_alternative<ItemType>(cell.ObjectHere))
      {
        if(std::get<ItemType>(cell.ObjectHere) == ItemType::COINS)
        {
          GameObject* go = ItemsFactory::Instance().CreateMoney();
          go->PosX = x;
          go->PosY = y;
          PlaceGameObject(go);
        }
      }
    }
    break;

    case TransformedRoom::CHESTROOM:
    {
      if (std::holds_alternative<GameObjectType>(cell.ObjectHere))
      {
        if(std::get<GameObjectType>(cell.ObjectHere) == GameObjectType::CONTAINER)
        {
          GameObject* go = GameObjectsFactory::Instance().CreateChest(x, y, Util::Rolld100(50));
          go->PosX = x;
          go->PosY = y;
          PlaceGameObject(go);
        }
      }
    }
    break;
  }
}
