#ifndef EXITINGSTATE_H
#define EXITINGSTATE_H

#include "gamestate.h"

class Player;

class ExitingState : public GameState
{
  public:
    void Init() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

  private:
    Player* _playerRef;
};

#endif // EXITINGSTATE_H
