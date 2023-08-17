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
    GameObject* CreateShrine(int x, int y, ShrineType type, int timeout);
    GameObject* CreateRemains(GameObject* from);

    GameObject* CreateDummyObject(int x,
                                  int y,
                                  const std::string& objName,
                                  char image,
                                  const uint32_t& fgColor,
                                  const uint32_t& bgColor);

    GameObject* CreateContainer(int x,
                                int y,
                                int image,
                                const std::string& name,
                                const uint32_t& bgColor);

    GameObject* CreateBreakableObjectWithRandomLoot(int x,
                                                    int y,
                                                    char image,
                                                    const std::string& objName,
                                                    const uint32_t& fgColor,
                                                    const uint32_t& bgColor);

    //
    // Create invisible trigger object.
    //
    void CreateTrigger(TriggerType triggerType,
                       TriggerUpdateType updateType,
                       const std::function<bool ()>& condition,
                       const std::function<void ()>& handler);

    //
    // Creates stairs on MapArray of current level.
    //
    void CreateStairs(MapLevelBase* levelWhereCreate,
                      int x,
                      int y,
                      int image,
                      MapType leadsTo);

    GameObject* CreateStaticObject(int x,
                                   int y,
                                   const GameObjectInfo& objectInfo,
                                   int hitPoints = -1,
                                   GameObjectType type = GameObjectType::HARMLESS);

    GameObject* CreateDoor(int x, int y,
                           bool isOpen,
                           DoorMaterials material = DoorMaterials::WOOD,
                           const std::string& doorName = std::string(),
                           int hitPoints = 10,
                           const uint32_t& fgOverrideColor = Colors::None,
                           const uint32_t& bgOverrideColor = Colors::None);

    ItemComponent* CloneItem(ItemComponent* copyFrom);
    GameObject* CloneObject(GameObject* copyFrom);

  protected:
    void InitSpecific() override;

  private:
    Player* _playerRef = nullptr;

    // -------------------------------------------------------------------------

    const std::unordered_map<DoorMaterials, int> _doorDefByMat =
    {
      { DoorMaterials::WOOD,  5  },
      { DoorMaterials::STONE, 10 },
      { DoorMaterials::IRON,  15 }
    };

    const std::unordered_map<DoorMaterials, std::string> _doorPrefixByMat =
    {
      { DoorMaterials::WOOD,  "Wooden" },
      { DoorMaterials::STONE, "Stone"  },
      { DoorMaterials::IRON,  "Iron"   }
    };

    // -------------------------------------------------------------------------
};

#endif // GAMEOBJECTSFACTORY_H
