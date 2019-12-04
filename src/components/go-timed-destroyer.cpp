#include "go-timed-destroyer.h"
#include "game-object.h"

TimedDestroyerComponent::TimedDestroyerComponent()
{
  _componentHash = typeid(*this).hash_code();
}

void TimedDestroyerComponent::Update()
{
  Time--;

  if (Time <= 0)
  {
    OwnerGameObject->IsDestroyed = true;
  }
}
