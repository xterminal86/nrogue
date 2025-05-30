#ifndef ATTACKSTATE_H
#define ATTACKSTATE_H

#include "gamestate.h"
#include "position.h"

class Player;

class AttackState : public GameState
{
  public:
    void Init() override;
    void Prepare() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

  private:
    Player* _playerRef = nullptr;

    Position _cursorPosition;
    Position _dir;
};

#endif // ATTACKSTATE_H
