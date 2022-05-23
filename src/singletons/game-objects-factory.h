#ifndef GAMEOBJECTSFACTORY_H
#define GAMEOBJECTSFACTORY_H

#include "singleton.h"
#include "constants.h"

class GameObjectInfo;
class ItemComponent;
class ContainerComponent;
class GameObject;
class Player;
class MapLevelBase;
class DoorComponent;

class GameObjectsFactory : public Singleton<GameObjectsFactory>
{
  public:
    // For random generation
    GameObject* CreateGameObject(int x, int y, ItemType objType);

    // Various
    GameObject* CreateShrine(int x, int y, ShrineType type, int timeout);
    GameObject* CreateRemains(GameObject* from);
    GameObject* CreateDummyObject(int x, int y, const std::string& objName, char image, const std::string& fgColor, const std::string& bgColor);
    GameObject* CreateContainer(const std::string& name, const std::string& bgColor, int image, int x, int y);
    GameObject* CreateBreakableObjectWithRandomLoot(int x, int y, char image, const std::string& objName, const std::string& fgColor, const std::string& bgColor);

    // Create invisible trigger object
    void CreateTrigger(TriggerType triggerType,
                       TriggerUpdateType updateType,
                       const std::function<bool ()>& condition,
                       const std::function<void ()>& handler);

    // Creates stairs on MapArray of current level
    void CreateStairs(MapLevelBase* levelWhereCreate, int x, int y, int image, MapType leadsTo);

    GameObject* CreateStaticObject(int x, int y, const GameObjectInfo& objectInfo, int hitPoints = -1, GameObjectType type = GameObjectType::HARMLESS);
    GameObject* CreateDoor(int x, int y,
                           bool isOpen,
                           DoorMaterials material = DoorMaterials::WOOD,
                           const std::string& doorName = std::string(),
                           int hitPoints = 10,
                           const std::string& fgOverrideColor = std::string(),
                           const std::string& bgOverrideColor = std::string());

    ItemComponent* CloneItem(ItemComponent* copyFrom);
    GameObject* CloneObject(GameObject* copyFrom);

  protected:
    void InitSpecific() override;

  private:
    Player* _playerRef = nullptr;
};

#endif // GAMEOBJECTSFACTORY_H
