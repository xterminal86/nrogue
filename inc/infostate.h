#ifndef INFOSTATE_H
#define INFOSTATE_H

#include "gamestate.h"

class InfoState : public GameState
{
  public:
    void HandleInput() override;
    void Update() override;
};

#endif
