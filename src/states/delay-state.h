#ifndef DELAYSTATE_H
#define DELAYSTATE_H

#include "gamestate.h"

class DelayState : public GameState
{
  public:
    void Prepare() override;
    void Cleanup() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

    void Setup(GameStates from, uint32_t ms);

  private:
    double _accumulator = 0.0;

    uint32_t _delay = 0;

    GameStates _changeBackTo = GameStates::UNDEIFNED;
};

#endif // DELAYSTATE_H
