#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <ncurses.h>

#include <typeinfo>

class GameState
{
  public:
    virtual ~GameState() {}

    virtual void Init() {}
    virtual void Cleanup() {}
    virtual void Prepare() {}
    virtual void HandleInput() = 0;
    virtual void Update() = 0;

  protected:
    int _keyPressed;
};

#endif
