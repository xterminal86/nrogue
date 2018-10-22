#ifndef ATTACKSTATE_H
#define ATTACKSTATE_H

#include "gamestate.h"
#include "player.h"
#include "util.h"

class AttackState : public GameState
{
  public:
    void Init() override;
    void Prepare() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

  private:
    Player* _playerRef;

    Position _cursorPosition;
};

#endif // ATTACKSTATE_H
