#ifndef MAP_H
#define MAP_H

#include <algorithm>
#include <random>
#include <chrono>
#include <memory>

#include "singleton.h"
#include "constants.h"
#include "position.h"
#include "map-level-base.h"

enum class GameObjectCollectionType
{
  STATIC_OBJECTS = 0,
  ITEM_OBJECTS,
  ACTORS,
  ALL
};

class GameObject;

class Map : public Singleton<Map>
{
  public:
    void Cleanup();

    void Draw();

    void InsertActor(GameObject* actor);
    void InsertGameObject(GameObject* goToInsert);
    void RemoveDestroyed(GameObjectCollectionType c = GameObjectCollectionType::ALL);
    void UpdateGameObjects();

    void ChangeLevel(MapType levelToChange, bool goingDown);
    void TeleportToExistingLevel(MapType levelToChange, const Position& teleportTo, GameObject* objectToTeleport = nullptr);
    void PrintMapArrayRevealedStatus();

    #ifdef DEBUG_BUILD
    void PrintMapLayout();
    #endif

    void ProcessAoEDamage(GameObject* target, ItemComponent* weapon, int centralDamage, bool againstRes);

    bool IsObjectVisible(const Position& from,
                         const Position& to);

    bool TryToDamageObject(GameObject* object,
                           GameObject* from,
                           int amount,
                           bool againstRes);

    bool IsTileDangerous(const Position& pos);

    GameObject* GetActorAtPosition(int x, int y);
    GameObject* GetStaticGameObjectAtPosition(int x, int y);

    std::vector<GameObject*> GetGameObjectsAtPosition(int x, int y);
    std::vector<GameObject*> GetActorsInRange(int range);

    std::pair<int, GameObject*> GetGameObjectToPickup(int x, int y);
    MapLevelBase* GetLevelRefByType(MapType type);

    std::vector<Position> GetWalkableCellsAround(const Position& pos);
    std::vector<Position> GetEmptyCellsAround(const Position& pos, int range);
    std::vector<MapType> GetAllVisitedLevels();
    Position GetRandomEmptyCell();

    int CountEmptyCellsAround(int x, int y);
    int CountAroundStatic(int x, int y, GameObjectType type);
    int CountWallsOrthogonal(int x, int y);

    MapLevelBase* CurrentLevel;

  protected:
    void InitSpecific() override;

  private:
    std::map<MapType, std::unique_ptr<MapLevelBase>> _levels;
    std::map<MapType, bool> _mapVisitFirstTime;

    void ChangeOrInstantiateLevel(MapType levelName);
    void ShowLoadingText(const std::string& textOverride = std::string());
    void DrawNonVisibleMapTile(int x, int y);
    void DrawNonVisibleStaticObject(int x, int y);
    void DrawMapTilesAroundPlayer();
    void DrawGameObjects();
    void DrawActors();

    void RemoveStaticObjects();
    void EraseFromCollection(std::vector<std::unique_ptr<GameObject>>& list);

    Player* _playerRef;

    Position _windowSize;

    friend class IntroState;
};

#endif
