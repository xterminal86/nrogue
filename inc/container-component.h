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

    void AddObject(GameObject* object);

    std::vector<std::unique_ptr<GameObject>> Contents;
};

#endif // CONTAINERCOMPONENT_H
