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

    bool Add(GameObject* object);

    bool IsFull();
    bool IsEmpty();

    IR Interact();

    std::vector<std::unique_ptr<GameObject>> Contents;

    bool CanBeOpened = true;

    size_t MaxCapacity = 0;
};

#endif // CONTAINERCOMPONENT_H
