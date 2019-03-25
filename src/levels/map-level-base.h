#ifndef MAPLEVELBASE_H
#define MAPLEVELBASE_H

#include "util.h"
#include "game-object.h"
#include "room-helper.h"

class Player;

class MapLevelBase
{
  public:
    MapLevelBase(int sizeX, int sizeY, MapType type, int dungeonLevel);
    MapLevelBase(const MapLevelBase&) = delete;
    virtual ~MapLevelBase() {}

    void InsertActor(GameObject* actor);
    void InsertGameObject(GameObject* goToInsert);
    void InsertStaticObject(GameObject* goToInsert);
    void InsertStaticObject(int x, int y, const GameObjectInfo& objectInfo, int hitPoints = -1, GameObjectType type = GameObjectType::HARMLESS);
    void TryToSpawnMonsters();

    virtual void PrepareMap(MapLevelBase* levelOwner);
    virtual void DisplayWelcomeText();

    // Map ground tiles (floor, water, ground etc.).
    // Drawn under fog of war.
    std::vector<std::vector<std::unique_ptr<GameObject>>> MapArray;

    // Static map objects without global update (walls, doors etc.)
    // Drawn under fog of war.
    std::vector<std::vector<std::unique_ptr<GameObject>>> StaticMapObjects;

    // Globally updated objects (traps with timers, shrines, etc.)
    // or objects that can be picked up (e.g. items).
    // Updated every frame. Aren't drawn under fog of war.
    std::vector<std::unique_ptr<GameObject>> GameObjects;

    // NPCs, drawn last. Aren't drawn under fog of war.
    std::vector<std::unique_ptr<GameObject>> ActorGameObjects;

    MapType MapType_;
    Position MapSize;
    std::string LevelName;

    Position LevelStart;
    Position LevelExit;

    int MapOffsetX;
    int MapOffsetY;

    int MaxMonsters = 0;

    // This depends heavily on player's speed,
    // values can be big since several turns
    // can pass before player turn is ready.
    int MonstersRespawnTurns = -1;

    int DungeonLevel;
    int VisibilityRadius;

    bool IsInitialized = false;
    bool WelcomeTextDisplayed = false;
    bool Peaceful = false;
    bool ExitFound = false;

    void AdjustCamera();    

    int RespawnCounter()
    {
      return _respawnCounter;
    }

    bool IsCellBlocking(const Position& pos);

  protected:
    std::vector<Position> _emptyCells;
    std::vector<std::vector<std::string>> _layoutsForLevel;
    std::vector<RoomForLevel> _roomsForLevel;
    std::map<GameObjectType, int> _monstersSpawnRateForThisLevel;

    Player* _playerRef;

    int _respawnCounter = 0;

    virtual void CreateLevel() {}

    bool IsSpotValidForSpawn(const Position& pos);

    void CreateBorders(GameObjectInfo& t);
    void RecordEmptyCells();
    void CreateItemsForLevel(int maxItems);
    void PlaceStairs();
    void CreateInitialMonsters();
};

#endif // MAPLEVEL_H
