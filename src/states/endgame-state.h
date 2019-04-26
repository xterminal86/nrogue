#ifndef ENDGAMESTATE_H
#define ENDGAMESTATE_H

#include "gamestate.h"

class Attribute;
class Player;

class EndgameState : public GameState
{
  public:
    void Init() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;
    void Prepare() override;

  private:
    void DisplayGameLog();
    void DrawHPMP();
    std::string UpdateBar(int x, int y, const Attribute& attr);

    Player* _playerRef;
};

#endif // ENDGAMESTATE_H
