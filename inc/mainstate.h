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
    enum class InputStateEnum
    {
      MOVE = 0,
      LOOK,
      INTERACT,
      EXIT_GAME
    };    

    Player* _playerRef;

    std::string _debugInfo;

    InputStateEnum _inputState;

    void ProcessMovement();
    void ProcessLook();
    void ProcessInteraction();
    void ProcessExitGame();

    void DrawMovementState();
    void DrawLookState();
    void DrawInteractionState();
    void DrawExitGameState();

    void MoveCursor(int dx, int dy);
    void DrawCursor();
    void DrawGameObjects();

    void DisplayGameLog();

    void TryToInteractWithObject(GameObject* go);

    Position _cursorPosition;

    bool _playerTurnDone;    
};

#endif
