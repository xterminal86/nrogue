#ifndef RETURNERSTATE_H
#define RETURNERSTATE_H

#include "gamestate.h"

#include "player.h"
#include "util.h"

class ReturnerState : public GameState
{
  public:
    void Prepare() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;
    void SetItemComponentRef(ItemComponent* item);

  private:
    Position GetRandomPositionAroundPlayer();

    Player* _playerRef;

    ItemComponent* _itemRef;
};

#endif // RETURNERSTATE_H
