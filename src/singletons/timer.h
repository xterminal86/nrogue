#ifndef TIMER_H
#define TIMER_H

#include "singleton.h"

#include <chrono>

using Clock = std::chrono::high_resolution_clock;
using Ms = std::chrono::milliseconds;
using Ns = std::chrono::nanoseconds;
using Sec = std::chrono::seconds;

class Timer : public Singleton<Timer>
{
  public:
    const Ns& DeltaTime();
    const Ns& TimePassed();

    const float& DT();

    void MeasureStart();
    void MeasureEnd();

  private:
    // chrono clock now() uses
    // nanoseconds as internal duration period
    // (see time_point struct declaration)
    Ns _deltaTime  = Ns{0};
    Ns _timePassed = Ns{0};

    Clock::time_point _measureStart;
    Clock::time_point _measureEnd;

    float _dt = 0.0f;
};

#endif // TIMER_H
