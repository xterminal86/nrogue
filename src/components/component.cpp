#include "component.h"

Component::Component()
{
  // NOTE: repeat this line in all
  // subsequent custom components constructors
  _hash = typeid(*this).hash_code();
}

size_t Component::Hash()
{
  return _hash;
}
