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

    void* Owner;

  protected:
    size_t _hash;
};

#endif
