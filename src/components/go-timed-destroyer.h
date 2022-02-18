#ifndef GOTIMEDDESTROYER_H
#define GOTIMEDDESTROYER_H

#include "component.h"

class TimedDestroyerComponent : public Component
{
  public:
    TimedDestroyerComponent(int delay);

    void Update() override;

  private:
    int _time;
};

#endif // GOTIMEDDESTROYER_H
