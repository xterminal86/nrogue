#include "timer.h"

void Timer::InitSpecific()
{
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
