#ifndef CONTAINERCOMPONENT_H
#define CONTAINERCOMPONENT_H

#include <vector>
#include <memory>

#include "component.h"
#include "game-object.h"

class ContainerComponent : public Component
{
  public:
    ContainerComponent(size_t maxCapacity = GlobalConstants::InventoryMaxSize);

    void Update() override;

    bool Add(GameObject* object);

    bool IsFull();
    bool IsEmpty();

    const size_t& MaxCapacity();

    IR Interact();

    std::vector<std::unique_ptr<GameObject>> Contents;

    bool CanBeOpened = true;

  private:
    size_t _maxCapacity = 0;
};

#endif // CONTAINERCOMPONENT_H
