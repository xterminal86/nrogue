#ifndef EQUIPMENTCOMPONENT_H
#define EQUIPMENTCOMPONENT_H

#include <map>
#include <vector>

#include "constants.h"
#include "component.h"

class ItemComponent;
class ContainerComponent;

class EquipmentComponent : public Component
{
  public:
    EquipmentComponent();

    void Update() override;

    std::map<EquipmentCategory, std::vector<ItemComponent*>> EquipmentByCategory;

  private:
    ContainerComponent* _inventory;
};

#endif // EQUIPMENTCOMPONENT_H
