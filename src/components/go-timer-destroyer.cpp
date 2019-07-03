#include "go-timer-destroyer.h"
#include "game-object.h"

TimerDestroyerComponent::TimerDestroyerComponent()
{
  _componentHash = typeid(*this).hash_code();
}

void TimerDestroyerComponent::Update()
{
  Time--;

  if (Time <= 0)
  {
    OwnerGameObject->IsDestroyed = true;
  }
}
