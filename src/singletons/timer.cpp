#include "timer.h"

void Timer::InitSpecific()
{
}

// =============================================================================

double Timer::DeltaTime()
{
  return (_dt * 1000.0);
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

  _dt = std::chrono::duration<double>(_measureEnd - _measureStart).count();

  _timePassed += _deltaTime;
}
