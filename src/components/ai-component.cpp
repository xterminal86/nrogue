#include "ai-component.h"

AIComponent::AIComponent()
{
  _hash = typeid(*this).hash_code();
}

void AIComponent::Update()
{
  if (CurrentModel != nullptr)
  {
    CurrentModel->Update();
  }
}
