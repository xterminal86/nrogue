#ifndef MAPLEVELBASE_H
#define MAPLEVELBASE_H

#include "util.h"

// Forward declaration due to cyclic dependency

class Player;
class GameObject;

class MapLevelBase
{
  public:
    MapLevelBase(int sizeX, int sizeY, MapType type);
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

    int PlayerStartX;
    int PlayerStartY;

    int MapOffsetX;
    int MapOffsetY;

  protected:
    Player* _playerRef;

    void SetPlayerStartingPosition(int x, int y);
};

#endif // MAPLEVEL_H
