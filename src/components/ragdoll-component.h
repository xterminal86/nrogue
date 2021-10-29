#ifndef RAGDOLLCOMPONENT_H
#define RAGDOLLCOMPONENT_H

#include "component.h"
#include "constants.h"

class ItemComponent;

class RagdollComponent : public Component
{
  public:
    RagdollComponent();

    void Update() override;

    std::map<EquipmentCategory, std::vector<ItemComponent*>> EquipmentByCategory;
};

#endif // RAGDOLLCOMPONENT_H
