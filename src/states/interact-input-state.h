#ifndef INTERACTINPUTSTATE_H
#define INTERACTINPUTSTATE_H

#include "gamestate.h"
#include "util.h"

class GameObject;
class Player;

class InteractInputState : public GameState
{
  public:
    void Init() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;
    void Prepare() override;

  private:
    void TryToInteractWithObject(GameObject* go);
    void TryToInteractWithActor(GameObject* actor);

    Player* _playerRef;

    Position _cursorPosition;    
};

#endif // INTERACTINPUTSTATE_H
