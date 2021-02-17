#ifndef MESSAGELOGSTATE_H
#define MESSAGELOGSTATE_H

#include "gamestate.h"

class MessageLogState : public GameState
{
  public:
    void Prepare() override;

    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

  private:
    void DrawScrollBars();

    const int kMaxGameLogMessages = 100;

    int _scrollPosition = 0;

    const std::string _windowHeader = " GAME LOG ";
};

#endif // MESSAGELOGSTATE_H
