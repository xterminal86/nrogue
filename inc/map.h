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

    void InsertGameObject(GameObject* goToInsert);
    void UpdateGameObjects();

    std::vector<GameObject*> GetGameObjectsAtPosition(int x, int y);
    std::pair<int, GameObject*> GetTopGameObjectAtPosition(int x, int y);

    // Map tiles
    GameObject MapArray[GlobalConstants::MapX][GlobalConstants::MapY];

    // Items, npcs etc.
    std::vector<std::unique_ptr<GameObject>> GameObjects;

    int PlayerStartX;
    int PlayerStartY;

    int MapOffsetX;
    int MapOffsetY;

  private:
    Player* _playerRef;

    void CreateTown();
    void CreateRoom(int x, int y, int w, int h);
    void FillArea(int x, int y, int w, int h, const Tile& tileToFill);
    void CreateDoor(int x, int y, bool isOpen = false);

    void DrawGameObjects();    
};

#endif
