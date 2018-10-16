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

    void Use();
    void Transfer(ContainerComponent* destination = nullptr);
    void Inspect();
    void Equip();
    void Throw();

    int Durability;
    int Cost;

    std::vector<std::string> Description;

    Attribute Damage;
};

#endif // ITEMCOMPONENT_H
