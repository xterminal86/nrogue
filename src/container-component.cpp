#include "container-component.h"

ContainerComponent::ContainerComponent()
{
  _hash = typeid(*this).hash_code();
}

void ContainerComponent::Update()
{
}

void ContainerComponent::AddObject(GameObject* object)
{
  Contents.push_back(std::unique_ptr<GameObject>(object));
}
