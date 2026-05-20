#ifdef MSVC_COMPILER
#include "game-object.h"
#endif

#include "game-objects-factory.h"
#include "map-level-base.h"
#include "application.h"
#include "gid-generator.h"
#include "monsters-inc.h"
#include "items-factory.h"
#include "printer.h"
#include "door-component.h"
#include "map.h"

#ifdef DEBUG_BUILD
#include "logger.h"
#endif

MapLevelBase::MapLevelBase(int sizeX,
                           int sizeY,
                           MapType type,
                           int dungeonLevel)
{
  LevelExit.X = -1;
  LevelExit.Y = -1;

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
      std::string name = Util::GenerateName(false,
                                            true,
                                            GlobalConstants::TownNameEndings);
      levelName = Util::StringFormat("Village of %s", name.data());
    }
  }
  else
  {
    levelName = GlobalConstants::MapLevelNames.at(MapType_);
  }

  LevelName = levelName;

  _playerRef = &Game::gApp.PlayerInstance;
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

  FowLayer.reserve(MapSize.X);

  for (int x = 0; x < MapSize.X; x++)
  {
    std::vector<std::unique_ptr<GameObject>> row;
    std::vector<std::unique_ptr<GameObject>> rowStatic;

    std::vector<FowObj> fowLine;
    fowLine.reserve(MapSize.Y);

    row.reserve(MapSize.Y);
    rowStatic.reserve(MapSize.Y);

    for (int y = 0; y < MapSize.Y; y++)
    {
      row.push_back(std::unique_ptr<GameObject>(new GameObject()));
      rowStatic.push_back(nullptr);

      fowLine.push_back(FowObj());
    }

    MapArray.push_back(std::move(row));
    StaticMapObjects.push_back(std::move(rowStatic));

    FowLayer.push_back(fowLine);
  }

  for (int x = 0; x < MapSize.X; x++)
  {
    for (int y = 0; y < MapSize.Y; y++)
    {
      MapArray[x][y]->Init(this,
                           x,
                           y,
                           '?',
                           Colors::White,
                           Colors::Magenta);

      MapArray[x][y]->Layer = GameObjectLayer::MAP_ARRAY;
    }
  }
}

// =============================================================================

void MapLevelBase::AdjustCamera()
{
#ifdef USE_SDL
  static int twHalf = Printer::GraphicsWindowWidth / 2;
  static int thHalf = Printer::GraphicsWindowHeight / 2;
#else
  static int twHalf = Printer::TerminalWidth / 2;
  static int thHalf = Printer::TerminalHeight / 2;
#endif

  MapOffsetX = twHalf - _playerRef->PosX;
  MapOffsetY = thHalf - _playerRef->PosY;
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
    Game::gPrnt.AddMessage(str);
    Game::gLogger.Print(str);
    DebugLog("%s\n", str.data());
    #endif

    return;
  }

  actor->Layer = GameObjectLayer::ACTORS;

  ActorGameObjects.push_back(std::unique_ptr<GameObject>(actor));

  //
  // NOTE: standing danger check for actors is performed in
  // GameObject::FinishTurn() -> GameObject::TileStandingCheck()
  // since not all monsters can be killed on otherwise dangerous tiles
  // (e.g. water element cannot drown in deep water,
  // fire mage can walk in lava etc.)
  //
}

// =============================================================================

void MapLevelBase::PlaceGameObject(GameObject* goToInsert)
{
  if (goToInsert == nullptr)
  {
    #ifdef DEBUG_BUILD
    std::string str = "[WARNING] tried to insert null object!";
    Game::gPrnt.AddMessage(str);
    Game::gLogger.Print(str);
    DebugLog("%s\n", str.data());
    #endif

    return;
  }

  goToInsert->Layer = GameObjectLayer::GAME_OBJECTS;

  GameObjects.push_back(std::unique_ptr<GameObject>(goToInsert));

  GameObject* what = GameObjects.back().get();
  GameObjectType t = MapArray[what->PosX][what->PosY]->Type;

  //
  // Assuming all items can perish this way, contrary to actors.
  //
  bool danger = (t == GameObjectType::DEEP_WATER
              || t == GameObjectType::LAVA
              || t == GameObjectType::CHASM);

  if (danger)
  {
    int x = what->PosX;
    int y = what->PosY;

    std::string msg = Util::GetDestroyedByMapString(what, MapArray[x][y].get());
    Game::gPrnt.AddMessage(msg);

    GameObjects.pop_back();
  }
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

  GameObject* go = Game::gGOF.CreateStaticObject(x, y,
                                                 objectInfo,
                                                 hitPoints,
                                                 type);
  go->Layer = GameObjectLayer::STATIC_OBJECTS;
  PlaceStaticObject(go);
}

// =============================================================================

void MapLevelBase::PlaceStaticObject(GameObject* goToInsert)
{
  if (goToInsert == nullptr)
  {
    #ifdef DEBUG_BUILD
    std::string str = "[WARNING] tried to insert null static object!";
    Game::gPrnt.AddMessage(str);
    Game::gLogger.Print(str);
    DebugLog("%s\n", str.data());
    #endif
    return;
  }

  int x = goToInsert->PosX;
  int y = goToInsert->PosY;

  goToInsert->Layer = GameObjectLayer::STATIC_OBJECTS;

  StaticMapObjects[x][y].reset(goToInsert);
}

// =============================================================================

void MapLevelBase::PlaceTrigger(GameObject* triggerObject,
                                TriggerUpdateType updateType)
{
  if (triggerObject == nullptr)
  {
    #ifdef DEBUG_BUILD
    std::string str = "[WARNING] tried to insert null trigger object!";
    Game::gPrnt.AddMessage(str);
    Game::gLogger.Print(str);
    DebugLog("%s\n", str.data());
    #endif
    return;
  }

  triggerObject->Layer = GameObjectLayer::TRIGGERS;

  switch (updateType)
  {
    case TriggerUpdateType::FINISH_TURN:
      FinishTurnTriggers.push_back(std::unique_ptr<GameObject>(triggerObject));
      break;

    case TriggerUpdateType::GLOBAL:
      GlobalTriggers.push_back(std::unique_ptr<GameObject>(triggerObject));
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
                                 const std::string& objectName,
                                 const GraphicTiles graphicTile)
{
  GameObjectInfo oi;

  oi.Graphic.Tile = graphicTile;

  oi.Image        = img;
  oi.FgColor      = fgColor;
  oi.BgColor      = bgColor;
  oi.IsBlocking   = true;
  oi.BlocksSight  = true;
  oi.ObjectName   = objectName;

  auto bounds = Util::GetPerimeter(0, 0, MapSize.X - 1, MapSize.Y - 1);
  for (auto& i : bounds)
  {
    //
    // Borders are to ignore IsOutOfBounds() check, so pasting contents
    // of PlaceStaticObject() method directly.
    //
    GameObject* go = Game::gGOF.CreateStaticObject(i.X,
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

    int index = Game::gRng.RandomRange(0, _emptyCells.size());

    int x = _emptyCells[index].X;
    int y = _emptyCells[index].Y;

    if (!IsSpotValidForSpawn({ x, y }))
    {
      continue;
    }

    //
    // NOTE: Not all objects may have been added to the factory yet, so check
    // against nullptr is needed.
    //
    auto go = Game::gIF.CreateRandomItem(x, y);
    if (go != nullptr)
    {
      //
      // Just to make everything simple, we'll blast fools first and ask
      // questions second: if item is not supposed to be created (yet), we just
      // delete it.
      //
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
  itemsToCreate = Game::gRng.RandomRange(1, (itemsToCreate / 2) + 1);

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

  int index = Game::gRng.RandomRange(0, possibleSpots.size());
  Position p = possibleSpots[index];

  size_t totalLayouts = GlobalConstants::ShrineLayoutsByType.size();
  index = Game::gRng.RandomRange(0, totalLayouts);
  auto it = GlobalConstants::ShrineLayoutsByType.begin();
  std::advance(it, index);
  ShrineType type = it->first;
  int layoutIndex = Game::gRng.RandomRange(0, it->second.size());
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
  int startIndex = Game::gRng.RandomRange(0, _emptyCells.size());

  LevelStart.X = _emptyCells[startIndex].X;
  LevelStart.Y = _emptyCells[startIndex].Y;

  MapType stairsDownTo = (MapType)(DungeonLevel + 1);
  MapType stairsUpTo   = (MapType)(DungeonLevel - 1);

  Game::gGOF.CreateStairs(this,
                          LevelStart.X,
                          LevelStart.Y,
                          '<',
                          stairsUpTo);

  _emptyCells.erase(_emptyCells.begin() + startIndex);

  int endIndex = Game::gRng.RandomRange(0, _emptyCells.size());

  LevelExit.X = _emptyCells[endIndex].X;
  LevelExit.Y = _emptyCells[endIndex].Y;

  Game::gGOF.CreateStairs(this,
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

  for (size_t i = 0; i < MaxMonsters; i++)
  {
    int index = Game::gRng.RandomRange(0, _emptyCells.size());

    int x = _emptyCells[index].X;
    int y = _emptyCells[index].Y;

    bool spawnOk = IsSpotValidForSpawn({ x, y });
    if (spawnOk && !_monstersSpawnRateForThisLevel.empty())
    {
      auto res = Util::WeightedRandom(_monstersSpawnRateForThisLevel);

      auto monster = Game::gMI.CreateMonster(x, y, res.first);
      PlaceActor(monster);
    }
  }

  CreateSpecialMonsters();
}

// =============================================================================

bool MapLevelBase::IsOutOfBounds(int x, int y, bool leaveBorders)
{
  return !Util::IsInsideMap({ x, y }, MapSize, leaveBorders);
}

// =============================================================================

bool MapLevelBase::IsSpotValidForSpawn(const Position& pos)
{
  auto& map = Game::gMap.CurrentLevel->MapArray;

  bool blocked   = IsCellBlocking(pos);
  bool occupied  = false;
  bool danger    = Game::gMap.IsTileDangerous(pos);
  bool farEnough = false;
  bool unmarked  = (map[pos.X][pos.Y]->ZoneMarker == TransformedRoom::UNMARKED
                 || map[pos.X][pos.Y]->ZoneMarker == TransformedRoom::EMPTY);
  bool special   = map[pos.X][pos.Y]->Special;

  int distanceToPlayer = Util::BlockDistance(_playerRef->GetPosition(), pos);

  //
  // Set min distance to block distance of player's light radius + a little bit.
  //
  int spawnPointMinDistance = _playerRef->VisibilityRadius.Get() * 2 + 1;

  //
  // If map size is greater than spawnPointMinDistance in block direction of
  // 80x24 terminal, potential spawn points distance should be greater than this
  // value.
  //
  // Otherwise take minimum of MapSize values, halve it and compare with that.
  //
  int mapMinSize = std::min(MapSize.X, MapSize.Y);
  if (mapMinSize < spawnPointMinDistance)
  {
    spawnPointMinDistance = mapMinSize / 2;

    //
    // Add certain remainder to each dimension increase after 2x2 to get maximum
    // available block distance for the square map of that dimension.
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
    // 3x3 has maximum block distance of 3 + (3 - 2) = 4
    // 4x4 -> 4 + (4 - 2) = 6
    // 5x5 -> 5 + (5 - 2) = 8
    //
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
    // To average out monsters' respawning speed, adjust respawn counter with
    // regards to player's SPD.
    //
    _respawnCounter += (_playerRef->Attrs.Spd.Get() <= 0)
                       ? 1
                       : (_playerRef->Attrs.Spd.Get() *
                          GlobalConstants::TurnTickValue);
    return;
  }

  _respawnCounter = 0;

  if (_monstersSpawnRateForThisLevel.empty()
    || (ActorGameObjects.size() >= MaxMonsters))
  {
    return;
  }

  int index = Game::gRng.RandomRange(0, _emptyCells.size());

  int cx = _emptyCells[index].X;
  int cy = _emptyCells[index].Y;

  //
  // Spawn monsters on cells invisible to the player.
  //
  if (!MapArray[cx][cy]->Visible
   && IsSpotValidForSpawn({ cx, cy }))
  {
    auto res = Util::WeightedRandom(_monstersSpawnRateForThisLevel);
    auto monster = Game::gMI.CreateMonster(cx, cy, res.first);
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

  Game::gApp.ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT,
                             "MapLevelBase",
                             msg);
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

  std::ignore = root;

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

void MapLevelBase::UpdateFowLayer(GameObject* obj)
{
  if (obj == nullptr)
  {
    DebugLog("UpdateFowLayer() called on nullptr!");
    return;
  }

  auto& fowTile = FowLayer[obj->PosX][obj->PosY];

  fowTile.Image   = obj->Image;
  fowTile.FowName = Util::GetFowName(obj);
  fowTile.Graphic = obj->Graphic;
}

// =============================================================================

GameObject* MapLevelBase::GetTopmostObject(const Position& pos)
{
  if (pos.X < 0 || pos.X >= MapSize.X || pos.Y < 0 || pos.Y >= MapSize.Y)
  {
    return nullptr;
  }

  //
  // There shouldn't be more than 1 actor per tile.
  //
  for (auto& i : ActorGameObjects)
  {
    if (i->PosX == pos.X && i->PosY == pos.Y)
    {
      return i.get();
    }
  }

  //
  // To get topmost object, we need to get last item for this tile.
  //
  std::stack<GameObject*> itemsHere;
  for (auto& i : GameObjects)
  {
    if (i->PosX == pos.X && i->PosY == pos.Y)
    {
      itemsHere.push(i.get());
    }
  }

  if (!itemsHere.empty())
  {
    return itemsHere.top();
  }

  //
  // Static object is also only 1 per tile.
  //
  if (StaticMapObjects[pos.X][pos.Y] != nullptr)
  {
    return StaticMapObjects[pos.X][pos.Y].get();
  }

  //
  // Fallback to map tile otherwise.
  //
  return MapArray[pos.X][pos.Y].get();
}

// =============================================================================

void MapLevelBase::PlaceGroundTile(int x, int y,
                                   int image,
                                   const uint32_t& fgColor,
                                   const uint32_t& bgColor,
                                   const std::string& objName,
                                   GraphicTiles graphicTile)
{
  if (IsOutOfBounds(x, y, false))
  {
    return;
  }

  GameObjectInfo t;
  t.Set(false, false, image, fgColor, bgColor, objName, Strings::Empty);
  t.SetGraphics(graphicTile);

  MapArray[x][y]->MakeTile(t);
}

// =============================================================================

//
// Places grass tile at [x; y], maxDiceRoll serves as a "frequency" modifier:
// the more its value, the less is the chance for flowers to appear.
//
void MapLevelBase::PlaceGrassTile(int x, int y, int maxDiceRoll)
{
  if (IsOutOfBounds(x, y, false))
  {
    return;
  }

  char img = '.';

  //
  // Create 'flowers'
  //

  uint32_t flowerColor = Colors::GrassDot;
  std::string tileName = Strings::TileNames::Grass;
  GraphicTiles flowerTile = GraphicTiles::GRASS2;

  //
  // Skip flower generation if frequency is -1, for example.
  //
  if (maxDiceRoll >= 0)
  {
    int colorChoice = Game::gRng.RandomRange(0, maxDiceRoll);
    if      (colorChoice == 0) flowerColor = Colors::White;
    else if (colorChoice == 1) flowerColor = Colors::DandelionYellow;
    else if (colorChoice == 2) flowerColor = Colors::RedPoppy;

    std::map<int, std::string> flowersNameByChoice =
    {
      { 0, "Chamomile" },
      { 1, "Dandelion" },
      { 2, "Poppy"     }
    };

    switch (colorChoice)
    {
      case 0:  flowerTile = GraphicTiles::CHAMOMILE_GRASS; break;
      case 1:  flowerTile = GraphicTiles::DANDELION_GRASS; break;
      case 2:  flowerTile = GraphicTiles::POPPY_GRASS;     break;
      default:
        break;
    }

    if (flowersNameByChoice.count(colorChoice) == 1)
    {
      tileName = flowersNameByChoice[colorChoice];
    }
  }

  GameObjectInfo t;
  t.Set(false, false, img, flowerColor, Colors::Grass, tileName, Strings::Empty);
  t.SetGraphics(flowerTile);

  MapArray[x][y]->MakeTile(t);
}

// =============================================================================

void MapLevelBase::PlaceShallowWaterTile(int x, int y)
{
  if (IsOutOfBounds(x, y, false))
  {
    return;
  }

  GameObjectInfo t;
  t.Set(false,
        false,
        '~',
        Colors::White,
        Colors::ShallowWater,
        Strings::TileNames::ShallowWater,
        Strings::Empty);
  t.SetGraphics(GraphicTiles::WATER_SHALLOW_HC);

  MapArray[x][y]->MakeTile(t, GameObjectType::SHALLOW_WATER);
}

// =============================================================================

void MapLevelBase::PlaceDeepWaterTile(int x, int y)
{
  if (IsOutOfBounds(x, y, false))
  {
    return;
  }

  //
  // int type is to avoid truncation in case of CP437 image which is 247
  //
  int img = '~';

  #ifdef USE_SDL
  img = GlobalConstants::CP437IndexByType[NameCP437::WAVES];
  #endif

  GameObjectInfo t;
  t.Set(false,
        false,
        img,
        Colors::White,
        Colors::DeepWater,
        Strings::TileNames::DeepWater,
        Strings::Empty);
  t.SetGraphics(GraphicTiles::WATER_DEEP_HC);

  MapArray[x][y]->MakeTile(t, GameObjectType::DEEP_WATER);
}

// =============================================================================

void MapLevelBase::PlaceLavaTile(int x, int y)
{
  if (IsOutOfBounds(x, y, false))
  {
    return;
  }

  GameObjectInfo t;
  t.Set(false,
        false,
        '~',
        Colors::LavaWaves,
        Colors::Lava,
        Strings::TileNames::Lava,
        Strings::Empty);
  t.SetGraphics(GraphicTiles::LAVA);

  MapArray[x][y]->MakeTile(t, GameObjectType::LAVA);
}

// =============================================================================

void MapLevelBase::PlaceChasmTile(int x, int y)
{
  if (IsOutOfBounds(x, y, false))
  {
    return;
  }

  int img = ' ';

#ifdef USE_SDL
  img = GlobalConstants::CP437IndexByType[NameCP437::SHADING_3];

  uint32_t fgColor = Colors::ShadesOfGrey::Three;
  uint32_t bgColor = Colors::Black;
#else
  uint32_t fgColor = Colors::Black;
  uint32_t bgColor = Colors::Black;
#endif

  GameObjectInfo t;
  t.Set(false,
        false,
        img,
        fgColor,
        bgColor,
        Strings::TileNames::Chasm,
        Strings::Empty);

  MapArray[x][y]->MakeTile(t, GameObjectType::CHASM);
}

// =============================================================================

void MapLevelBase::PlaceShrine(const Position& pos, LevelBuilder& lb)
{
  if (IsOutOfBounds(pos.X, pos.Y))
  {
    return;
  }

  ShrineType type = lb.ShrinesByPosition().at(pos);
  PlaceShrine(pos, type);
}

// =============================================================================

void MapLevelBase::PlaceShrine(const Position& pos, ShrineType type)
{
  if (IsOutOfBounds(pos.X, pos.Y))
  {
    return;
  }

  //
  // Shrine is placed as GameObject so that it is updated every turn.
  // To draw it under FoW we create static "dummy" object on top of it.
  //
  GameObjectInfo t;
  auto go = Game::gGOF.CreateShrine(pos.X, pos.Y, type, 1000);
  PlaceGameObject(go);

  std::string description = GlobalConstants::ShrineNameByType.at(type);
  t.Set(true,
        false,
        '/',
        Colors::ShadesOfGrey::Four,
        Colors::Black,
        description,
        "?Shrine?");

  PlaceStaticObject(pos.X, pos.Y, t);
}

// =============================================================================

void MapLevelBase::PlaceTree(int x, int y)
{
  if (IsOutOfBounds(x, y, false))
  {
    return;
  }

  char img = 'T';

  #ifdef USE_SDL
  img = GlobalConstants::CP437IndexByType[NameCP437::CLUB];
  #endif

  GameObjectInfo t;
  t.Set(true,
        true,
        img,
        Colors::Green,
        Colors::Black,
        Strings::TileNames::Tree,
        Strings::Empty);

  PlaceStaticObject(x, y, t);
}

// =============================================================================

void MapLevelBase::PlaceWall(int x, int y,
                             int image,
                             const uint32_t& fgColor,
                             const uint32_t& bgColor,
                             const std::string& objName,
                             bool cannotBePickaxed,
                             GraphicTiles graphicTile)
{
  if (IsOutOfBounds(x, y))
  {
    return;
  }

  GameObjectInfo t;
  t.Set(true,
        true,
        image,
        fgColor,
        bgColor,
        objName,
        Strings::Empty);
  t.SetGraphics(graphicTile);

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
                             const std::string& objName,
                             DoorMaterials doorMaterial,
                             DoorGraphicType doorType)
{
  if (IsOutOfBounds(x, y))
  {
    return;
  }

  GameObject* door =
      Game::gGOF.CreateDoor(x, y, isOpen, doorMaterial, objName);

  DoorComponent* dc = door->GetComponent<DoorComponent>();

  dc->DoorType_ = doorType;
  dc->OpenedBy  = openedBy;

  bool openedByAnyone = (openedBy == GlobalConstants::OpenedByAnyone);

  if (dc->DoorType_ != DoorGraphicType::UNDEFINED)
  {
    door->Graphic.Tile = openedByAnyone ?
                         GlobalConstants::DoorGraphicsByType.at(doorType)[1] :
                         GlobalConstants::DoorGraphicsByType.at(doorType)[2];
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
      // This ensures that all common objects will share the same visual style
      // that is defined for the current map.
      //
      CreateCommonObjects(x, y, lb.MapRaw);

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
                                const std::string& tileName,
                                const GraphicTiles graphicTile)
{
  GameObjectInfo t;
  t.Set(false,
        false,
        img,
        fgColor,
        bgColor,
        tileName,
        Strings::Empty);
  t.SetGraphics(graphicTile);

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

void MapLevelBase::CreateSpecialObjects(int x,
                                        int y,
                                        const DGBase::MapCell& cell)
{
  MapArray[x][y]->ZoneMarker = cell.ZoneMarker;

  switch (cell.ZoneMarker)
  {
    // -------------------------------------------------------------------------

    case TransformedRoom::SHRINE:
    {
      if (std::holds_alternative<ShrineType>(cell.ObjectHere))
      {
        ShrineType t = std::get<ShrineType>(cell.ObjectHere);
        PlaceShrine({ x, y }, t);
      }
    }
    break;

    // -------------------------------------------------------------------------

    case TransformedRoom::STORAGE:
    {
      if (std::holds_alternative<GameObjectType>(cell.ObjectHere))
      {
        GameObjectType t = std::get<GameObjectType>(cell.ObjectHere);
        if (t == GameObjectType::BREAKABLE)
        {
          static GameObjectsFactory& gof = Game::gGOF;
          GameObject* box = gof.CreateBreakableObjectWithRandomLoot(
            x,
            y,
            'B',
            "Wooden Box",
            Colors::Wood,
            Colors::Black,
            GraphicTiles::CRATE_WOODEN2
          );

          PlaceStaticObject(box);
        }
      }
    }
    break;

    // -------------------------------------------------------------------------

    case TransformedRoom::TREASURY:
    {
      if (std::holds_alternative<ItemType>(cell.ObjectHere))
      {
        if(std::get<ItemType>(cell.ObjectHere) == ItemType::COINS)
        {
          GameObject* go = Game::gIF.CreateMoney();
          go->PosX = x;
          go->PosY = y;
          PlaceGameObject(go);
        }
      }
    }
    break;

    // -------------------------------------------------------------------------

    case TransformedRoom::CHESTROOM:
    {
      if (std::holds_alternative<GameObjectType>(cell.ObjectHere))
      {
        GameObjectType t = std::get<GameObjectType>(cell.ObjectHere);
        if(t == GameObjectType::CONTAINER)
        {
          static GameObjectsFactory& gof = Game::gGOF;
          GameObject* go = gof.CreateChest(x, y, Util::Rolld100(50));
          go->PosX = x;
          go->PosY = y;
          PlaceGameObject(go);
        }
      }
    }
    break;
  }
}

// =============================================================================

void MapLevelBase::Paint(int x,
                         int y,
                         GraphicTiles tile,
                         uint8_t flipMask,
                         uint16_t rotation)
{
  if (!Util::IsInsideMap({ x, y }, MapSize))
  {
    return;
  }

  MapArray[x][y]->Graphic.Tile            = tile;
  MapArray[x][y]->Graphic.FlipMask        = flipMask;
  MapArray[x][y]->Graphic.RotationDegrees = rotation;
}

// =============================================================================

void MapLevelBase::PaintBigStar(int x, int y)
{
  if (!Util::IsInsideMap({ x, y }, MapSize))
  {
    return;
  }

  Paint(x,
        y,
        GraphicTiles::TILE_STAR_BIG_CORNER,
        GlobalConstants::FlipMaskV);

  Paint(x + 1,
        y,
        GraphicTiles::TILE_STAR_BIG_MIDDLE,
        GlobalConstants::FlipMaskV);

  Paint(x + 2,
        y,
        GraphicTiles::TILE_STAR_BIG_CORNER,
        (GlobalConstants::FlipMaskH | GlobalConstants::FlipMaskV));

  Paint(x,
        y + 1,
        GraphicTiles::TILE_STAR_BIG_MIDDLE,
        GlobalConstants::FlipMaskNone,
        90);

  Paint(x + 1, y + 1, GraphicTiles::TILE_STAR_BIG_CENTER);

  Paint(x + 2,
        y + 1,
        GraphicTiles::TILE_STAR_BIG_MIDDLE,
        GlobalConstants::FlipMaskNone,
        270);

  Paint(x,     y + 2, GraphicTiles::TILE_STAR_BIG_CORNER);
  Paint(x + 1, y + 2, GraphicTiles::TILE_STAR_BIG_MIDDLE);
  Paint(x + 2, y + 2, GraphicTiles::TILE_STAR_BIG_CORNER,
        GlobalConstants::FlipMaskH);
}

// =============================================================================

void MapLevelBase::PaintTileBorders(const Position& from,
                                    const Position& to,
                                    GraphicTiles corner,
                                    GraphicTiles line,
                                    GraphicTiles filler)
{
  int lx = from.X;
  int ly = from.Y;
  int hx = to.X;
  int hy = to.Y;

  auto ValidateCoords = [this](int& x, int& y)
  {
    if (x < 0)               x = 0;
    if (x > (MapSize.X - 1)) x = MapSize.X - 1;
    if (y < 0)               y = 0;
    if (y > (MapSize.Y - 1)) y = MapSize.Y - 1;
  };

  ValidateCoords(lx, ly);
  ValidateCoords(hx, hy);

  if (lx > hx) std::swap(lx, hx);
  if (ly > hy) std::swap(ly, hy);

  auto perimeter = Util::GetPerimeter(lx, ly, hx, hy);
  for (const Position& p : perimeter)
  {
    bool cornerUL    = (p.X == lx && p.Y == ly);
    bool cornerUR    = (p.X == hx && p.Y == ly);
    bool cornerDR    = (p.X == hx && p.Y == hy);
    bool cornerDL    = (p.X == lx && p.Y == hy);
    bool upperStripH = (p.X > lx && p.X < hx) && (p.Y == ly);
    bool lowerStripH = (p.X > lx && p.X < hx) && (p.Y == hy);
    bool leftStripV  = (p.X == lx) && (p.Y > ly && p.Y < hy);
    bool rightStripV = (p.X == hx) && (p.Y > ly && p.Y < hy);

    if (cornerUL)
    {
      Paint(p.X, p.Y, corner);
    }
    else if (cornerUR)
    {
      Paint(p.X, p.Y, corner, GlobalConstants::FlipMaskH);
    }
    else if (cornerDR)
    {
      Paint(p.X,
            p.Y,
            corner,
            (GlobalConstants::FlipMaskH | GlobalConstants::FlipMaskV));
    }
    else if (cornerDL)
    {
      Paint(p.X, p.Y, corner, GlobalConstants::FlipMaskV);
    }
    else if (upperStripH || lowerStripH)
    {
      Paint(p.X, p.Y, line);
    }
    else if (leftStripV || rightStripV)
    {
      Paint(p.X, p.Y, line, GlobalConstants::FlipMaskNone, 90);
    }
  }

  if (filler != GraphicTiles::NONE)
  {
    for (int x = lx + 1; x <= hx - 1; x++)
    {
      for (int y = ly + 1; y <= hy - 1; y++)
      {
        Paint(x, y, filler);
      }
    }
  }
}

// =============================================================================

void MapLevelBase::PlaceWoodenFence(const Position& from, const Position& to)
{
  int lx = from.X;
  int ly = from.Y;
  int hx = to.X;
  int hy = to.Y;

  auto ValidateCoords = [this](int& x, int& y)
  {
    if (x < 0)               x = 0;
    if (x > (MapSize.X - 1)) x = MapSize.X - 1;
    if (y < 0)               y = 0;
    if (y > (MapSize.Y - 1)) y = MapSize.Y - 1;
  };

  ValidateCoords(lx, ly);
  ValidateCoords(hx, hy);

  if (lx > hx) std::swap(lx, hx);
  if (ly > hy) std::swap(ly, hy);

  GameObjectInfo goi;
  goi.Set(true,
          false,
          '#',
          Colors::Wood,
          Colors::None,
          Strings::TileNames::WoodenFence,
          Strings::Empty);

  goi.SetGraphics(GraphicTiles::FENCE_WOODEN_CORNER_UP);
  PlaceStaticObject(lx, ly, goi);

  goi.SetGraphics(GraphicTiles::FENCE_WOODEN_CORNER_DOWN);
  PlaceStaticObject(lx, hy, goi);

  goi.SetGraphics(GraphicTiles::FENCE_WOODEN_CORNER_DOWN,
                  Colors::White,
                  1.0,
                  0,
                  GlobalConstants::FlipMaskH);
  PlaceStaticObject(hx, hy, goi);

  goi.SetGraphics(GraphicTiles::FENCE_WOODEN_CORNER_UP,
                  Colors::White,
                  1.0,
                  0,
                  GlobalConstants::FlipMaskH);
  PlaceStaticObject(hx, ly, goi);

  goi.SetGraphics(GraphicTiles::FENCE_WOODEN);
  for (int x = lx + 1; x <= hx - 1; x++)
  {
    PlaceStaticObject(x, ly, goi);
    PlaceStaticObject(x, hy, goi);
  }

  for (int y = ly + 1; y <= hy - 1; y++)
  {
    goi.SetGraphics(GraphicTiles::FENCE_WOODEN_V);
    PlaceStaticObject(lx, y, goi);

    goi.SetGraphics(GraphicTiles::FENCE_WOODEN_V,
                    Colors::White,
                    1.0,
                    0,
                    GlobalConstants::FlipMaskH);
    PlaceStaticObject(hx, y, goi);
  }
}

// =============================================================================

bool MapLevelBase::IsCellFacingFront(int x, int y, int image)
{
  if (!Util::IsInsideMap({ x, y }, MapSize))
  {
    return false;
  }

  if (!Util::IsInsideMap({ x, y + 1 }, MapSize))
  {
    return false;
  }

  return StaticMapObjects[x][y + 1] == nullptr 
      || (StaticMapObjects[x][y + 1] != nullptr 
       && StaticMapObjects[x][y + 1]->Image != image);
}
