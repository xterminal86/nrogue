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
  GAME_OBJECTS,
  ACTORS,
  MAP_ARRAY,
  TRIGGERS,
  ALL
};

class GameObject;

class Map : public Singleton<Map>
{
  public:
    void Cleanup();

    void Draw();

    void LoadTown();

    #ifdef BUILD_TESTS
    void LoadTestLevel();
    #endif

    void PlaceActor(GameObject* actor);
    void PlaceGameObject(GameObject* goToInsert);
    void RemoveDestroyed(GameObjectCollectionType c = GameObjectCollectionType::ALL);
    void Update();
    void UpdateTriggers(TriggerUpdateType updateType);

    void ChangeLevel(MapType levelToChange, bool goingDown);
    void TeleportToExistingLevel(MapType levelToChange,
                                 const Position& teleportTo,
                                 GameObject* objectToTeleport = nullptr);
    void PrintMapArrayRevealedStatus();

    #ifdef DEBUG_BUILD
    void PrintMapLayout();
    #endif

    void ProcessAoEDamage(GameObject* target,
                          ItemComponent* weapon,
                          int centralDamage,
                          bool againstRes);

    bool IsObjectVisible(const Position& from,
                         const Position& to,
                         bool excludeEnd = false);

    bool IsTileDangerous(const Position& pos);

    GameObject* GetActorAtPosition(int x, int y);
    GameObject* GetStaticGameObjectAtPosition(int x, int y);

    GameObject* FindGameObjectById(const uint64_t& objId,
                                   GameObjectCollectionType collectionType);

    GameObject* GetMapObjectAtPosition(int x, int y);

    std::vector<GameObject*> GetGameObjectsAtPosition(int x, int y);

    std::pair<int, GameObject*> GetGameObjectToPickup(int x, int y);
    std::vector<std::pair<int, GameObject*>> GetGameObjectsToPickup(int x, int y);

    MapLevelBase* GetLevelRefByType(MapType type);

    std::vector<Position> GetWalkableCellsAround(const Position& pos);
    std::vector<Position> GetEmptyCellsAround(const Position& pos, int range);
    std::vector<MapType> GetAllVisitedLevels();

    Position GetRandomEmptyCell();

    int CountEmptyCellsAround(int x, int y);
    int CountAroundStatic(int x, int y, GameObjectType type);
    int CountWallsOrthogonal(int x, int y);

    MapLevelBase* CurrentLevel = nullptr;

    template <typename Collection>
    GameObject* FindInVV(const Collection& c,
                         const uint64_t& objId)
    {
      for (auto& line : c)
      {
        for (auto& o : line)
        {
          if (o.get() != nullptr && o.get()->ObjectId() == objId)
          {
            return o.get();
          }
        }
      }

      return nullptr;
    }

    template <typename Collection>
    GameObject* FindInV(const Collection& c,
                        const uint64_t& objId)
    {
      for (auto& o : c)
      {
        if (o.get() != nullptr && o.get()->ObjectId() == objId)
        {
          return o.get();
        }
      }

      return nullptr;
    }

  protected:
    void InitSpecific() override;

  private:
    bool _townLoaded = false;

    std::unordered_map<MapType, std::unique_ptr<MapLevelBase>> _levels;
    std::unordered_map<MapType, bool> _mapVisitFirstTime;

    void ChangeOrInstantiateLevel(MapType levelName);
    void ShowLoadingText(const std::string& textOverride = std::string());
    void DrawNonVisibleMapTile(int x, int y);
    void DrawNonVisibleStaticObject(int x, int y);
    void DrawMapTilesAroundPlayer();
    void DrawGameObjects();
    void DrawActors();

    void UpdateGameObjects();
    void UpdateActors();

    void RemoveTriggers();
    void RemoveStaticObjects();
    void EraseFromCollection(std::vector<std::unique_ptr<GameObject>>& list);

    std::pair<uint32_t, uint32_t> GetActorColors(GameObject* actor);

    Player* _playerRef = nullptr;

    Position _windowSize;

    template <typename T>
    void InstantiateLevel(int sizeX, int sizeY, MapType type, int dungeonLevel)
    {
      _levels[type] = std::make_unique<T>(sizeX, sizeY, type, dungeonLevel);
    }

    friend class Application;
};

#endif
