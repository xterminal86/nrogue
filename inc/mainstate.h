#ifndef MAINSTATE_H
#define MAINSTATE_H

#include <vector>
#include <memory>

#include "gamestate.h"
#include "util.h"
#include "player.h"

// 262, 259, 339, 260, 350, 261, 360, 258, 338

#if defined(__unix__) || defined(__linux__)

  #define NUMPAD_7  262
  #define NUMPAD_8  259
  #define NUMPAD_9  339
  #define NUMPAD_4  260
  #define NUMPAD_5  350
  #define NUMPAD_6  261
  #define NUMPAD_1  360
  #define NUMPAD_2  258
  #define NUMPAD_3  338

#else

  #define NUMPAD_7  KEY_A1
  #define NUMPAD_8  KEY_A2
  #define NUMPAD_9  KEY_A3
  #define NUMPAD_4  KEY_B1
  #define NUMPAD_5  KEY_B2
  #define NUMPAD_6  KEY_B3
  #define NUMPAD_1  KEY_C1
  #define NUMPAD_2  KEY_C2
  #define NUMPAD_3  KEY_C3

#endif

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
      INTERACT
    };

    std::vector<std::string> _gameLog;

    Player* _playerRef;

    std::string _debugInfo;

    InputStateEnum _inputState;

    void ProcessMovement();
    void ProcessLook();
    void ProcessInteraction();

    void DrawMovementState();
    void DrawLookState();
    void DrawInteractionState();

    void MoveCursor(int dx, int dy);
    void DrawCursor();
    void DrawGameObjects();

    void AddMessage(std::string message);
    void DisplayGameLog();

    Position _cursorPosition;

    bool _playerTurnDone;

    const int kMaxGameLogMessages = 10;
    const int kMessagesToDisplay = 5;
};

#endif
