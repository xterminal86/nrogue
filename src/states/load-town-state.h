#ifndef LOADTOWNSTATE_H
#define LOADTOWNSTATE_H

#include "gamestate.h"

class LoadTownState : public GameState
{
  public:
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;
};

#endif // LOADTOWNSTATE_H
