#ifndef EXITINGSTATE_H
#define EXITINGSTATE_H

#include "gamestate.h"

#include "player.h"

class ExitingState : public GameState
{
  public:
    void Init() override;
    void HandleInput() override;
    void Update() override;

  private:
    Player* _playerRef;
};

#endif // EXITINGSTATE_H
