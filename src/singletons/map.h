#ifndef MAP_H
#define MAP_H

#include <algorithm>
#include <random>
#include <chrono>
#include <memory>

#include "singleton.h"
#include "constants.h"
#include "map-level-base.h"

class GameObject;

class Map : public Singleton<Map>
{
  public:
    void Init() override;

    void Draw(int playerX, int playerY);

    void InsertActor(GameObject* actor);
    void InsertGameObject(GameObject* goToInsert);
    void RemoveDestroyed();
    void UpdateGameObjects();

    void ChangeLevel(MapType levelToChange, bool goingDown);
    void TeleportToExistingLevel(MapType levelToChange, const Position& teleportTo, GameObject* objectToTeleport = nullptr);
    void PrintMapArrayRevealedStatus();
    void PrintMapLayout();

    bool IsObjectVisible(const Position& from,
                         const Position& to);

    GameObject* GetActorAtPosition(int x, int y);
    std::vector<GameObject*> GetGameObjectsAtPosition(int x, int y);
    std::vector<GameObject*> GetActorsInRange(int range);
    std::pair<int, GameObject*> GetGameObjectToPickup(int x, int y);
    MapLevelBase* GetLevelRefByType(MapType type);

    std::vector<Position> GetWalkableCellsAround(const Position& pos);
    std::vector<MapType> GetAllVisitedLevels();
    Position GetRandomEmptyCell();

    MapLevelBase* CurrentLevel;

  private:
    std::map<MapType, std::unique_ptr<MapLevelBase>> _levels;
    std::map<MapType, bool> _mapVisitFirstTime;

    void ChangeOrInstantiateLevel(MapType levelName);
    void ShowLoadingText();
    void DrawNonVisibleMapTile(int x, int y);
    void DrawNonVisibleStaticObject(int x, int y);

    Player* _playerRef;
};

#endif
