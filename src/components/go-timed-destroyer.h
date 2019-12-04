#ifndef GOTIMEDDESTROYER_H
#define GOTIMEDDESTROYER_H

#include "component.h"

class TimedDestroyerComponent : public Component
{
  public:
    TimedDestroyerComponent();

    void Update() override;

    int Time;
};

#endif // GOTIMEDDESTROYER_H
