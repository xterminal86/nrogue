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
    void Update(bool forceUpdate = false) override;    

  private:
    Player* _playerRef;

    std::string _debugInfo;

    void DisplayGameLog();
    void TryToPickupItem();
    void DrawHPMP();
    std::string UpdateBar(int x, int y, Attribute attr);

    Position _cursorPosition;
};

#endif
