#include "go-timed-destroyer.h"

#include "game-object.h"
#include "map.h"

TimedDestroyerComponent::TimedDestroyerComponent(int delay)
{
  _componentHash = typeid(*this).hash_code();

  _time = delay;
}

void TimedDestroyerComponent::Update()
{
  _time--;

  if (_time <= 0)
  {
    OwnerGameObject->IsDestroyed = true;
  }
}
