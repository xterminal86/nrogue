#ifndef INTERACTINPUTSTATE_H
#define INTERACTINPUTSTATE_H

#include "gamestate.h"
#include "position.h"

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
    void ProcessInteraction();
    void TryToInteractWithObject(GameObject* go);
    void TryToInteractWithActor(GameObject* actor);

    bool SetDir(const Position& dir);

    Player* _playerRef;

    Position _cursorPosition;
};

#endif // INTERACTINPUTSTATE_H
