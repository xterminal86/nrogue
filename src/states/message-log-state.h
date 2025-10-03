#ifndef MESSAGELOGSTATE_H
#define MESSAGELOGSTATE_H

#include "gamestate.h"

class MessageLogState : public GameState
{
  public:
    void Prepare() override;
    void Cleanup() override;

    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

  private:
    const std::string _windowHeader = " GAME LOG ";
};

#endif // MESSAGELOGSTATE_H
