#ifndef TIMER_H
#define TIMER_H

#include "singleton.h"

#include <chrono>

//
// For "Fucking Time"
//
namespace FT = std::chrono;

//
// Let's try to use steady_clock since it's claimed to be increasing
// at a uniform rate.
//
using Clock = FT::steady_clock;

using Ms  = FT::milliseconds;
using Ns  = FT::nanoseconds;
using Sec = FT::seconds;

class Timer : public Singleton<Timer>
{
  public:
    const Ns& TimePassed();

    double DeltaTime();

    void MeasureStart();
    void MeasureEnd();

  protected:
    void InitSpecific() override;

  private:
    //
    // chrono clock now() uses
    // nanoseconds as internal duration period
    // (see time_point struct declaration)
    //
    Ns _deltaTime  = Ns{0};
    Ns _timePassed = Ns{0};

    Clock::time_point _measureStart;
    Clock::time_point _measureEnd;

    double _dt = 0.0;
};

#endif // TIMER_H
