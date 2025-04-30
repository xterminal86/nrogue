#ifndef TIMEDDESTROYERCOMPONENT_H
#define TIMEDDESTROYERCOMPONENT_H

#include <functional>

#include "component.h"

class TimedDestroyerComponent : public Component
{
  public:
    TimedDestroyerComponent(
        int delay,
        const std::function<void()>& onTimerEnd = std::function<void()>()
    );

    void Update() override;

  private:
    int _time = 0;

    std::function<void()> _onTimerEnd;
};

#endif // TIMEDDESTROYERCOMPONENT_H
