#ifndef TIMER_H
#define TIMER_H

#include <string>
#include <chrono>
#include <cstdint>
#include <unordered_map>

#include "singleton.h"
#include "constants.h"

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
    void StartProfiling(const std::string& timestampName);
    void FinishProfiling(const std::string& timestampName);

    const Ns& TimePassedDur();
    const Ns& DeltaTimeDur();

    const double& DelatTimeMs();

    void MeasureStart();
    void MeasureEnd();

    const StringV& GetProfilingReport();

  protected:
    void InitSpecific() override;

  private:
    //
    // Will use nanoseconds for maximum accuracy.
    //
    Ns _deltaTime  = Ns{0};
    Ns _timePassed = Ns{0};

    Clock::time_point _measureStart;
    Clock::time_point _measureEnd;

    double _dt = 0.0;

    struct ProfilerData
    {
      Clock::time_point TimePoint;
      double DeltaTime = 0.0;
    };

    std::unordered_map<std::string, ProfilerData> _timestamps;

    StringV _report;
};

#endif // TIMER_H
