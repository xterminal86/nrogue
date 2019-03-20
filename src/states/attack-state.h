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
    Player* _playerRef;

    Position _cursorPosition;

    bool CheckPickaxe();

    void ProcessPickaxe();
    void ProcessWeapon();
};

#endif // ATTACKSTATE_H
