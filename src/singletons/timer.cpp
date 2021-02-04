#include "timer.h"

const Ns& Timer::DeltaTime()
{
  return _deltaTime;
}

const Ns& Timer::TimePassed()
{
  return _timePassed;
}

void Timer::MeasureStart()
{
  _measureStart = Clock::now();
  _measureEnd   = _measureStart;
}

void Timer::MeasureEnd()
{
  _measureEnd  = Clock::now();
  _deltaTime   = _measureEnd - _measureStart;
  _timePassed += _deltaTime;
}
