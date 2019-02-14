#ifndef GAMESTATE_H
#define GAMESTATE_H

#ifndef USE_SDL
#include <ncurses.h>
#else
#include <SDL2/SDL.h>
#endif

#include <typeinfo>

#ifdef USE_SDL
#define KEY_DOWN 0
#define KEY_UP   1
#endif

class GameState
{
  public:
    virtual ~GameState() {}

    virtual void Init() {}
    virtual void Cleanup() {}
    virtual void Prepare() {}
    virtual void HandleInput() = 0;
    virtual void Update(bool forceUpdate = false) = 0;

    int GetKeyDown();

  protected:
    int _keyPressed;    
};

#endif
