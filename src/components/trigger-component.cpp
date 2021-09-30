#include "trigger-component.h"

TriggerComponent::TriggerComponent()
{
  _componentHash = typeid(*this).hash_code();
}

void TriggerComponent::Update()
{
}
