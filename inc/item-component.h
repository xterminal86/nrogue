#ifndef ITEMCOMPONENT_H
#define ITEMCOMPONENT_H

#include "component.h"

#include "constants.h"

class ItemComponent : public Component
{
  public:
    ItemComponent();

    void Update() override;

    void Use();
    void Drop();
    void Inspect();
    void Equip();
    void Throw();

    int Durability;
    int Cost;

    Attribute Damage;
};

#endif // ITEMCOMPONENT_H
