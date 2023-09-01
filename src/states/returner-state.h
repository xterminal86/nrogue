#ifndef RETURNERSTATE_H
#define RETURNERSTATE_H

#include "gamestate.h"

#include "position.h"

class Player;
class ItemComponent;

class ReturnerState : public GameState
{
  public:
    void Init() override;
    void Prepare() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;
    void SetItemComponentRef(ItemComponent* item);

  private:
    Position GetRandomPositionAroundPlayer();

    Player* _playerRef = nullptr;

    ItemComponent* _itemRef = nullptr;
};

#endif // RETURNERSTATE_H
