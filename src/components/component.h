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

    // Called after player finishes his turn
    virtual void Update() = 0;    

    size_t GetComponentHash();

    // Automatically assigned in GameObject::AddComponent()
    GameObject* OwnerGameObject;

  protected:
    size_t _componentHash;
};

#endif
