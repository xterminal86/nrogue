#include "component.h"

Component::Component()
{
  // NOTE: repeat this line in all
  // subsequent custom components constructors
  _componentHash = typeid(*this).hash_code();
}

size_t Component::GetComponentHash()
{
  return _componentHash;
}
