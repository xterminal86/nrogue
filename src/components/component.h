#ifndef COMPONENT_H
#define COMPONENT_H

#include <typeinfo>

#include <stdlib.h>

class GameObject;

class Component
{
  public:
    Component();

    virtual ~Component() = default;

    void Prepare(GameObject* owner);

    virtual void Update() = 0;

    GameObject* OwnerGameObject = nullptr;

    bool IsEnabled = true;

  protected:
    virtual void PrepareAdditional();
};

#endif
