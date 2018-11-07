#ifndef MAPLEVELBASE_H
#define MAPLEVELBASE_H

#include "util.h"
#include "game-object.h"

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

    int MaxMonsters;
    int MonstersRespawnRate;
    int DungeonLevel;
    int VisibilityRadius;

    void AdjustCamera();

  protected:
    std::vector<Position> _emptyCells;

    Player* _playerRef;

    virtual void CreateLevel() {}

    void RecordEmptyCells();
};

#endif // MAPLEVEL_H
