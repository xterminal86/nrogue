#ifndef LOOKINPUTSTATE_H
#define LOOKINPUTSTATE_H

#include "gamestate.h"
#include "position.h"

class Player;
class GameObject;

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
    void DisplayMonsterStats();

    bool CheckPlayer();
    GameObject* CheckActor();
    const std::vector<GameObject*> CheckGameObjects();

    void PrintDebugInfo();

    Player* _playerRef;

    Position _cursorPosition;
};

#endif // LOOKINPUTSTATE_H
