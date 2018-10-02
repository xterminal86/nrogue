#ifndef MAINSTATE_H
#define MAINSTATE_H

#include <vector>
#include <memory>

#include "gamestate.h"
#include "printer.h"
#include "game-object.h"
#include "util.h"

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
    std::vector<std::unique_ptr<GameObject>> _mapObjects;
    std::unique_ptr<GameObject> _player;      

    std::string _debugInfo;
    std::string _inputState;

    const std::string kInputMoveState = "move";
    const std::string kInputLookState = "look";

    void ProcessMovement();
    void ProcessLook();
    void DrawMovementState();
    void DrawLookState();
    void MoveCursor(int dx, int dy);
    void DrawCursor();
    
    Position _cursorPosition;
};

#endif
