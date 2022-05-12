#include "go-timed-destroyer.h"

#include "game-object.h"
#include "map.h"

TimedDestroyerComponent::TimedDestroyerComponent(int delay, const std::function<void()>& onDecay)
{
  _componentHash = typeid(*this).hash_code();

  _time = delay;
  _onDecay = onDecay;
}

void TimedDestroyerComponent::Update()
{
  _time--;

  if (_time <= 0)
  {
    if (Util::IsFunctionValid(_onDecay))
    {
      _onDecay();
    }

    OwnerGameObject->IsDestroyed = true;
  }
}
