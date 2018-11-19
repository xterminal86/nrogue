#ifndef ITEMCOMPONENT_H
#define ITEMCOMPONENT_H

#include "constants.h"
#include "component.h"

class ContainerComponent;

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

  private:
    std::vector<std::string> GetWeaponInspectionInfo();
};

#endif // ITEMCOMPONENT_H
