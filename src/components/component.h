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

    virtual void Update() = 0;

    const size_t& GetComponentHash();

    // Automatically assigned in GameObject::AddComponent()
    GameObject* OwnerGameObject;

    bool IsEnabled = true;

  protected:
    size_t _componentHash;
};

#endif
