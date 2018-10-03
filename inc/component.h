#ifndef COMPONENT_H
#define COMPONENT_H

#include <typeinfo>

class GameObject;

class Component
{
  public:
    Component()
    {
      _hash = typeid(*this).hash_code();
    }

    GameObject* Owner;

    virtual void Update() = 0;
    
    size_t Hash() { return _hash; }
    
  protected:
    size_t _hash;    
};

#endif
