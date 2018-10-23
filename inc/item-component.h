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
    void Equip();
    void Throw();

    int Durability = -1;
    int Cost = 0;
    int Amount = 1;
    bool IsStackable = false;

    ItemType TypeOfObject;

    std::vector<std::string> Description;

    Attribute Damage;
};

#endif // ITEMCOMPONENT_H
