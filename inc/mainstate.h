#ifndef MAINSTATE_H
#define MAINSTATE_H

#include <vector>
#include <memory>

#include "gamestate.h"
#include "printer.h"
#include "game-object.h"
#include "util.h"

#define NUMPAD_7  KEY_A1
#define NUMPAD_8  KEY_A2
#define NUMPAD_9  KEY_A3
#define NUMPAD_4  KEY_B1
#define NUMPAD_5  KEY_B2
#define NUMPAD_6  KEY_B3
#define NUMPAD_1  KEY_C1
#define NUMPAD_2  KEY_C2
#define NUMPAD_3  KEY_C3

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
};

#endif
