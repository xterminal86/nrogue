#ifndef INFOSTATE_H
#define INFOSTATE_H

#include "gamestate.h"

class InfoState : public GameState
{
  public:
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

  private:
    void ShowInventory();
};

#endif
