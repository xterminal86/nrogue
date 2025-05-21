#include "timer.h"

#include "util.h"

void Timer::InitSpecific()
{
}

// =============================================================================

void Timer::StartProfiling(const std::string& timestampName)
{
  #ifdef DEBUG_BUILD
  _timestamps[timestampName] = { Clock::now(), 0.0 };
  #endif
}

// =============================================================================

const StringV& Timer::GetProfilingReport()
{
  #ifdef DEBUG_BUILD
  _report.resize(_timestamps.size());

  size_t ind = 0;
  for (auto& kvp : _timestamps)
  {
    const std::string& ts  = kvp.first;
    const ProfilerData& pd = kvp.second;

    _report[ind] = Util::StringFormat("%s: %.2f", ts.data(), pd.DeltaTime);
    ind++;
  }
  #endif

  return _report;
}

// =============================================================================

void Timer::FinishProfiling(const std::string& timestampName)
{
  #ifdef DEBUG_BUILD
  if (_timestamps.count(timestampName))
  {
    Ns dt = FT::duration_cast<Ns>(
              Clock::now() - _timestamps[timestampName].TimePoint
    );

    _timestamps[timestampName].DeltaTime = (double)dt.count() / 1000000.0;
  }
  #endif
}

// =============================================================================

const Ns& Timer::DeltaTimeDur()
{
  return _deltaTime;
}

// =============================================================================

const double& Timer::DelatTimeMs()
{
  return _dt;
}

// =============================================================================

const Ns& Timer::TimePassedDur()
{
  return _timePassed;
}

// =============================================================================

void Timer::MeasureStart()
{
  #ifdef DEBUG_BUILD
  _timestamps.clear();
  #endif

  _measureStart = Clock::now();
  _measureEnd   = _measureStart;
}

// =============================================================================

void Timer::MeasureEnd()
{
  _measureEnd = Clock::now();
  _deltaTime  = FT::duration_cast<Ns>(_measureEnd - _measureStart);

  _dt = (double)_deltaTime.count() / 1000000.0;

  _timePassed += _deltaTime;
}
