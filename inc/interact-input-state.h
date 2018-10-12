#ifndef INTERACTINPUTSTATE_H
#define INTERACTINPUTSTATE_H

#include "gamestate.h"

#include "player.h"
#include "util.h"
#include "game-object.h"

class InteractInputState : public GameState
{
  public:
    void Init() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;
    void Prepare() override;

  private:
    void TryToInteractWithObject(GameObject* go);

    Player* _playerRef;

    Position _cursorPosition;

    bool _playerTurnDone;
};

#endif // INTERACTINPUTSTATE_H
