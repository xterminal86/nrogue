#ifndef SELECTITEMSTATEBASE_H
#define SELECTITEMSTATEBASE_H

#include "gamestate.h"

class SelectItemStateBase : public GameState
{
  public:
    void HandleInput() final;
    void Update(bool forceUpdate) final;

  protected:
    std::string _headerText = "N/A";

    virtual void DrawSpecific() = 0;
    virtual void ProcessInput() = 0;
};

#endif // SELECTITEMSTATEBASE_H
