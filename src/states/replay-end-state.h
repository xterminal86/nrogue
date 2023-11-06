#ifndef REPLAYENDSTATE_H
#define REPLAYENDSTATE_H

#include "gamestate.h"

class ReplayEndState : public GameState
{
  public:
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;
};

#endif // REPLAYENDSTATE_H
