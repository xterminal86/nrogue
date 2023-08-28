#ifndef SAVEGAMESTATE_H
#define SAVEGAMESTATE_H

#include "gamestate.h"

class Player;

class SaveGameState : public GameState
{
  public:
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;
    void Prepare() override;

  private:
    void SaveGame();

    Player* _playerRef = nullptr;
};

#endif // SAVEGAMESTATE_H
