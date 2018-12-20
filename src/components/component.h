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

    size_t Hash();

    GameObject* OwnerGameObject;

  protected:
    size_t _hash;
};

#endif
