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
    EquipmentComponent(ContainerComponent* inventoryRef);

    void Update() override;

    bool Equip(ItemComponent* item);
    bool HasBonus(ItemBonusType type);

    std::map<EquipmentCategory, std::vector<ItemComponent*>> EquipmentByCategory;

  private:
    bool ProcessItemEquiption(ItemComponent* item);
    bool ProcessRingEquiption(ItemComponent* item);

    void EquipRing(ItemComponent* ring, int index);
    void UnequipRing(ItemComponent* ring, int index);

    void EquipItem(ItemComponent* item);
    void UnequipItem(ItemComponent* item);

    bool IsThisPlayer();

    ContainerComponent* _inventory = nullptr;

    Player* _playerRef = nullptr;
};

#endif // EQUIPMENTCOMPONENT_H
