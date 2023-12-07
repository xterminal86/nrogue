#include "timer.h"

void Timer::InitSpecific()
{
}

// =============================================================================

double Timer::DeltaTime()
{
  return FT::duration<double, std::milli>(_dt).count();
}

// =============================================================================

const Ns& Timer::TimePassed()
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
  _deltaTime  = _measureEnd - _measureStart;

  _dt = FT::duration<double>(_deltaTime).count();

  _timePassed += _deltaTime;
}
