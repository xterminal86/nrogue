#ifndef MAPLEVELBASE_H
#define MAPLEVELBASE_H

#include "game-object.h"
#include "util.h"

class MapLevelBase
{
  public:
    MapLevelBase(int sizeX, int sizeY, MapType type);
    MapLevelBase(const MapLevelBase&) = delete;
    virtual ~MapLevelBase() {}

    void InsertActor(GameObject* actor)
    {
      ActorGameObjects.push_back(std::unique_ptr<GameObject>(actor));
    }

    void InsertGameObject(GameObject* goToInsert)
    {
      GameObjects.push_back(std::unique_ptr<GameObject>(goToInsert));
    }

    // Map tiles or objects that don't have to be updated globally.
    // Updated around player position.
    GameObject MapArray[GlobalConstants::MapX][GlobalConstants::MapY];

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
    void SetPlayerStartingPosition(int x, int y);
};

#endif // MAPLEVEL_H
