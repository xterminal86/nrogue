#ifndef ITEMCOMPONENT_H
#define ITEMCOMPONENT_H

#include "component.h"
#include "container-component.h"

#include "constants.h"

class ItemComponent : public Component
{
  public:
    ItemComponent();

    void Update() override;

    bool Use();
    void Transfer(ContainerComponent* destination = nullptr);
    void Inspect();
    bool Equip();
    void Throw();

    int Durability = -1;
    int Cost = 0;
    int Amount = 1;
    bool IsStackable = false;
    bool IsEquipped = false;

    ItemType TypeOfObject = ItemType::DUMMY;
    EquipmentCategory EquipmentType = EquipmentCategory::NOT_EQUIPPABLE;

    std::vector<std::string> Description;

    Attribute Damage;
};

#endif // ITEMCOMPONENT_H
