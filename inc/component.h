#ifndef COMPONENT_H
#define COMPONENT_H

#include <typeinfo>

#include <stdlib.h>

class Component
{
  public:
    Component();

    virtual ~Component() = default;

    virtual void Update() = 0;
    
    size_t Hash();

    // A little bit of a hack probably, but it doesn't compile
    // otherwise due to cyclic dependency shit or whatever.
    //
    // This is GameObject*, use C style cast 
    // when you need access to the owner of this component.
    void* OwnerGameObject;

  protected:
    size_t _hash;
};

#endif
