#include "stairs-component.h"

StairsComponent::StairsComponent()
{
  _hash = typeid(*this).hash_code();
}

void StairsComponent::Update()
{
}
