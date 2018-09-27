#ifndef MAINSTATE_H
#define MAINSTATE_H

#include <vector>
#include <memory>

#include "gamestate.h"
#include "printer.h"
#include "game-object.h"
#include "util.h"

class MainState : public GameState
{
  public:
    void Init() override;
    void HandleInput() override;
    void Update() override;
    
  private:
    std::vector<std::unique_ptr<GameObject>> _mapObjects;
    std::unique_ptr<GameObject> _player;      
};

#endif
