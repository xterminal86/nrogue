#ifndef GOTIMEDDESTROYER_H
#define GOTIMEDDESTROYER_H

#include <functional>

#include "component.h"

class TimedDestroyerComponent : public Component
{
  public:
    TimedDestroyerComponent(int delay, const std::function<void()>& onDecay = std::function<void()>());

    void Update() override;

  private:
    int _time;

    std::function<void()> _onDecay;
};

#endif // GOTIMEDDESTROYER_H
