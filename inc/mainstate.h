#ifndef MAINSTATE_H
#define MAINSTATE_H

#include <vector>
#include <memory>

#include "gamestate.h"
#include "util.h"
#include "player.h"
#include "game-object.h"

class MainState : public GameState
{
  public:
    void Init() override;
    void HandleInput() override;
    void Update() override;

  private:
    Player* _playerRef;

    std::string _debugInfo;

    void DisplayGameLog();

    Position _cursorPosition;

    bool _playerTurnDone;
};

#endif
