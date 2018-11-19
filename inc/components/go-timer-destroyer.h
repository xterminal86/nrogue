#ifndef GOTIMERDESTROYER_H
#define GOTIMERDESTROYER_H

#include "component.h"

class TimerDestroyerComponent : public Component
{
  public:
    TimerDestroyerComponent();

    void Update() override;

    int Time;
};

#endif // GOTIMERDESTROYER_H
