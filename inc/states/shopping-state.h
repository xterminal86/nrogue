#ifndef SHOPPINGSTATE_H
#define SHOPPINGSTATE_H

#include "gamestate.h"

class ShoppingState : public GameState
{
  public:
    void Update(bool forceUpdate = false) override;
    void HandleInput() override;
};

#endif // SHOPPINGSTATE_H
