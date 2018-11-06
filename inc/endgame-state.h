#ifndef ENDGAMESTATE_H
#define ENDGAMESTATE_H

#include "gamestate.h"
#include "constants.h"

class Player;

class EndgameState : public GameState
{
  public:
    void Init() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

  private:
    void DisplayGameLog();
    void DrawHPMP();
    std::string UpdateBar(int x, int y, Attribute attr);

    Player* _playerRef;
};

#endif // ENDGAMESTATE_H
