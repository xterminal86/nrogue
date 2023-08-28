#ifndef STARTGAMESTATE_H
#define STARTGAMESTATE_H

#include "gamestate.h"

class StartGameState : public GameState
{
  public:
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;
};

#endif // LOADTOWNSTATE_H
