#ifndef GAMEOVERSTATE_H
#define GAMEOVERSTATE_H

#include "gamestate.h"

class RangedAttribute;
class Player;

class GameOverState : public GameState
{
  public:
    void Init() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;
    void Prepare() override;

  private:
    void DisplayGameLog();
    void DrawHPMP();
    void UpdateBar(int x, int y, RangedAttribute& attr);

    Player* _playerRef = nullptr;

    std::string _bar;
};

#endif // GAMEOVERSTATE_H
