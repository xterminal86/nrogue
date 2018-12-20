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
    void UpdateGameObjects();

    void ChangeLevel(MapType levelToChange, bool goingDown);

    void PrintMapArrayRevealedStatus();
    void PrintMapLayout();

    GameObject* GetActorAtPosition(int x, int y);
    std::vector<GameObject*> GetGameObjectsAtPosition(int x, int y);
    std::pair<int, GameObject*> GetGameObjectToPickup(int x, int y);

    MapLevelBase* CurrentLevel;

  private:    
    std::map<MapType, std::unique_ptr<MapLevelBase>> _levels;
    std::map<MapType, bool> _mapVisitFirstTime;

    void RemoveDestroyed();
    void ChangeOrInstantiateLevel(MapType levelName);
    void ShowLoadingText();
};

#endif
