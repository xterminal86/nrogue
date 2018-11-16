#ifndef CONTAINERCOMPONENT_H
#define CONTAINERCOMPONENT_H

#include <vector>
#include <memory>

#include "component.h"
#include "game-object.h"

class ContainerComponent : public Component
{
  public:
    ContainerComponent();

    void Update() override;

    void AddToInventory(GameObject* object);

    bool IsFull();
    bool IsEmpty();

    void Interact();

    std::vector<std::unique_ptr<GameObject>> Contents;

    int MaxCapacity = 0;
};

#endif // CONTAINERCOMPONENT_H
