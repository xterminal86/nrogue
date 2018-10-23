#ifndef MAP_H
#define MAP_H

#include <algorithm>
#include <random>
#include <chrono>

#include "singleton.h"
#include "rect.h"
#include "constants.h"
#include "game-object.h"
#include "player.h"

enum class MapType
{
  TOWN = 0,
  MINES
};

class Map : public Singleton<Map>
{
  public:
    void Init() override;

    void CreateMap(MapType mapType);

    void Draw(int playerX, int playerY);

    void InsertActor(GameObject* actor);
    void InsertGameObject(GameObject* goToInsert);
    void UpdateGameObjects();

    GameObject* GetActorAtPosition(int x, int y);
    std::vector<GameObject*> GetGameObjectsAtPosition(int x, int y);
    std::pair<int, GameObject*> GetTopGameObjectAtPosition(int x, int y);

    // Map tiles or objects that don't have to be updated globally.
    // Updated around player position.
    GameObject MapArray[GlobalConstants::MapX][GlobalConstants::MapY];

    // Globally updated objects (traps with timers, shrines, etc.)
    // or objects that can be removed from the map (e.g. items)
    // Updated every frame.
    std::vector<std::unique_ptr<GameObject>> GameObjects;

    // NPCs, drawn last
    std::vector<std::unique_ptr<GameObject>> ActorGameObjects;

    int PlayerStartX;
    int PlayerStartY;

    int MapOffsetX;
    int MapOffsetY;

  private:
    Player* _playerRef;

    void SetPlayerStartingPosition(int x, int y);
    void CreateTown();

    [[deprecated("Old method, creates area with ground tiles and walls")]]
    void CreateRoom(int x, int y, int w, int h);

    /// Creates room starting from (x;y) increasing to the down right
    /// using \p layout provided
    void CreateRoom(int x, int y, const std::vector<std::string>& layout, bool randomizeOrientation = false);

    void CreateChurch(int x, int y);
    void CreateCastle(int x, int y);
    void FillArea(int x, int y, int w, int h, const Tile& tileToFill);
    void CreateDoor(int x, int y, bool isOpen = false);

    void DrawGameObjects();
    void RemoveDestroyed();
};

#endif
