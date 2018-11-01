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

    ItemData Data;    
};

#endif // ITEMCOMPONENT_H
