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

    bool AddToInventory(GameObject* object);

    bool IsFull();
    bool IsEmpty();

    bool Interact();

    std::vector<std::unique_ptr<GameObject>> Contents;

    size_t MaxCapacity = 0;

    bool CanBeOpened = true;
};

#endif // CONTAINERCOMPONENT_H
