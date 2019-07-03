#include "stairs-component.h"

StairsComponent::StairsComponent()
{
  _componentHash = typeid(*this).hash_code();
}

void StairsComponent::Update()
{
}
