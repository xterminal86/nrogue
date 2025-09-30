#ifndef COMPONENT_H
#define COMPONENT_H

#include <typeinfo>

#include <stdlib.h>

#ifdef DEBUG_BUILD
#include <string>
#include <vector>

#include "util.h"
#endif

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

#ifdef DEBUG_BUILD
    virtual StringV Dump(size_t indent = 0);
    std::string HexAddressString;
#endif

  protected:
    virtual void PrepareAdditional();
};

#endif
