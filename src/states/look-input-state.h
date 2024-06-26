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

    #ifdef DEBUG_BUILD
    void PrintDebugInfo();
    #endif

    Player* _playerRef = nullptr;

    Position _cursorPosition;

    std::vector<std::string> _monsterStatsInfo;

    #ifdef DEBUG_BUILD
    const StringV _debugInfo =
    {
      "'ENTER' - display actor stats",
      "",
      "'f' - player's potential field value here",
      "'d' - destroy static object here",
      "'D' - destroy updatable game object here"
    };

    std::string _distanceField;
    #endif

};

#endif // LOOKINPUTSTATE_H
