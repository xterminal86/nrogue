#ifndef EQUIPMENTCOMPONENT_H
#define EQUIPMENTCOMPONENT_H

#include <map>
#include <vector>

#include "constants.h"
#include "component.h"

class ItemComponent;
class ContainerComponent;
class Player;

//
// Not every GameObject can be equipped, so in order
// not to overload already overloaded GameObject class,
// functionality is moved to separate component.
//
class EquipmentComponent : public Component
{
  public:
    explicit EquipmentComponent(ContainerComponent* inventoryRef);

    void Update() override;

    bool Equip(ItemComponent* item, bool suppressLog = false);
    bool HasBonus(ItemBonusType type);

    std::unordered_map<EquipmentCategory, std::vector<ItemComponent*>>
    EquipmentByCategory;

#ifdef DEBUG_BUILD
    StringV Dump(size_t indent = 0) override;
#endif

  private:
    bool ProcessItemEquiption(ItemComponent* item, bool suppressLog);
    bool ProcessRingEquiption(ItemComponent* item, bool suppressLog);

    void EquipRing(ItemComponent* ring, int index, bool suppressLog);
    void UnequipRing(ItemComponent* ring, int index, bool suppressLog);

    void EquipItem(ItemComponent* item, bool suppressLog);
    void UnequipItem(ItemComponent* item, bool suppressLog);

    bool IsThisPlayer();

    ContainerComponent* _inventory = nullptr;

    Player* _playerRef = nullptr;
};

#endif // EQUIPMENTCOMPONENT_H
