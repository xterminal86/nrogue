#include "timer.h"

const Ns& Timer::DeltaTime()
{
  return _deltaTime;
}

const float& Timer::DT()
{
  return _dt;
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

  _dt = std::chrono::duration<float>(_measureEnd - _measureStart).count();

  _timePassed += _deltaTime;
}
