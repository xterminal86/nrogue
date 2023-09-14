#ifndef MAPLEVELBASE_H
#define MAPLEVELBASE_H

#include "util.h"
#include "game-object.h"
#include "level-builder.h"

class Player;

class MapLevelBase
{
  public:
    MapLevelBase(int sizeX, int sizeY, MapType type, int dungeonLevel);
    MapLevelBase(const MapLevelBase&) = delete;
    virtual ~MapLevelBase();

    void PlaceActor(GameObject* actor);
    void PlaceGameObject(GameObject* goToInsert);
    void PlaceStaticObject(GameObject* goToInsert);
    void PlaceStaticObject(int x, int y,
                           const GameObjectInfo& objectInfo,
                           int hitPoints = -1,
                           GameObjectType type = GameObjectType::HARMLESS);
    void PlaceTrigger(GameObject* trigger, TriggerUpdateType updateType);
    void TryToSpawnMonsters();

    virtual void PrepareMap(MapLevelBase* levelOwner);
    virtual void DisplayWelcomeText();
    virtual void OnLevelChanged(MapType from);

    //
    // Map ground tiles (floor, water, ground etc.).
    // Drawn under fog of war.
    //
    std::vector<std::vector<std::unique_ptr<GameObject>>> MapArray;

    //
    // Static map objects without global update (walls, doors etc.)
    // Drawn under fog of war.
    //
    std::vector<std::vector<std::unique_ptr<GameObject>>> StaticMapObjects;

    //
    // Globally updated objects (traps with timers, shrines, etc.)
    // or objects that can be picked up (e.g. items).
    // Updated every frame. Aren't drawn under fog of war.
    //
    std::vector<std::unique_ptr<GameObject>> GameObjects;

    //
    // NPCs, drawn last. Aren't drawn under fog of war.
    //
    std::vector<std::unique_ptr<GameObject>> ActorGameObjects;

    //
    // In-game events that are checked on GameObject::FinishTurn()
    //
    std::vector<std::unique_ptr<GameObject>> FinishTurnTriggers;

    //
    // In-game events that are checked on every Map::Update()
    //
    std::vector<std::unique_ptr<GameObject>> GlobalTriggers;

    MapType MapType_;
    Position MapSize;
    std::string LevelName;

    Position LevelStart;
    Position LevelExit;

    int MapOffsetX;
    int MapOffsetY;

    size_t MaxMonsters = 0;

    //
    // This depends heavily on player's speed,
    // values can be big since several turns
    // can pass before player turn is ready.
    //
    int MonstersRespawnTurns = -1;

    int DungeonLevel;
    int VisibilityRadius;

    bool WelcomeTextDisplayed   = false;
    bool Peaceful               = false;
    bool ExitFound              = false;
    bool MysteriousForcePresent = false;

    void AdjustCamera();

    const int& RespawnCounter();
    const std::vector<Position>& EmptyCells();

    bool IsCellBlocking(const Position& pos);

#ifdef DEBUG_BUILD
    GameObject* FindObjectByAddress(const std::string& addressString);
#endif

  protected:
    std::vector<Position> _emptyCells;
    std::vector<StringV> _layoutsForLevel;
    std::unordered_map<GameObjectType, int> _monstersSpawnRateForThisLevel;

    StringV _specialLevel;

    Player* _playerRef = nullptr;

    int _respawnCounter = 0;

    const int _shrineRollChance = 50;

    int GetEstimatedNumberOfItemsToCreate();

    void ConstructFromBuilder(LevelBuilder& lb);

    void CreateGround(char img,
                      uint32_t fgColor,
                      uint32_t bgColor,
                      const std::string& tileName);

    virtual void CreateLevel();
    virtual void CreateSpecialLevel();
    virtual void CreateSpecialMonsters();

    //
    // To process tiles based on their visual representation.
    // Used to instantiate common content: walls, doors, etc.
    //
    virtual void CreateCommonObjects(int x, int y, char image) = 0;

    //
    // To instantiate some additional content on this tile
    // based on metadata in MapCell. Basically used to process zones
    // and create some extra special objects there.
    //
    void CreateSpecialObjects(int x, int y, const MapCell& cell);

    bool IsSpotValidForSpawn(const Position& pos);
    bool IsOutOfBounds(int x, int y);

    void FillArea(int ax, int ay,
                  int aw, int ah,
                  const GameObjectInfo& tileToFill);

    void CreateBorders(GameObjectInfo& t);
    void RecordEmptyCells();
    void CreateItemsForLevel(int maxItems);
    void PlaceRandomShrine(LevelBuilder& lb);
    void PlaceStairs();
    void CreateInitialMonsters();

    void PlaceGroundTile(int x, int y,
                         int image,
                         const uint32_t& fgColor,
                         const uint32_t& bgColor,
                         const std::string& objName);

    void PlaceGrassTile(int x, int y, int maxDiceRoll = 10);
    void PlaceShallowWaterTile(int x, int y);
    void PlaceDeepWaterTile(int x, int y);
    void PlaceLavaTile(int x, int y);
    void PlaceChasmTile(int x, int y);
    void PlaceTree(int x, int y);

    void PlaceWall(int x, int y,
                   int image,
                   const uint32_t& fgColor,
                   const uint32_t& bgColor,
                   const std::string& objName,
                   bool cannotBePickaxed = false);

    void PlaceShrine(const Position& pos, LevelBuilder& lb);
    void PlaceShrine(const Position& pos, ShrineType type);

    void PlaceDoor(int x, int y,
                   bool isOpen = false,
                   size_t openedBy = GlobalConstants::OpenedByAnyone,
                   const std::string& objName = std::string());

#ifdef DEBUG_BUILD
    template <typename Collection>
    GameObject* FindInVV(const Collection& c,
                         const std::string& addrStr)
    {
      for (auto& line : c)
      {
        for (auto& o : line)
        {
          if (o.get() != nullptr && o.get()->HexAddressString == addrStr)
          {
            return o.get();
          }
        }
      }

      return nullptr;
    }

    template <typename Collection>
    GameObject* FindInV(const Collection& c,
                        const std::string& addrStr)
    {
      for (auto& o : c)
      {
        if (o.get() != nullptr && o.get()->HexAddressString == addrStr)
        {
          return o.get();
        }
      }

      return nullptr;
    }
#endif

#ifdef DEBUG_BUILD
    friend class DevConsole;
#endif
};

#endif // MAPLEVEL_H
