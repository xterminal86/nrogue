#include "ai-component.h"

AIComponent::AIComponent()
{
  _componentHash = typeid(*this).hash_code();
}

void AIComponent::Update()
{
  if (CurrentModel != nullptr)
  {
    CurrentModel->Update();
  }
}
