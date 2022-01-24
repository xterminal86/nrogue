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
    GameObject* CreateMonster(int x, int y, GameObjectType monsterType);

    // Various
    GameObject* CreateNPC(int x, int y, NPCType npcType, bool standing = false, ServiceType serviceType = ServiceType::NONE);
    GameObject* CreateShrine(int x, int y, ShrineType type, int timeout);
    GameObject* CreateRemains(GameObject* from);
    GameObject* CreateDummyObject(int x, int y, const std::string& objName, char image, const std::string& fgColor, const std::string& bgColor);
    GameObject* CreateContainer(const std::string& name, const std::string& bgColor, int image, int x, int y);
    GameObject* CreateBreakableObjectWithRandomLoot(int x, int y, char image, const std::string& objName, const std::string& fgColor, const std::string& bgColor);

    // Creates stairs on MapArray of current level
    void CreateStairs(MapLevelBase* levelWhereCreate, int x, int y, int image, MapType leadsTo);

    bool HandleItemUse(ItemComponent* item);
    bool HandleItemEquip(ItemComponent* item);

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
    Player* _playerRef;
};

#endif // GAMEOBJECTSFACTORY_H
