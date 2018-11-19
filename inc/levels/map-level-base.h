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

    virtual void PrepareMap(MapLevelBase* levelOwner);
    virtual void DisplayWelcomeText();

    virtual void TryToSpawnMonsters() {}

    // Map tiles or objects that don't have to be updated globally.
    // Updated around player position.
    std::vector<std::vector<std::unique_ptr<GameObject>>> MapArray;

    // Globally updated objects (traps with timers, shrines, etc.)
    // or objects that can be removed from the map (e.g. items)
    // Updated every frame.
    std::vector<std::unique_ptr<GameObject>> GameObjects;

    // NPCs, drawn last
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

    void AdjustCamera();    

    int RespawnCounter()
    {
      return _respawnCounter;
    }

  protected:
    std::vector<Position> _emptyCells;
    std::vector<std::vector<std::string>> _layoutsForLevel;
    std::vector<RoomForLevel> _roomsForLevel;

    Player* _playerRef;

    int _respawnCounter = 0;

    virtual void CreateLevel() {}

    void RecordEmptyCells();
};

#endif // MAPLEVEL_H
