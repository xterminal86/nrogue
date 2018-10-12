#ifndef LOOKINPUTSTATE_H
#define LOOKINPUTSTATE_H

#include "gamestate.h"

#include "player.h"
#include "util.h"

class LookInputState : public GameState
{
  public:
    void Init() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;
    void Prepare() override;

  private:
    void MoveCursor(int dx, int dy);
    void DrawCursor();

    Player* _playerRef;

    Position _cursorPosition;
};

#endif // LOOKINPUTSTATE_H
