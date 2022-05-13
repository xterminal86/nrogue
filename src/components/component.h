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

    const size_t& GetComponentHash();

    GameObject* OwnerGameObject;

    bool IsEnabled = true;

  protected:
    virtual void PrepareAdditional();

    size_t _componentHash;
};

#endif
