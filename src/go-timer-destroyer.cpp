#include "go-timer-destroyer.h"
#include "game-object.h"

TimerDestroyerComponent::TimerDestroyerComponent()
{
  _hash = typeid(*this).hash_code();
}

void TimerDestroyerComponent::Update()
{
  Time--;

  if (Time <= 0)
  {
    ((GameObject*)OwnerGameObject)->IsDestroyed = true;
  }
}
