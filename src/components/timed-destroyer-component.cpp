#include "timed-destroyer-component.h"

#include "game-object.h"
#include "map.h"

TimedDestroyerComponent::TimedDestroyerComponent(int delay, const std::function<void()>& onTimerEnd)
{
  _time = delay;
  _onTimerEnd = onTimerEnd;
}

// =============================================================================

void TimedDestroyerComponent::Update()
{
  _time--;

  if (_time <= 0)
  {
    if (Util::IsFunctionValid(_onTimerEnd))
    {
      _onTimerEnd();
    }

    OwnerGameObject->IsDestroyed = true;
  }
}
