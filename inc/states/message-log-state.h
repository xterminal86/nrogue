#ifndef MESSAGELOGSTATE_H
#define MESSAGELOGSTATE_H

#include "gamestate.h"

class MessageLogState : public GameState
{
  public:
    void Prepare() override
    {
      _scrollPosition = 0;
    }

    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

  private:
    void DrawScrollBars();

    const int kMaxGameLogMessages = 100;

    int _scrollPosition = 0;
};

#endif // MESSAGELOGSTATE_H