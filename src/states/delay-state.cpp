#include "delay-state.h"
#include "timer.h"
#include "application.h"

void DelayState::HandleInput()
{
}

// =============================================================================

void DelayState::Prepare()
{
  _accumulator = 0.0;
}

// =============================================================================

void DelayState::Cleanup()
{
  _accumulator = 0.0;
}

// =============================================================================

void DelayState::Setup(GameStates from, uint32_t ms)
{
  _changeBackTo = from;
  _delay = ms;
}

// =============================================================================

void DelayState::Update(bool forceUpdate)
{
  _accumulator += Timer::Instance().DeltaTime();

#ifdef DEBUG_BUILD
  DebugLog("delay state %f", _accumulator);
#endif

  if (_accumulator > _delay)
  {
    Application::Instance().ChangeState(_changeBackTo);
  }
}
