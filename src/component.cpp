#include "component.h"

Component::Component()
{
  _hash = typeid(*this).hash_code();
}

size_t Component::Hash()
{
  return _hash;
}
